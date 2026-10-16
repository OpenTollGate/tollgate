# Known-good state (pinned 2026-10-16)

This document pins the **known-good interop state** for the TollGate
token surface. Per [#28](https://github.com/OpenTollGate/tollgate/issues/28)'s
direction — CDK as the reference, newest notes first — this state is
the contract every consumer is measured against. **Backwards
compatibility (V1-keyset mints, older wallets) is explicitly out of
scope for this pin** and lives deferred in `compat-era/`; it is
layered on only after this state is green across every consumer.

## Definition

**Known-good** = tokens minted by the **CDK reference toolchain
(cdk-cli 0.18.1)** against a **V2 keyset** (66-hex `01`-prefix id),
in the **NUT-00 V4 format (primary, the newest notes)** and the V3
format (current, still-served), are:

1. **accepted end-to-end by the Go reference backend** — HTTP-01
   POST / answers `200` with a kind-1022 Session event, and a
   degraded-mode TollGate refuses with `service-unavailable`
   **without consuming the proofs**;
2. **received cleanly by the CDK reference wallet** (decode, verify,
   swap);
3. **never silently diverged on** — any consumer whose verdict set
   differs from the tables below has an interop bug and it gets filed.

## The pin

| Leg | Pinned by | Evidence |
|---|---|---|
| Reference minter | cdk-cli 0.18.1 | minted the KG fixtures (`tokens/v3_v2_kg181.cashu`, `tokens/v4_v2_kg181.cashu`), proofs UNSPENT at commit |
| Reference mint | cdk-mintd 0.18.1 fakewallet, keyset `01e73e75…e6` (V2), unit sat | `manifest.json` reference_environment; regen recipes re-executed end-to-end |
| Wallet leg | cdk-cli 0.18.1 receive of sibling V3/V4 tokens | live, 2026-10-16 (4 sat / 2 sat received, incl. V4's 8-byte keyset-id prefix resolution) |
| Backend full-service leg (1022) | corpus v0 live capture on the same mint+keyset (cdk 0.18.0 minting) + origin/main source (`tollgate-module-basic-go` @ 27baf22, v0.6.0-rc3) | 2026-10-16; one-command re-verify below once the rig is back in full service |
| Backend degraded-refusal leg | **live, cdk 0.18.1 tokens POSTed through the rig** (`http://192.168.13.1:2121`): HTTP 400 + kind 21023 `service-unavailable`, proofs verified **UNSPENT** by NUT-07 afterwards | 2026-10-16 |

## Consumer scoreboard against this pin

| Consumer | Verdict on the KG set | Status |
|---|---|---|
| cdk-cli 0.18.1 (reference) | decodes, verifies, receives both formats | **green** (is the reference) |
| tollgate-module-basic-go (rc) | decodes both formats; 1022 per v0 live evidence + source; degraded-refusal-unconsumed live-verified | **green** (full-service 1022 re-verify pending rig recovery) |
| tollgate-captive-portal @ main | decodes both KG formats + all incident shapes (probed through the real decoder, 2026-10-16); raw POST / wire pinned by regression tests | **green at main** — regression lock portal#83 |
| tollgate-captive-portal @ repo pin 6c25834 | **passes the full corpus** (decode + POST wire, both id spellings, all incident shapes — verified 2026-10-16 by running the vendored-corpus suite at the pin) | **green** — the tmbg packaging pin contains the decoder fixes (#38/#55/#61) |
| tollgate-captive-portal @ lab rig build | rejected tokens in portal#82's capture (2026-10-16): CU109 + 0 POSTs | **RED — the incident**: portal#82 / PRTA#247 / tmbg#952. The repo pin already passes, so the failing deployment is an older/different build or a flow-level trigger (mint-option/unit path). Root cause OPEN: the rig's portal port was not serving for fingerprinting |
| Nutshell 0.21.0 (secondary) | decodes both formats (verify untested on spent v0 twins) | partially green — runner fills |

**Correction 2026-10-16 (v1.1):** an earlier revision of this
scoreboard claimed the repo pin "predates the main fixes" — that was
wrong: pin 6c25834 (2026-10-08) contains the decoder fixes and passes
the entire corpus decode+wire suite, including both id spellings
(`lifecycle/v3_v2_full66.json` is the wallet-side spelling fixture).
The remaining RED row is the lab rig's deployed build (portal#82's
capture), whose vintage is unidentified — the incident is NOT closed
by a pin bump. Closing it needs: (1) fingerprint the deployed build
(the rig's portal port was down at capture time), (2) exercise the
app-level flow (mint-option selection from a live 10021 advertisement)
against a V2-keyset mint.

## One-command re-verification (full-service leg)

With the rig in full service and the fakewallet up:

```sh
T=$(cdk-cli send --mint-url http://192.168.13.221:8383 --amount 4 --include-fee --v3 | tail -1)
curl -s -X POST http://192.168.13.1:2121/ -H 'Content-Type: text/plain' -d "$T"
# expect: HTTP 200, body kind=1022, tags device-identifier/allotment/metric/start-time
```

(the V4 variant drops `--v3`). The committed KG twins are never
consumed by verification runs — always send a sibling minted by the
recipe so the committed fixtures stay UNSPENT.

## Out of scope (deferred)

- **V1-keyset era** (16-hex ids, Version00 keysets) and older-wallet
  behaviors: `compat-era/`. Two Nutshell-minted V1 fixtures and their
  verdicts are parked there, marked `deferred`. Not part of this
  contract; not a consumer obligation until this document says so.
- The cdk-cli 0.18.1 `decode-token` deadlock on some non-KG shapes
  (unresolvable short ids, Nutshell-minted V1 tokens) is recorded in
  the vocab note and `lifecycle/v3_v2_truncated_id.json` as a
  candidate upstream bug — it does not affect the KG set, which cdk
  0.18.1 handles perfectly.
