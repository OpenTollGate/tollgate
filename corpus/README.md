# Token & HTTP conformance corpus (v1)

Canonical fixtures and vectors that define the interop surface every
TollGate consumer must handle identically. Owned by the protocol repo
per [#28](https://github.com/OpenTollGate/tollgate/issues/28): a
consumer whose verdict set diverges from this corpus has an interop bug —
file it against this repo, don't discover it in the field.

**Read [KNOWN-GOOD.md](KNOWN-GOOD.md) first.** It pins the current
contract: **newest notes** (NUT-00 V4 primary, V3 current) on **V2
keysets**, minted by the **CDK reference toolchain (cdk 0.18.1)**,
accepted end-to-end by the reference backend. Backwards compatibility
(V1-keyset era, older wallets) is **deferred** — see `compat-era/` —
and is layered on only after the known-good state is green across
every consumer. It is not green yet: the portal row is the open
incident.

Layout:

- `KNOWN-GOOD.md` — the pinned state, consumer scoreboard,
  one-command re-verification.
- `manifest.json` — index, `kg_state`, reference environment, verdict
  vocabularies, known divergences. The runner's entry point.
- `tokens/` — fixtures. KG twins (`*_kg181`, cdk-cli 0.18.1-minted,
  ship UNSPENT) plus the v0 regression shapes (spent originals).
- `lifecycle/` — per-fixture stage vectors: **decode → verify →
  checkstate → spend**, with per-consumer verdicts and evidence.
- `http/` — endpoint request/response vectors for the `:2121` API,
  pinned against the reference implementation (live captures +
  source citations) and the docs-series API truth
  (#23/#25/#26/#27).
- `events/` — wire grammar of the three Nostr event kinds that cross
  the HTTP surface (10021 / 1022 / 21023), including the p-tag rules.
- `compat-era/` — **deferred** V1-keyset fixtures and verdicts;
  parked data, not a consumer obligation.

Run the self-validation (the local CI for this repo):

```sh
python3 corpus/validate.py
```

## Token lifecycle verdicts

The v0.6.0 incident was stage confusion: three implementations each
passed their own tests because the broken pair was internally
consistent. The corpus therefore pins verdicts **per stage**, not just
end-to-end:

| Stage | Meaning | Vocabulary |
|---|---|---|
| decode | parse the token string offline | `decoded-v3` `decoded-v4` `reject-undecodable` `no-verdict-hang` |
| verify | offline field/keyset validation | `ok` `reject-keyset-format` `reject-keyset-unknown` `reject-c-encoding` `reject-amount-not-power-of-two` |
| checkstate | NUT-07 at the minting mint | `UNSPENT` `PENDING` `SPENT` |
| spend | submit as payment to a TollGate | `accepted-1022` `rejected-21023:<code>` `refused-degraded-unconsumed` |

Current verdict matrix (details + evidence in `lifecycle/*.json`; portal
verdicts split `@pin` (deployed router pin — the incident) vs `@main`
(probed through the real decoder 2026-10-16, regression lock portal#83)):

| Fixture | cdk-cli 0.18.1 | nutshell 0.21.0 | tmbg backend (rc) | portal @pin → @main | mint checkstate |
|---|---|---|---|---|---|
| `v4_v2_kg181` **(KG)** | minted + receive ✓ | unknown | decodes; 1022 per v0 + source; degraded-refusal-unconsumed live ✓ | unknown (same shape class as the v0 CU109 rejects) → **decoded-v4** | **UNSPENT** |
| `v3_v2_kg181` **(KG)** | minted + receive ✓ | unknown | decodes; 1022 per v0 + source; degraded-refusal-unconsumed live ✓ | unknown (same shape class) → **decoded-v3** | **UNSPENT** |
| `v3_v2_valid` (v0) | ok (receive ✓) | decoded | ok → 1022 (fresh) / `payment-error-token-spent` (as committed) | reject (CU109) → **ok** | SPENT |
| `v4_v2_valid` (v0) | ok (receive ✓) | decoded | ok → 1022 | unknown → **decoded-v4** | SPENT |
| `v3_v2_truncated_id` (v0, reframed) | decoded (warm) / hang (cold) | decoded | ok → 1022 | reject (CU109) → **ok** | SPENT |
| `v3_v2_hex_c` (v0, reclassified) | **reject at verify** | decoded / **reject at verify** | reject at verify → `payment-processing-failed` | unknown → **ok (defers C verdict to backend)** | SPENT |
| `compat-era/*` (deferred) | see `compat-era/lifecycle/` | decoded / ok / receive ✓ | unknown — out of KG scope | unknown | UNSPENT |

Key facts pinned by the lifecycle set:

1. **KG twins ship UNSPENT** (a single 4-sat proof each) and are never
   consumed by verification runs — runners replay with sibling tokens
   from the regen recipes (which the v1 capture re-executed
   end-to-end: mint → send V3/V4 → receive). The v0 fixtures' proofs
   are SPENT (verified via NUT-07) and serve as double-spend /
   decode / verify vectors.
2. **cdk serializes SHORT keyset ids in both token formats** — V3
   per proof (16 hex = version byte + 7-byte prefix of the canonical
   id) and V4 at the token level (8 bytes) — with C points as hex.
   Source: cdk nut00/mod.rs:519, nut02.rs:383. Every consumer must
   resolve short ids against the mint's keyset list; cdk-family mints
   resolve them natively. The v0-era claim that the 16-hex spelling
   was a test minter's "truncation" is RETRACTED — it is the
   reference toolchain's native output, byte-identical across the
   valid fixtures; the portal@pin incident was a short-id resolution
   failure (CU109 compared 16-hex token ids to 66-hex keyset-list
   ids).
3. `tokens/v3_v2_hex_c.cashu` is **not** a clean wrong-encoding
   probe: its C values are 49 garbage bytes (v0's regen recipe
   base64-decoded a hex string). It remains a valid invalid-shape
   vector — C unparseable as a point in ANY encoding — with the
   backend verdict `payment-processing-failed` (21023) recorded
   live; the verdict class is "C not a point", not "hex vs base64".
4. **Degraded-refusal-does-not-consume is verified live end-to-end
   with cdk 0.18.1**: sibling tokens POSTed through the rig were
   refused `service-unavailable` and re-checked UNSPENT at the mint
   (tollgate#27's fund-safety invariant, made checkable).
5. **cdk-cli 0.18.1 `decode-token` deadlocks on a cold wallet** (no
   cached keysets, wallet-state-dependent) for tokens whose keyset id
   cannot resolve from wallet state — the short-id V3 fixture and
   Nutshell-minted V1-era tokens — while decoding the same shapes
   with the mint's keysets cached. A hang is not a verdict: runners
   must apply timeouts, and the shape is a candidate upstream bug
   against cdk. One token shape, four consumer behaviors on record
   (accept / reject / decode / hang).
6. DLEQ presence is wallet-dependent (cdk-minted proofs carry dleq,
   Nutshell's omit it) — consumers must treat it as optional.

## HTTP endpoint vectors

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

## Event grammar

`events/` pins the three kinds crossing the HTTP surface, with the
p-tag rule as the load-bearing clause (tollgate#26, from the #16
divergence): **p is optional and pubkey-only** — present only when the
payment carried a customer Nostr identity, never a MAC. Two live
divergences are recorded with issue links:

- `DIV-2026-10-mac-in-p-tag` — reference backend emits the customer
  MAC in p on kind-1022 sessions (tmbg#949, fix #951 open at capture)
  and addressed kind-21023 notices (tmbg#950). Observed live on the
  notice path during the captures.
- `DIV-2026-10-portal-v2-keyset` — portal rejects every V2-keyset
  token at its client-side keyset check (portal#82, PRTA#247,
  tmbg#952). **This is the blocker for the known-good state.**

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
5. reports verdict-set divergence against the reference — with
   `kg_state.open_gaps` and `known_divergences` suppressing
   already-filed bugs so a lane run produces new issues, not
   duplicates.

## What's next

- Close the portal gap for real: portal **main AND the repo pin
  6c25834** pass the corpus (probed 2026-10-16, both id spellings) —
  the failing deployment in portal#82's capture is an older/different
  build or a flow-level trigger. Next: fingerprint the deployed build
  and exercise the app-level mint-option flow against a live V2-keyset
  advertisement.
- Re-verify the backend full-service leg (one-command recipe in
  KNOWN-GOOD.md) when the rig is back in full service.
- Fill the remaining `unknown` verdicts (portal V4 decode, Nutshell
  verify on the KG twins) as the runner lane comes up.
- Only after KG is green everywhere: lift `compat-era/` out of
  deferral and pin the V1-keyset layer on top.
- Spec gaps this corpus documented are now filed: POST / Content-Type
  contract (#31), degraded-mode /ln-invoice bare 400 (#32), uncovered
  session/ticket + session/rebind endpoints (#33; session-state itself
  is #24).
