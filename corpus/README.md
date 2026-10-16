# Token & HTTP conformance corpus (v1)

Canonical fixtures and vectors that define the interop surface every
TollGate consumer must handle identically. Owned by the protocol repo
per [#28](https://github.com/OpenTollGate/tollgate/issues/28): a
consumer whose verdict set diverges from this corpus has an interop bug —
file it against this repo, don't discover it in the field.

- `manifest.json` — index, reference environment, verdict vocabularies,
  known divergences. Start here.
- `tokens/` — raw token fixtures (v0 set, cdk-cli-minted against the
  deterministic fakewallet, V2-keyset era).
- `lifecycle/` — per-fixture stage vectors: **decode → verify →
  checkstate → spend**, with per-consumer verdicts and evidence.
- `http/` — endpoint request/response vectors for the `:2121` API,
  pinned against the reference implementation (live captures +
  source citations) and the docs-series API truth
  (#23/#25/#26/#27).
- `events/` — wire grammar of the three Nostr event kinds that cross
  the HTTP surface (10021 / 1022 / 21023), including the p-tag rules.

Run the self-validation (the local CI for this repo):

```sh
python3 corpus/validate.py
```

## Token lifecycle verdicts (v1)

The v0.6.0 incident was stage confusion: three implementations each
passed their own tests because the broken pair was internally
consistent. The corpus therefore pins verdicts **per stage**, not just
end-to-end:

| Stage | Meaning | Vocabulary |
|---|---|---|
| decode | parse the token string offline | `decoded-v3` `decoded-v4` `reject-undecodable` |
| verify | offline field/keyset validation | `ok` `reject-keyset-format` `reject-keyset-unknown` `reject-c-encoding` `reject-amount-not-power-of-two` |
| checkstate | NUT-07 at the minting mint | `UNSPENT` `PENDING` `SPENT` |
| spend | submit as payment to a TollGate | `accepted-1022` `rejected-21023:<code>` `refused-degraded-unconsumed` |

Current verdict matrix (details + evidence in `lifecycle/*.json`):

| Fixture | cdk-cli | tmbg backend (rc) | portal (pin) | mint checkstate |
|---|---|---|---|---|
| `v3_v2_valid` | ok | ok → 1022 (fresh) / `payment-error-token-spent` (as committed) | **reject at verify (CU109)** | SPENT |
| `v4_v2_valid` | ok | ok → 1022 | needs verification | unknown (V4 tooling pending) |
| `v3_v2_truncated_id` | unknown | ok → 1022 (resolves 16-hex prefix to canonical id) | **reject at verify (CU109)** | SPENT |
| `v3_v2_hex_c` | unknown | reject at verify → `payment-processing-failed` | unknown | SPENT (shares secrets with `v3_v2_valid`) |

Key facts pinned by the lifecycle set:

1. The committed fixtures' proofs are **SPENT** at the fakewallet
   (verified 2026-10-16 via NUT-07) — they are double-spend and
   decode/verify vectors, not fresh-payment vectors. Regeneration
   recipes are in each lifecycle file.
2. The backend accepted the truncated-id token AND the mint recorded
   the spend — so the swap request carried the canonical id; the
   backend resolves the 16-hex prefix (mechanism inferred, source
   confirmation pending).
3. `refused-degraded-unconsumed` is asserted with a checkstate
   post-condition: a degraded-mode refusal must leave every proof
   UNSPENT (tollgate#27's fund-safety invariant, made checkable).

## HTTP endpoint vectors (v1)

One file per endpoint family; every vector carries `spec_refs`,
`preconditions`, machine-checkable response assertions, and `evidence`
(`live` capture / `source` citation / `spec` reference / `reported`).
Highlights the capture runs surfaced (2026-10-16, rig
`http://192.168.13.1:2121`, source `tollgate-module-basic-go`
origin/main @ 27baf22):

- **POST / media-type gate**: only `text/plain`, `application/json`,
  or absent pass; anything else is `415 {"error":"Unsupported Media
  Type"}` — spec-silent, pinned as de-facto.
- **Envelope vs degraded ordering**: a kind=21000 JSON body without a
  payment tag fails `invalid-event` even in degraded mode (extraction
  precedes the gate); a raw garbage body in degraded mode answers
  `service-unavailable` (the gate precedes token processing).
- **POST / has no 413**: bodies over 1 MiB answer 400
  `invalid-request` (as a kind-21023 notice), unlike /ln-invoice's
  413 at 8 KiB.
- **Rate limiting** (GET+POST on /): 429 with `Retry-After: 6` and a
  JSON body served as `text/plain` (http.Error quirk — pinned, not
  endorsed).
- **ln-invoice refusals** carry `{status:0, code, retry_after}`
  mirroring the `Retry-After` header; the settled-but-not-applied
  answer is **503 `access-grant-failed` with a do-not-pay-again
  message** (paying again is the double-pay footgun the code guards).
- **The / mux is a catch-all**: unknown GET paths serve the
  discovery/degraded slot, not 404.
- **`GET /session-state`, `/session/ticket`, `/session/rebind`** are
  served by the reference without spec coverage (gap #24 /
  uncovered) — pinned with explicit `spec_status` so the protocol
  repo can decide, not inherit.

## Event grammar (v1)

`events/` pins the three kinds crossing the HTTP surface, with the
p-tag rule as the load-bearing clause (tollgate#26, from the #16
divergence): **p is optional and pubkey-only** — present only when the
payment carried a customer Nostr identity, never a MAC. Two live
divergences are recorded with issue links:

- `DIV-2026-10-mac-in-p-tag` — reference backend emits the customer
  MAC in p on kind-1022 sessions (tmbg#949, fix #951 open at capture)
  and addressed kind-21023 notices (tmbg#950). Observed live on the
  notice path during the v1 capture.
- `DIV-2026-10-portal-v2-keyset` — portal rejects every V2-keyset
  token at its client-side keyset check (portal#82, PRTA#247,
  tmbg#952).

## Runner consumption contract

The corpus is data first: a runner (the planned consumer-conformance
lane in [Amperstrand/cashu-audit](https://github.com/Amperstrand/cashu-audit))
walks `manifest.json`, and for each vector

1. sets up the `preconditions` (mint fixtures via the regen recipes,
   gateway mode, quota state),
2. drives the consumer under test (HTTP surface or token
   decoder/wallet),
3. evaluates the response assertions using the matcher vocabulary in
   the manifest (`exact`, `prefix`, `regex`, `integer`, `mac-format`,
   `nostr-pubkey`, `nonempty`, `omit`, `one-of`),
4. checks any `post_conditions`,
5. reports verdict-set divergence against the reference — and the
   `known_divergences` entries tell it which divergences are already
   filed, so a lane run produces new issues, not duplicates.

## v1 scope and what's next

- v1 covers the **V2-keyset era** (the live default-mint reality) plus
  the full HTTP/event surface above. Token fixtures are minted by
  **cdk-cli 0.18.0** (CDK reference; Nutshell secondary), refreshed on
  every NUT-00/NUT-02 change.
- V1-keyset (compat-era) fixtures follow once a deterministic V1 mint
  is in the fixture toolchain (v0's open item, unchanged).
- Fill the `unknown` verdicts (portal V4 decode, cdk-cli on the
  deliberately-invalid shapes) as the runner lane comes up.
- Unresolved spec gaps this corpus now documents as issues-to-file:
  POST / Content-Type contract (415 gate), degraded-mode /ln-invoice
  answering a bare 400 without a code, and the uncovered
  session/ticket/rebind endpoints.
