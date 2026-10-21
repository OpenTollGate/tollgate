# TIP-04 - Vouchers: acquisition and redemption (DISCUSSION DRAFT)

**Status: proposal for discussion. Not a ratified TIP.** The number `TIP-04` is a placeholder.

## Problem

In a voucher design a TollGate issues its own ecash and a buyer must **acquire** that ecash before it
can be spent on access. `tollgate-rs` says its own limitation plainly: *"A buying node cannot yet pay a
gateway that sells for money"* — `merchantd` funds channels only from mints that give vouchers away
(`auto_accept`). The whole economy currently depends on a mint handing out free tokens.

This TIP proposes to specify only the **redemption** half, and to leave acquisition out of scope so any
Cashu-native mechanism can serve it.

## Scope split

| Half | In this TIP? | Why |
|---|---|---|
| **Redemption** — presenting a voucher to obtain access | **yes** | must be identical across implementations or nothing interoperates |
| **Acquisition** — how the buyer got the voucher | **no** | already solved by the wider Cashu ecosystem |

Acquisition is deliberately unspecified. A voucher is an ordinary Cashu token from `voucher_mint`
(TIP-03), so a buyer may obtain it by any means: an atomic Cashu-for-Cashu swap coordinated over Nostr
(the primitive Granola implements), a Lightning→mint flow, a transfer from a friend, or an operator's
own tooling. Naming one would couple this spec to a deployment.

## Proposal

### 1. A voucher is a Cashu token

- Issued by `voucher_mint` (the TollGate's own mint, TIP-03).
- **Denominated in the sold metric** (bytes or milliseconds), not in sat — one voucher is one unit.
- An ordinary Cashu token: any Cashu wallet can hold, send, or receive it without knowing TollGate
  exists.

### 2. Redemption is a normal payment

Presenting a voucher to the TollGate's payment surface opens or extends a budget. No new transport is
required: the existing TIP-02 payment path (raw token in the request body, TIP/HTTP-01) carries it.

The gateway MUST:
- verify the token against `voucher_mint` and its keysets;
- credit the budget by **the voucher's face value in the sold metric**;
- refuse `payment-error-mint-not-accepted` for any other mint;
- on a degraded/ambiguous outcome, **not** consume the proofs (the existing fund-safety invariant).

### 3. Redemption is idempotent

Re-presenting a spent voucher is `payment-error-token-spent`, never a second credit. The gateway MUST
treat the mint as the authority on proof state (NUT-07).

## Open questions

1. Should the TIP say anything at all about the **rate of exchange** between the pricing unit (sat) and
   the sold metric, or is that purely the advertisement's business (TIP-02 `price_per_step`)?
2. If a voucher is denominated in bytes, does it need a **deadline**, or is it bearer and perpetual?
3. Does a voucher need an explicit **expiry / refund** path for a closed channel's unspent remainder?
4. Should redemption of a voucher *always* open a channel, or is a one-shot voucher legitimate?
5. Is "acquisition out of scope" right, or does the spec need at least a **conformance pointer** so two
   buyers agree on what a valid acquisition looks like?

## Compatibility

Additive. A `payment_model: steps` TollGate never issues vouchers and is unaffected.
