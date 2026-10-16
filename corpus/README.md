# Token conformance corpus (v0)

Canonical Cashu token fixtures that define the interop surface every
TollGate consumer must handle identically. Minted by **cdk-cli 0.18.0**
(CDK reference) against a deterministic fakewallet (fixed mnemonic,
cdk-mintd 0.18, V2 keysets — `01`-prefix 66-hex ids).

## Fixtures

| File | Shape | Backend verdict (rc5, verified 2026-10-16) | Portal verdict (current pin) |
|---|---|---|---|
| `v3_v2_valid.cashu` | V3 (`cashuA`), 16-hex id, base64 C | **1022 ACCEPTED** | **REJECTED** — client-side keyset check (CU109 card), never submits (portal#82) |
| `v4_v2_valid.cashu` | V4 (`cashuB`), CBOR, short id | **1022 ACCEPTED** | needs verification |
| `v3_v2_truncated_id.cashu` | V3 with id truncated to 16 hex (the #247 regression shape) | **1022 ACCEPTED** | **REJECTED** — CU109 card (the shape that hid the portal defect for months) |
| `v3_v2_hex_c.cashu` | V3 with hex-encoded C points (CU102 class) | **21023 payment-processing-failed** | n/a (never reaches portal validation) |

## Key findings this corpus documents

1. **The backend (Go, rc5) accepts BOTH id spellings** in V3 — the full
   66-hex and the 16-hex short form — and both valid token versions.
2. **The portal rejects both spellings** against V2-keyset mints: its
   client-side keyset comparison can't match any V2 id, so it renders the
   CU109 "outdated mint key" card and never submits. This is the browser
   e-cash payment defect (portal#82, PRTA#247, tmbg#952).
3. **4 of TollGate's 6 default mints serve V2 keysets today** — the portal
   gap is live, not theoretical.

## Verdict contract

Every TollGate consumer MUST produce these verdicts for these fixtures:

- valid shapes → **accept** (payment processes, kind 1022 at the backend)
- `hex_c` → **reject** as undecodable (CU102 class)
- unknown/other shapes → reject loudly, never silently

A consumer whose verdict set diverges from this table has an interop bug —
file it against this repo.

## Regenerating

```sh
# Deterministic given the fixed-mnemonic fakewallet:
cdk-cli mint <fakewallet-url> 11
cdk-cli send --mint-url <url> --amount 4 --include-fee            # V4
cdk-cli send --mint-url <url> --amount 4 --include-fee --v3       # V3
# truncated_id: decode V3, set each proof id[:16], re-encode
# hex_c: decode V3, replace each C (b64 point) with its .hex() form
```

## v1 scope and what's next

- v0 covers the **V2-keyset era** (the live default-mint reality).
- Add V1-keyset fixtures (compat era) once a deterministic V1 mint is in
  the fixture toolchain — cashu-audit (`Amperstrand/cashu-audit`) is the
  runner framework this corpus plugs into.
- Extend per-consumer verdict tables as consumers are tested.
