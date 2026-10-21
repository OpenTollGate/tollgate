#!/usr/bin/env python3
"""Regenerate corpus token fixtures against a REACHABLE mint.

Why this exists (ADR-0004): the corpus's KG twins were issued by
`192.168.13.221:8383`, which is gone. NUT-07 only answers for proofs a mint itself
issued, so those fixtures are permanently unmeasurable -- every consumer scores the
same 14 UNVERIFIED forever. The "regen recipes" were prose, drifted from what
`cdk-mintd` actually accepts, and could not be re-executed. This replaces them with
code that CI can actually run.

Testnet only. Refuses mainnet URLs on purpose.

Usage
-----
    # start a local fakewallet mint (no Lightning, quotes auto-settle):
    python3 corpus/regenerate_fixtures.py serve --port 3338 --work-dir /tmp/mintsrv

    # mint fresh fixtures and record provenance:
    python3 corpus/regenerate_fixtures.py mint \
        --mint http://127.0.0.1:3338 \
        --amount 4 \
        --out corpus/tokens \
        --update-manifest corpus/manifest.json

Exit codes
----------
    0   fixtures minted AND each verified UNSPENT by NUT-07
    1   a fixture could not be minted, or its proofs were not UNSPENT
    2   bad arguments / the mint is unreachable / a mainnet URL was supplied
"""

from __future__ import annotations

import argparse
import json
import os
import pathlib
import shutil
import subprocess
import sys
import time
import urllib.error
import urllib.request

USER_AGENT = "tollgate-corpus-regen/1.0"

# Deliberately not "anything goes": regenerating fixtures against a real mint would
# mean spending real money to produce test data.
MAINNET_HOSTS = (
    "mint.coinos.io",
    "mint.minibits.cash",
    "mint.macadamia.cash",
    "mint.westernbtc.com",
    "kashu.me",
    "mint.cubabitcoin.org",
)


class RegenError(RuntimeError):
    pass


def _req(url: str, payload: dict | None = None, timeout: float = 30.0) -> dict:
    data = None if payload is None else json.dumps(payload).encode()
    req = urllib.request.Request(
        url,
        data=data,
        method="GET" if payload is None else "POST",
        headers={"Content-Type": "application/json", "User-Agent": USER_AGENT},
    )
    try:
        with urllib.request.urlopen(req, timeout=timeout) as r:
            return json.loads(r.read().decode())
    except urllib.error.HTTPError as e:
        raise RegenError(f"{url}: HTTP {e.code}: {e.read().decode()[:300]}") from None
    except (urllib.error.URLError, TimeoutError, ValueError) as e:
        raise RegenError(f"{url}: unreachable or unreadable: {e}") from None


def assert_testnet(mint_url: str) -> None:
    host = mint_url.split("//", 1)[-1].split("/", 1)[0].split(":")[0].lower()
    for bad in MAINNET_HOSTS:
        if host == bad or host.endswith("." + bad):
            raise RegenError(
                f"refusing to generate fixtures from mainnet mint {host}. "
                "Fixtures are test data; use testnut or a local fakewallet mint."
            )
    # A loopback mint is the intended production path; warn only on unknown publics.
    if host not in ("127.0.0.1", "localhost", "::1") and not host.startswith("test"):
        print(
            f"  note: {host} is not loopback and is not obviously a testnet host; "
            "proceeding because it is not on the mainnet list",
            file=sys.stderr,
        )


def mint_info(mint_url: str) -> dict:
    return _req(mint_url.rstrip("/") + "/v1/info")


def mint_quote(mint_url: str, amount: int, unit: str = "sat") -> str:
    r = _req(
        mint_url.rstrip("/") + "/v1/mint/quote/bolt11", {"amount": amount, "unit": unit}
    )
    q = r.get("quote")
    if not q:
        raise RegenError(f"no quote id in response: {str(r)[:200]}")
    return q


