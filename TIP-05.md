# TIP-05 - Payment channels: lifecycle and durability (DISCUSSION DRAFT)

**Status: proposal for discussion. Not a ratified TIP.** The number `TIP-05` is a placeholder.
**Do not merge before `tollgate-rs` reaches 1.0** — see "Timing" at the end.

## Problem

`tollgate-rs` settles over a Cashu Spilman payment channel: a 2-of-2 multisig token, funded by the
buyer, from which the provider draws as it delivers. That channel is the central object of its design.
The TIPs say nothing about channels at all — TIP-02 stops at *"The customer sends a Cashu token
directly over the TollGate's supported transport(s)."*

So the one object a channel design cannot work without is unspecified, and a second implementation
would have to guess. What follows is the minimum worth pinning.

## Proposal — five statements

These are deliberately *requirements about behaviour*, not a wire format. The wire format belongs in
each implementation until the design settles (see Timing).

### 1. Funding order

The channel MUST be funded and confirmed **before** the first unit is drawn. A provider MUST NOT deliver
against an unfunded channel, and MUST NOT count delivery as owed until funding is observed.

### 2. Durability — the sharp one

A provider MUST persist, **before acknowledging** a draw, enough state to reconstruct the channel after a
restart: the channel identity, the funded amount, the amount drawn, and the buyer's refund terms.

> `tollgate-rs` currently records the opposite as a known limit: *"Channels are kept in memory only. A
> `tollgated` that restarts loses its open channels. Budgets survive; channel state does not."*

A spec that stays silent lets that ship as conformant. **Whether durability is REQUIRED or RECOMMENDED
is the main question this PR asks.**

### 3. Rollover

When a channel's funded amount is exhausted, or its deadline passes, the provider MUST define:
- whether the unspent remainder returns to the buyer, carries into a successor channel, or is forfeit;
- what the buyer must sign to open the successor, and in what order relative to the last draw.

### 4. Unilateral refund

A buyer MUST be able to recover the unspent remainder **without the provider's cooperation**, after a
negotiated locktime. An implementation that requires the provider to co-sign a refund is not a payment
channel; it is a prepayment.

The worked model: per-leg refund keys plus negotiated locktimes, as in atomic Cashu swaps.

### 5. What is NOT required

- A specific multisig construction or script.
- A specific draw cadence.
- On-chain anything.

## Open questions

1. **Durability**: REQUIRED, RECOMMENDED, or "implementation-defined with a stated recovery contract"?
   This is the difference between a channel and a prepayment.
2. Is a channel one per device, or may several sessions share one channel?
3. Should the TIP name a maximum locktime, or leave it to the advertisement?
4. Does rollover preserve queue position / remaining budget, or start fresh?

## Timing — why this one is a draft among drafts

`tollgate-rs` is `0.1.0-rc.2` and its release notes warn that *"the Rust APIs, the configuration keys and
the wire format"* may all change before 1.0. Pinning channel semantics now risks standardising a moving
target. This PR exists to start the conversation and to record what a spec will need to decide — **not**
to freeze anything. It should stay a draft until `tollgate-rs` 1.0 or an explicit decision to stabilise.

## Compatibility

Additive and inert for `payment_model: steps` TollGates, which have no channels.
