# TIP-06 - The accounting rule (DISCUSSION DRAFT)

**Status: proposal for discussion. Not a ratified TIP.** The number `TIP-06` is a placeholder.

## Problem

The existing TIPs describe *what is bought* (`step_size`, `price_per_step`) and *what is granted*
(`allotment`). They do not describe **how a running balance is debited as the resource is used**.

A correct rule already exists in `tollgate-rs`, stated in one sentence:

> every second the provider draws the larger of what the payer moved and the rate it reserved

Two implementations that each invent their own version of that will agree on the happy path and
disagree on every edge: an unused reservation, a mid-second rate change, a `burst`, a rounding
boundary. Those disagreements are invisible until a customer disputes a balance.

## Proposal

### The rule

For each accounting interval, the provider debits:

```
draw = max(observed_usage_in_interval, reserved_rate * interval_length)
```

capped at the funded budget. A reservation therefore buys *time at a speed*; reserving nothing is pay
per use.

### Terms that MUST be defined, because they are where implementations diverge

| Term | What must be said |
|---|---|
| **interval** | fixed cadence or continuous? does a partial interval debit? |
| **reserved_rate** | per what unit — per second, per interval? |
| **burst** | an allowance above the reservation; does a burst debit the budget, and over what window does it replenish? |
| **rounding** | `tollgate-rs` records: *"Draws are counted in whole units, rounded down, so a reserved rate of a few units a second can come to nothing over a short stretch."* A spec must **state** the rounding, not leave it per-implementation — a floored draw and a rounded draw differ by measurable money |
| **unit of account** | the sold metric's base unit (byte / millisecond), distinct from the pricing unit (sat) |
| **exhaustion** | what happens at budget exhaustion — refuse, throttle to zero, or close? |

### Worked example (the kind the spec needs)

```
budget       = 1 000 000 bytes
reserved     =      2 500 bytes/second
observed     =        900 bytes in second 1
draw(sec 1)  = max(900, 2500) = 2500   -> remaining 997 500
```

with a companion example for the flapping case (reserved 0, burst 5000, observed 3000) so the
`max` and the burst allowance are both pinned by an example, not by prose.

## Open questions

1. Is `max(moved, reserved)` the right rule to standardise at all, or should the TIP only require that
   the rule be *advertised* so a buyer can model it?
2. Should rounding be REQUIRED to a specific direction, or merely REQUIRED to be documented and stable
   for the life of the budget?
3. Does an unused reservation accrue? (The rule above says no — a reservation purchases capacity, not
   credit.)
4. Should `burst` be in the advertisement (TIP-03) as well as the accounting rule?

## Compatibility

Inert for `payment_model: steps` TollGates, which grant a single `allotment` and do not debit
continuously.