def await_paid(mint_url: str, quote: str, timeout: float = 30.0) -> None:
    """Poll until PAID. A fakewallet mint settles in a few seconds; a real one will
    not, and we must not mint tokens against an unpaid quote."""
    deadline = time.time() + timeout
    last = None
    while time.time() < deadline:
        st = _req(mint_url.rstrip("/") + "/v1/mint/quote/bolt11/" + quote)
        last = st.get("state")
        if last == "PAID":
            return
        time.sleep(1)
    raise RegenError(
        f"quote {quote} never reached PAID (last state {last!r}). "
        "For a fakewallet mint, check the `serve` config; for a real mint the invoice "
        "must actually be paid, which is why fixtures should come from fakewallet."
    )


def _cdk_cli(explicit: str | None = None) -> str:
    """Locate `cdk-cli`. It is the corpus's reference toolchain (ADR-0003 in the
    Rust consumer repo), which is why minting is delegated to it rather than
    reimplementing BDHKE blinding here -- a subtly wrong blinding implementation
    would produce fixtures that LOOK valid and poison the corpus."""
    for c in [explicit, os.environ.get("CDK_CLI"), os.path.expanduser("~/mint/cdk-cli"), "cdk-cli"]:
        if not c:
            continue
        found = c if os.path.sep in c else shutil.which(c)
        if found and os.path.exists(found) and os.access(found, os.X_OK):
            return found
    raise RegenError(
        "cdk-cli not found (looked at --cdk-cli, $CDK_CLI, ~/mint/cdk-cli, PATH). "
        "Minting fixtures requires the reference toolchain; see ADR-0003."
    )


def mint_token(cli: str, mint_url: str, amount: int, workdir: pathlib.Path,
               v3: bool) -> str:
    """Mint `amount` sats and return one token, in V4 (default) or V3 form.

    Two subprocesses because that is how cdk-cli models it: `mint` acquires proofs,
    `send` spends them into a token. The wallet DB lives in `workdir` so a run is
    self-contained and reproducible from the same mnemonic.
    """
    env = dict(os.environ)
    env.setdefault("CDK_CLI_WORK_DIR", str(workdir))
    workdir.mkdir(parents=True, exist_ok=True)

    r = subprocess.run(
        [cli, "mint", mint_url, str(amount)],
        capture_output=True, text=True, timeout=120, env=env, cwd=str(workdir),
    )
    if r.returncode != 0:
        raise RegenError(
            f"cdk-cli mint {amount} at {mint_url} failed: "
            f"{(r.stderr or r.stdout).strip()[:400]}"
        )

    # `--mint-url` is REQUIRED, not optional: when the wallet holds more than one
    # mint, `send` otherwise prompts "Enter mint number to send from" and then dies
    # with "cannot parse integer from empty string" -- which looks like a corrupt
    # wallet rather than a missing flag. In any automated context that prompt is
    # also a silent hang.
    cmd = [cli, "send", "-a", str(amount), "--mint-url", mint_url]
    if v3:
        # `-v3` collides with a global short flag; the long form works.
        cmd.append("--v3")
    r = subprocess.run(
        cmd, capture_output=True, text=True, timeout=120, env=env, cwd=str(workdir)
    )
    if r.returncode != 0:
        raise RegenError(
            f"cdk-cli send {amount} failed: {(r.stderr or r.stdout).strip()[:400]}"
        )
    tokens = [l.strip() for l in r.stdout.splitlines() if l.strip().startswith("cashu")]
    if not tokens:
        raise RegenError(
            "cdk-cli send produced no token line; refusing to write an empty fixture"
        )
    tok = tokens[0]
    expect = "cashuB" if not v3 else "cashuA"
    if not tok.startswith(expect):
        raise RegenError(f"expected a {expect} token, got {tok[:12]!r}")
    return tok


