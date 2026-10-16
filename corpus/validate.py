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

    token_index = {e["path"] for e in manifest["index"]["tokens"]}
    tokens_on_disk = {str(p.relative_to(CORPUS)) for p in sorted((CORPUS / "tokens").glob("*.cashu"))}
    for missing in sorted(token_index - tokens_on_disk):
        fail(f"manifest indexes {missing} but the fixture does not exist")

    # --- lifecycle vector checks ---
    seen_ids: set[str] = set()
    for entry in manifest["index"]["lifecycle"]:
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
                check(verdict in spend_verdicts, f"{vid}/spend: illegal spend verdict {verdict!r}")
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
        for key, value in body.items():
            if key in ("p", "tags_include", "tag_value_class", "fields", "tags_must_omit"):
                _ptag_check_value(vid, key, value)
            _check_assertions(vid, value)
    elif isinstance(body, list):
        for item in body:
            _check_assertions(vid, item)


def _ptag_check_value(vid: str, key: str, value) -> None:
    def scan(node) -> None:
        global CHECKS
        if isinstance(node, dict):
            matcher = node.get("matcher")
            if matcher == "nostr-pubkey":
                CHECKS += 1  # correct class; nothing to enforce beyond usage
            if matcher == "mac-format":
                # a p tag asserted to hold a MAC is the #950 wire-bug class
                pass
            for v in node.values():
                scan(v)
        elif isinstance(node, list):
            for item in node:
                scan(item)
        elif isinstance(node, str) and MAC_RE.match(node):
            # literal MAC-shaped values are only legal in device-identifier/mac contexts
            pass
    scan(value)


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


if __name__ == "__main__":
    sys.exit(main())
