#!/usr/bin/env python3
"""Corpus self-validation (local CI).

Run from the repo root:  python3 corpus/validate.py

Checks structural invariants of the conformance corpus without external
dependencies: manifest/index consistency, fixture reference resolution,
verdict vocabulary legality, evidence presence, and the p-tag pubkey
invariant (a Nostr p tag is never asserted or excused to hold a MAC).

Exit code 0 = corpus is internally consistent.
"""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

CORPUS = Path(__file__).resolve().parent
FAILURES: list[str] = []
CHECKS = 0

MAC_RE = re.compile(r"^[0-9A-Fa-f]{2}(?::[0-9A-Fa-f]{2}){5}$")
PUBKEY_RE = re.compile(r"^[0-9a-f]{64}$")


def fail(msg: str) -> None:
    FAILURES.append(msg)


def check(cond: bool, msg: str) -> bool:
    global CHECKS
    CHECKS += 1
    if not cond:
        fail(msg)
    return cond


def load(path: Path):
    global CHECKS
    CHECKS += 1
    try:
        with open(path, encoding="utf-8") as fh:
            return json.load(fh)
    except (OSError, json.JSONDecodeError) as exc:
        fail(f"{path.relative_to(CORPUS)}: unreadable/invalid JSON ({exc})")
        return None