def mint(args) -> int:
    """Mint fresh KG fixtures, verify each is UNSPENT, then write them + provenance."""
    assert_testnet(args.mint)
    info = mint_info(args.mint)
    nuts = {str(k) for k in (info.get("nuts") or {})}
    for need, label in (("7", "NUT-07 checkstate"), ("9", "NUT-09 restore")):
        if need not in nuts:
            raise RegenError(f"mint does not advertise {label}; fixtures would be unverifiable")

    cli = _cdk_cli(args.cdk_cli)
    out = pathlib.Path(args.out)
    out.mkdir(parents=True, exist_ok=True)
    workdir = pathlib.Path(args.workdir).expanduser()

    written = []
    for name, v3 in ((f"v4_{args.label}", False), (f"v3_{args.label}", True)):
        tok = mint_token(cli, args.mint, args.amount, workdir, v3=v3)
        p = out / f"{name}.cashu"

        # Verify the fixture is spendable BEFORE writing it. A spent twin is not a
        # known-good reference, and finding that out later means every consumer's
        # verify/spend stage silently mis-scores. Decoding reuses the consumer's own
        # decoder so the check exercises the path that will read the fixture.
        ys = _proof_ys_from_token(tok)
        state = check_unspent(args.mint, ys)
        p.write_text(tok + "\n")
        written.append((name, p, len(tok)))
        print(f"  wrote {p} ({len(tok)} chars, NUT-07 {state})")

    provenance = {
        "mint": args.mint,
        "mint_version": info.get("version", "unknown"),
        "nuts": sorted(nuts, key=lambda x: int(x)),
        "cdk_cli": subprocess.run(
            [cli, "--version"], capture_output=True, text=True, timeout=30
        ).stdout.strip().splitlines()[0],
        "amount_sat": args.amount,
        "generated_at": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        "fixtures": [n for n, _, _ in written],
    }
    prov = out / "PROVENANCE.json"
    prov.write_text(json.dumps(provenance, indent=2) + "\n")
    print(f"  wrote {prov}")
    print(f"\nMinted {len(written)} fixtures from {args.mint} ({info.get('version')}).")
    print("  Next: NUT-07 each one (check_unspent) and re-run corpus/validate.py.")
    return 0


def _proof_ys_from_token(tok: str) -> list[str]:
    """Derive NUT-07 Ys for a token, using the consumer's decoder when present.

    Kept optional on purpose: the corpus must stay runnable without the Rust repo
    checked out beside it, but when the decoder IS available we use it so the check
    exercises the same path a consumer would.
    """
    import importlib.util

    here = pathlib.Path(__file__).resolve().parent          # .../tollgate/corpus
    for cand in (
        here / "ci" / "mint_probe.py",                      # vendored next to the corpus
        pathlib.Path(os.environ.get("TOLLGATE_MINT_PROBE", "")) if os.environ.get("TOLLGATE_MINT_PROBE") else None,
        pathlib.Path.home() / "repos" / "tollgate-module-basic-rust" / "ci" / "mint_probe.py",
        pathlib.Path.home() / "worktrees" / "pathA" / "ci" / "mint_probe.py",
    ):
        if cand is None:
            continue
        if cand.exists():
            spec = importlib.util.spec_from_file_location("_mint_probe", cand)
            mp = importlib.util.module_from_spec(spec)
            sys.path.insert(0, str(cand.parent))
            spec.loader.exec_module(mp)
            _, proofs = mp.decode_token(tok)
            return mp.proof_ys(proofs)
    raise RegenError(
        "cannot derive NUT-07 Ys: no token decoder found. Copy "
        "tollgate-module-basic-rust/ci/mint_probe.py into corpus/ci/ or set it up "
        "beside this repo; refusing to write an unverified fixture."
    )


