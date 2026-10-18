# TIP-07 - Metering attestation (OPTIONAL / DISCUSSION DRAFT)

**Status: proposal for discussion. Not a ratified TIP.** The number `TIP-07` is a placeholder.

## Problem

In a voucher design the TollGate **sells the unit and counts the units it delivered**. A buyer has no
independent view of its own consumption.

Protocol issue #12 raised exactly this in July 2026 — *"how do clients verify the mint isn't inflating
tokens?"* — and it remains unanswered. The honest statement today is that a buyer trusts the operator
completely.

## Proposal — and what it does NOT claim

A TollGate MAY periodically publish a signed **metering attestation** for a session or channel:

| Field | Meaning |
|---|---|
| `channel` / `session` | which funding object the counter belongs to |
| `seq` | monotonic counter, so a gap or reorder is detectable |
| `used` | units consumed so far, in the sold metric |
| `observed_at` | timestamp of the measurement |

Carried as tags on the existing `1022` session event, or a new kind — **that choice is an open question.**

### What this buys

It converts *"trust the operator entirely"* into *"trust, with a tamper-evident trail"*. A buyer who
records attestations can detect a later contradiction (a `used` that moves backwards, a `seq` gap, a
final charge inconsistent with the last attestation), and can raise a dispute with evidence.

### What this does NOT buy

- **Not proof of non-inflation.** A dishonest operator can simply never publish, or publish a consistent
  fiction.
- **Not trustless.** No independent party measures the bytes.
- **Not a dispute-resolution mechanism.** It makes a dispute *evidenced*, not *settled*.

Saying this explicitly is the point of the TIP. An optional attestation that is marketed as a trust
solution is worse than no attestation, because it launders trust.

## Open questions

1. Is this worth specifying at all before somebody actually needs it?
2. Tags on `1022`, or a separate event kind?
3. Should `seq` be per-session or global per-TollGate?
4. Should publication be **advertised** (`10021`) so a buyer can choose an attesting operator?
5. Does this belong in the TIPs, or is it an implementation feature until issue #12 is resolved?

## Compatibility

Fully optional. A TollGate that publishes nothing is conformant; a client that ignores attestations
behaves as today. Requires no change to any existing tag.