def main() -> int:
    manifest = load(CORPUS / "manifest.json")
    if manifest is None:
        print("\n".join(FAILURES))
        return 1

    vocab = manifest["vocabularies"]
    evidence_kinds = set(vocab["evidence_kinds"])
    stage_vocabs = {
        "decode": set(vocab["decode_verdicts"]),
        "verify": set(vocab["verify_verdicts"]),
        "checkstate": set(vocab["checkstate_verdicts"]),
    }
    spend_reject_codes = set(vocab["spend_reject_codes"])
    spend_verdicts = set(vocab["spend_verdicts"])

    # --- index <-> filesystem consistency (both directions) ---
    for group, dirname in [("lifecycle", "lifecycle"), ("http", "http"), ("events", "events")]:
        indexed = {e["path"] for e in manifest["index"][group]}
        on_disk = {str(p.relative_to(CORPUS)) for p in sorted((CORPUS / dirname).glob("*.json"))}
        for missing in sorted(indexed - on_disk):
            fail(f"manifest indexes {missing} but the file does not exist")
        for unindexed in sorted(on_disk - indexed):
            fail(f"{unindexed} exists but is not in manifest.index.{group}")
        for path in sorted(indexed & on_disk):
            check(load(CORPUS / path) is not None, f"{path} must parse")

    compat = manifest["index"].get("compat_era")
    if isinstance(compat, dict):
        check(bool(compat.get("status")), "compat_era must carry an explicit status")
        comp_tokens = {e["path"] for e in compat.get("tokens", [])}
        comp_lifecycle = {e["path"] for e in compat.get("lifecycle", [])}
        for path in sorted(comp_lifecycle):
            check((CORPUS / path).is_file(), f"compat_era indexes {path} but the file does not exist")
            check(load(CORPUS / path) is not None, f"{path} must parse")
        for path in sorted(comp_tokens):
            check((CORPUS / path).is_file(), f"compat_era indexes {path} but the fixture does not exist")
        on_disk_lc = {str(p.relative_to(CORPUS)) for p in sorted((CORPUS / "compat-era" / "lifecycle").glob("*.json"))} if (CORPUS / "compat-era" / "lifecycle").is_dir() else set()
        on_disk_tk = {str(p.relative_to(CORPUS)) for p in sorted((CORPUS / "compat-era" / "tokens").glob("*.cashu"))} if (CORPUS / "compat-era" / "tokens").is_dir() else set()
        for unindexed in sorted(on_disk_lc - comp_lifecycle):
            fail(f"{unindexed} exists but is not in manifest.index.compat_era.lifecycle")
        for unindexed in sorted(on_disk_tk - comp_tokens):
            fail(f"{unindexed} exists but is not in manifest.index.compat_era.tokens")
        check(bool(comp_tokens) == bool(on_disk_tk), "compat_era tokens index and disk must agree")

    token_index = {e["path"] for e in manifest["index"]["tokens"]}
    tokens_on_disk = {str(p.relative_to(CORPUS)) for p in sorted((CORPUS / "tokens").glob("*.cashu"))}
    for missing in sorted(token_index - tokens_on_disk):
        fail(f"manifest indexes {missing} but the fixture does not exist")
    for unindexed in sorted(tokens_on_disk - token_index):
        fail(f"{unindexed} exists but is not in manifest.index.tokens")

    lifecycle_entries = list(manifest["index"]["lifecycle"])
    if isinstance(compat, dict):
        lifecycle_entries += compat.get("lifecycle", [])
    seen_ids: set[str] = set()
    for entry in lifecycle_entries:
        path = CORPUS / entry["path"]
        vec = load(path)
        if vec is None:
            continue
        vid = vec.get("id", entry["path"])
        check(vid not in seen_ids, f"duplicate lifecycle id {vid}")
        seen_ids.add(vid)

        # fixture reference resolves
        fixture = (path.parent / vec["fixture"]).resolve()
        check(fixture.is_file(), f"{vid}: fixture ref {vec['fixture']} does not resolve")

        stages = vec.get("stages", {})
        for stage in ("decode", "verify", "checkstate", "spend"):
            check(stage in stages, f"{vid}: missing stage {stage}")
        for stage, allowed in stage_vocabs.items():
            block = stages.get(stage)
            if not block:
                continue
            verdicts = block.get("verdicts")
            if verdicts is None:
                continue
            values = verdicts if isinstance(verdicts, list) else [verdicts]
            for item in values:
                for consumer, verdict in item.items():
                    if consumer == "when":
                        continue
                    check(
                        verdict in allowed or verdict == "unknown",
                        f"{vid}/{stage}: illegal verdict {verdict!r} for {consumer}",
                    )
            check(
                bool(block.get("evidence")) or all(v == "unknown" for v in _flat_verdicts(verdicts)),
                f"{vid}/{stage}: known verdicts require at least one evidence entry",
            )
        spend = stages.get("spend", {})
        for item in _spend_verdict_list(spend):
            verdict = item.get("tollgate-module-basic-go@rc")
            if verdict is not None:
                check(verdict in spend_verdicts or verdict == "unknown", f"{vid}/spend: illegal spend verdict {verdict!r}")
            code = item.get("reject_code")
            if code is not None:
                check(code in spend_reject_codes, f"{vid}/spend: unknown reject code {code!r}")
        for stage_block in stages.values():
            for ev in stage_block.get("evidence", []):
                check(ev.get("kind") in evidence_kinds, f"{vid}: illegal evidence kind {ev.get('kind')!r}")

    # --- http + events vector checks ---
    for group in ("http", "events"):
        for entry in manifest["index"][group]:
            path = CORPUS / entry["path"]
            data = load(path)
            if data is None:
                continue
            file_spec_refs = data.get("spec_refs") or ([data["spec_status"]] if data.get("spec_status") else [])
            vector_ids: dict[str, dict] = {}
            for vec in data.get("vectors", []):
                vid = vec.get("id")
                if isinstance(vid, str):
                    vector_ids[vid] = vec
            for vec in data.get("vectors", []):
                vid = vec.get("id")
                check(isinstance(vid, str) and vid, f"{entry['path']}: vector without id")
                if vid in seen_ids:
                    fail(f"duplicate vector id {vid}")
                seen_ids.add(vid)
                check(
                    bool(vec.get("spec_refs") or vec.get("spec_status") or file_spec_refs),
                    f"{vid}: needs spec_refs (own, file-level, or an explicit spec_status for uncovered surface)",
                )
                check(bool(vec.get("evidence")), f"{vid}: needs at least one evidence entry")
                for ev in vec.get("evidence", []):
                    check(ev.get("kind") in evidence_kinds, f"{vid}: illegal evidence kind {ev.get('kind')!r}")
                resp = vec.get("response")
                if isinstance(resp, dict):
                    if "same_as" in resp:
                        target = resp["same_as"]
                        check(target in vector_ids, f"{vid}: same_as target {target!r} not found in {entry['path']}")
                    else:
                        check("status" in resp, f"{vid}: response without status")
                    _check_assertions(vid, resp.get("body"))
                    for div in vec.get("divergences", []):
                        check(bool(div.get("ref")), f"{vid}: divergence entry without ref")
                elif isinstance(resp, str):
                    check(bool(resp.strip()), f"{vid}: descriptive response must be non-empty")
                elif resp is not None:
                    check(False, f"{vid}: response must be an object, string, or absent")

    # --- p-tag pubkey invariant across the whole corpus ---
    _ptag_scan(CORPUS / "http")
    _ptag_scan(CORPUS / "events")

    # --- fixture provenance: is the reference mint still reachable? ---
    # ADR-0004. This is the check that would have caught the current state, where
    # every KG twin was issued by a mint that no longer exists and NUT-07 therefore
    # cannot answer for any of them -- which silently caps every consumer at the
    # same permanently-UNVERIFIED score.
    _check_reference_mint(manifest)

    print(f"corpus self-validation: {CHECKS} checks, {len(FAILURES)} failure(s)")
    for msg in FAILURES:
        print(f"  FAIL: {msg}")
    return 1 if FAILURES else 0


def _flat_verdicts(verdicts):
    if isinstance(verdicts, dict):
        return [v for k, v in verdicts.items() if k != "when"]
    out = []
    for item in verdicts:
        for k, v in item.items():
            if k not in ("when", "evidence"):
                out.append(v)
    return out