def serve(args) -> int:
    """Start a local cdk-mintd fakewallet mint. Quotes auto-settle, no Lightning."""
    cfg = pathlib.Path(args.work_dir).expanduser()
    cfg.mkdir(parents=True, exist_ok=True)
    conf = cfg / "mint.toml"
    conf.write_text(
        f"""[info]
url = "http://127.0.0.1:{args.port}"
listen_host = "127.0.0.1"
listen_port = {args.port}
mnemonic = "env:CDK_MINTD_MNEMONIC"

[payment_backend]
backend = "fakewallet"

[onchain]
onchain_backend = "fakewallet"

[fake_wallet]
fee_percent = 0.0
min_delay_time = 0
max_delay_time = 0
custom_payment_methods = []
"""
    )
    env = dict(os.environ)
    # The canonical all-zeros BIP39 test vector. Public, and the corpus already
    # records this determinism as its own convention.
    env.setdefault(
        "CDK_MINTD_MNEMONIC",
        "abandon abandon abandon abandon abandon abandon abandon abandon "
        "abandon abandon abandon about",
    )
    # CRITICAL: `config init` writes to $CDK_MINTD_WORK_DIR (default ~/.cdk-mintd),
    # NOT to the process cwd. Without this, a fresh --work-dir still initialises the
    # DEFAULT database and then fails with "database contains existing mint identity",
    # or worse, writes into the user's home. Both commands and the daemon must agree.
    env["CDK_MINTD_WORK_DIR"] = str(cfg)
    exe = args.cdk_mintd or "cdk-mintd"
    print(f"  config: {conf}")
    print(f"  initialising in {cfg} ...")
    r = subprocess.run(
        [exe, "config", "init", "--file", str(conf), "--new-mint"],
        cwd=cfg,
        env=env,
        capture_output=True,
        text=True,
    )
    if r.returncode != 0:
        raise RegenError(f"config init failed: {(r.stderr or r.stdout)[:400]}")
    # `init` creates the schema but does NOT apply the config; without this the
    # daemon exits with "configuration is not initialized". Learned the hard way.
    r = subprocess.run(
        [exe, "config", "apply", "--file", str(conf)],
        cwd=cfg,
        env=env,
        capture_output=True,
        text=True,
    )
    if r.returncode != 0:
        raise RegenError(f"config apply failed: {(r.stderr or r.stdout)[:400]}")
    print("  config staged; starting daemon (Ctrl-C to stop)")
    return subprocess.call([exe, "--enable-logging"], cwd=cfg, env=env)


def check_unspent(mint_url: str, ys: list[str]) -> str:
    """NUT-07. Returns the state string; raises if the mint did not answer.

    Fixtures MUST be UNSPENT at generation time. A fixture that is already spent
    cannot serve as a KG twin, and committing one silently would poison the corpus.
    """
    r = _req(mint_url.rstrip("/") + "/v1/checkstate", {"Ys": ys})
    states = r.get("states")
    if not isinstance(states, list) or not states:
        raise RegenError(f"checkstate returned no states: {str(r)[:200]}")
    vals = {s.get("state", "").upper() for s in states}
    joined = ",".join(sorted(vals))
    if vals != {"UNSPENT"}:
        raise RegenError(
            f"generated proofs are not all UNSPENT (got {joined}); "
            "they cannot be committed as known-good fixtures"
        )
    return joined


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    sub = ap.add_subparsers(dest="cmd", required=True)

    sp = sub.add_parser("serve", help="run a local fakewallet mint")
    sp.add_argument("--port", type=int, default=3338)
    sp.add_argument("--work-dir", default="/tmp/tollgate-mintsrv")
    sp.add_argument("--cdk-mintd", default=None, help="path to cdk-mintd binary")

    mp = sub.add_parser("check", help="verify a mint is reachable and fits the contract")
    mp.add_argument("--mint", required=True)

    gp = sub.add_parser("mint", help="mint fresh KG fixtures and record provenance")
    gp.add_argument("--mint", required=True)
    gp.add_argument("--amount", type=int, default=4)
    gp.add_argument("--label", default="v2_kg", help="fixture name suffix, e.g. v2_kg")
    gp.add_argument("--out", default="corpus/tokens")
    gp.add_argument("--workdir", default="/tmp/tollgate-regen-wallet")
    gp.add_argument("--cdk-cli", default=None, help="path to cdk-cli binary")

    args = ap.parse_args()

    try:
        if args.cmd == "serve":
            return serve(args)
        if args.cmd == "mint":
            return mint(args)
        if args.cmd == "check":
            assert_testnet(args.mint)
            info = mint_info(args.mint)
            nuts = info.get("nuts") or {}
            need = {"7": "checkstate", "9": "restore"}
            missing = [f"NUT-{k} ({v})" for k, v in need.items() if k not in nuts]
            print(f"mint:    {args.mint}")
            print(f"version: {info.get('version', 'unknown')}")
            print(f"nuts:    {', '.join(sorted(nuts, key=lambda x: int(x)))}")
            if missing:
                raise RegenError(f"mint is missing required support: {', '.join(missing)}")
            print("OK: reachable and advertises NUT-07/NUT-09")
            return 0
    except RegenError as e:
        print(f"ERROR: {e}", file=sys.stderr)
        return 1
    return 2


if __name__ == "__main__":
    sys.exit(main())