def _spend_verdict_list(spend: dict) -> list:
    verdicts = spend.get("verdicts")
    if verdicts is None:
        return []
    return verdicts if isinstance(verdicts, list) else [verdicts]


def _check_assertions(vid: str, body) -> None:
    """Walk a response-body assertion tree; enforce the p-tag invariant."""
    if body is None:
        return
    if isinstance(body, dict):
        _ptag_check_value(vid, body)
        for value in body.values():
            _check_assertions(vid, value)
    elif isinstance(body, list):
        head = body[0] if body else None
        if head == "p":
            _ptag_enforce(vid, body[1:])
        for item in body:
            _check_assertions(vid, item)


def _ptag_check_value(vid: str, body: dict) -> None:
    tvc = body.get("tag_value_class")
    if isinstance(tvc, dict) and "p" in tvc:
        _ptag_enforce(vid, [tvc["p"]])
    fields = body.get("fields")
    if isinstance(fields, dict) and "p" in fields:
        _ptag_enforce(vid, [fields["p"]])


def _ptag_enforce(vid: str, assertions) -> None:
    """A p tag may only be asserted as a Nostr pubkey (or non-empty/one-of);
    asserting a MAC in p is the tmbg#950 wire-bug class and must fail."""
    global CHECKS
    for item in assertions:
        if not isinstance(item, dict) or "matcher" not in item:
            continue
        CHECKS += 1
        matcher = item["matcher"]
        if matcher == "mac-format":
            fail(f"{vid}: p tag asserted with a MAC value — the tmbg#950 wire-bug class, forbidden by TIP-01")
        elif matcher not in ("nostr-pubkey", "nonempty", "one-of"):
            fail(f"{vid}: p tag asserted with matcher {matcher!r}; only nostr-pubkey/nonempty/one-of are legal")


def _ptag_scan(directory: Path) -> None:
    for path in sorted(directory.glob("*.json")):
        data = load(path)
        if data is None:
            continue
        text = json.dumps(data)
        for m in re.finditer(r'"p"\s*,\s*"([^"]*)"', text):
            val = m.group(1)
            if val and PUBKEY_RE.match(val) is None and not val.startswith("<"):
                fail(f"{path.relative_to(CORPUS)}: p tag carries non-pubkey literal {val!r}")


def _check_reference_mint(manifest: dict) -> None:
    """Fixture provenance: a reference mint must be declared, reachable and capable.

    Deliberately offline-tolerant: an unreachable mint is reported as a FAILURE of
    this check, but the check can be skipped entirely with TOLLGATE_SKIP_MINT_CHECK=1
    so that an air-gapped CI run is honest rather than red for the wrong reason.

    What it asserts:
      * `reference_environment` names at least one mint URL;
      * the URL's host is not one of the known-dead lab addresses, unless it is
        loopback (a local fakewallet mint is the intended replacement);
      * if network is allowed, the mint answers /v1/info and advertises NUT-07 and
        NUT-09, because without those the verify/spend stages are unmeasurable.
    """
    import os as _os

    ref = (manifest.get("reference_environment") or {}).get("mint") or {}
    url = ref.get("url")
    if not url:
        check(False, "reference_environment.mint.url is absent: fixtures have no declared origin")
        return
    check(True, f"reference mint declared: {url}")

    host = url.split("//", 1)[-1].split("/", 1)[0].split(":")[0].lower()
    dead_lab = {"192.168.13.221", "192.168.13.231", "192.168.13.1"}
    if host in dead_lab:
        # Not a hard failure -- the corpus may be documenting a historical capture --
        # but it must be visible, because it means NUT-07 cannot be re-run.
        FAILURES.append(
            f"reference mint {host} is a lab address; fixtures issued there cannot be "
            "NUT-07 verified anywhere. Regenerate with corpus/regenerate_fixtures.py."
        )
        return

    if _os.environ.get("TOLLGATE_SKIP_MINT_CHECK") == "1":
        print("  (mint reachability check skipped: TOLLGATE_SKIP_MINT_CHECK=1)")
        return

    import json as _json
    import urllib.error as _uerr
    import urllib.request as _req

    try:
        r = _req.Request(
            url.rstrip("/") + "/v1/info",
            headers={"User-Agent": "tollgate-corpus-validate/1.0"},
        )
        with _req.urlopen(r, timeout=10) as resp:
            info = _json.loads(resp.read().decode())
    except (_uerr.URLError, TimeoutError, ValueError, OSError) as e:
        check(False, f"reference mint {url} is unreachable ({e}); verify/spend cannot be measured")
        return

    nuts = {str(k) for k in (info.get("nuts") or {})}
    check("7" in nuts, f"reference mint {url} does not advertise NUT-07 (checkstate)")
    check("9" in nuts, f"reference mint {url} does not advertise NUT-09 (restore)")
    print(f"  reference mint {url} reachable: {info.get('version', 'unknown')}")


if __name__ == "__main__":
    sys.exit(main())
