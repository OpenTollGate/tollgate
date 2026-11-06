# TIP-10 - Wire-byte metering for link sessions

---
**Status: discussion draft. Not a ratified TIP.** The number `TIP-10` is a
placeholder; the number and scope may change during review. Nothing here is
normative yet.

**Depends on:** TIP-09 (session scope). **Related:** TIP-01 (metric and
step_size), TIP-11 (pause and renewal).

## Problem

TIP-09 adds `scope=link`: a sale of transit on one hop. That sale needs a unit,
and without a normative unit the buyer and the seller will each believe a
different number is correct:

- payload vs frame;
- including or excluding medium encapsulation;
- counting control traffic or not;
- rounding each frame up to a sat;
- charging retransmissions once or twice.

Each disagreement is a dispute the protocol cannot resolve after the fact,
because the bytes are gone. The boundary has to be stated before the first
session, not negotiated per sale.

Wire bytes are also the seller's actual cost: air time, battery, uplink bytes,
header overhead and AEAD overhead. MONAD (Cashu Spilman channels over onion
routing) reached the same conclusion independently and meters
`in_bytes_per_millisat` / `out_bytes_per_millisat`.

## Proposal

### 1. The unit

For a `scope=link` session with `metric=bytes`, the metered quantity is:

```
chargeable_bytes = sum over chargeable frames of FMP_frame_length
```

where, for an established FMP frame:

```
FMP_frame_length = 4  (common prefix)
                 + 12 (remaining header: receiver index, counter, type/timestamp)
                 + payload_length
                 + 16 (AEAD tag)
```

and for a handshake frame it is the handshake frame length (MSG1 114 B,
MSG2 69 B at the current FIPS revision).

The count is taken **at the transport boundary**: the frame handed to, or
returned from, the socket. It **excludes** medium encapsulation:

| Medium | Excluded from the count | Counted |
|---|---|---|
| UDP/IPv6 | 8 + 40 = 48 B | FMP frame |
| TCP | 20 + 20 = 40 B (plus stream framing) | FMP frame |
| Ethernet | 14 B MAC header (+ FCS) | FMP frame (see caveat below) |
| BLE | L2CAP header and radio overhead | FMP frame |

A seller that wants to recover encapsulation cost MUST advertise it
explicitly, for example `["overhead", "udp", "48"]`. Default: not charged.

**Ethernet caveat.** The FIPS Ethernet transport prefixes each frame with a
3-byte framing header (frame type plus a 2-byte LE payload length) and removes
it again on send, so the current receive counter is 3 bytes per frame higher
than the send counter for the same frame. An implementation MUST normalise to
the FMP frame and MUST NOT present a per-direction total that silently mixes
the two conventions.

### 2. Chargeable frames

**Chargeable:**

- `SessionDatagram` and other session-layer payload frames attributable to the
  customer;
- handshake frames of the session being sold (they are the admission cost).

**Not chargeable:**

- `Heartbeat`
- MMP `SenderReport` / `ReceiverReport`
- `TreeAnnounce`
- `FilterAnnounce`
- `LookupRequest` / `LookupResponse`
- `Disconnect`
- rekey frames
- frames dropped by the seller's own forwarding policy (a paused peer). They
  consume link bytes but deliver no transit; the gate MUST freeze or subtract
  them and never bill them as revenue.

The last rule was measured: a node-wide `drop_policy_blocked_{packets,bytes}`
counter plus per-peer `policy_blocked_by_peer` attribution let the reference
meter subtract the window delta, so paused transit billed at zero while the
link stayed up.

Rationale: a customer does not pay for the seller's routing chatter, and does
not pay for the control traffic that keeps the seller's own mesh alive.

### 3. Direction

An offer MAY carry:

```json
["direction", "both" | "in" | "out"]
```

- `in`: bytes received by the seller from the customer.
- `out`: bytes sent by the seller to the customer.
- `both` (default for `scope=link`): the sum.

The session MUST mirror the direction. A seller that bills `both` MUST count
both directions at the same boundary.

Ethernet and BLE air time is roughly symmetric; a WAN uplink is not. Making the
direction explicit avoids the "you charged me for my own upload" dispute.

### 4. Rate and rounding

The price is expressed as **bytes per millisat**:

```json
["rate", "<bytes_per_millisat>"]
```

- The rate MUST be a positive rational or decimal string. It MUST NOT be
  constrained to an integer number of sats per byte: sub-sat rates exist, and
  integer step-rounding biases whichever side the rounding favours.
- `step_size` remains a **billing** granularity (how often a balance is
  decremented), never a **measurement** granularity. Frames are counted
  individually; steps are settled in aggregate.
- The **measurement step** bounds the overshoot at exhaustion: one frame if
  metering per frame, one polling interval if counters are batched. An
  implementation MUST state the step in the offer. A batched 40-packet step in
  a 50 sat demo overshot by 2.18x; a per-packet step reduced the overshoot to
  one packet.
- Implementations SHOULD carry the remainder across steps rather than rounding
  each frame or each step.

### 5. Retransmission and duplication

A frame that crossed the wire twice was paid for twice. Retransmissions,
duplicates and rekey-adjacent copies are charged per crossing. Implementations
MUST NOT deduplicate by content.

A frame that crossed during a drain window still crossed and is chargeable.

### 6. Reconciliation

Both ends see the same frames on the link. A seller SHOULD expose its counter
to the buyer, and a buyer SHOULD compare. Disagreement beyond a stated tolerance
SHOULD pause the session rather than silently continue. The protocol does not
require a third-party auditor.

### 7. Relationship to the reference implementation's counters

FIPS already counts at this boundary:

- UDP receive `stats.record_recv(len)` at `src/transport/udp/mod.rs:644,707`;
- TCP receive `stats.record_recv(data.len())` at `src/transport/tcp/mod.rs:1532`
  (after the stream framing is stripped, so the FMP frame);
- per-link send `peer.link_stats_mut().record_sent(bytes_sent)` at
  `src/node/mod.rs:4592,4664`;
- Ethernet receive `stats.record_recv(len)` at
  `src/transport/ethernet/mod.rs:1275`, before the 3-byte framing header is
  stripped (see the caveat in section 1).

So an implementation MAY use the daemon's counters. It MUST NOT re-count at a
different boundary and call the result the same thing.

**Known limitation.** Current counters are aggregate per link, not per frame
class. Splitting chargeable from non-chargeable frames requires either a
per-class counter (not present in the current FIPS tree) or modelling the
control-frame rate. Until a per-class counter exists, a `link` session SHOULD
price an all-frames rate and say so in the offer rather than claim exact
wire-byte accounting.

## Backwards compatibility

This TIP adds fields. Absent `direction`, `both` applies. Absent `rate`, the
offer's existing price applies, but it is then not a wire-byte price and the
seller MUST NOT claim wire-byte metering.

## Security considerations

- A buyer can under-report its own counter. The seller's counter is the one
  that bills. Reconciliation is a courtesy, not a consensus.
- A seller can over-report. The buyer's remedy is to leave; the protocol has no
  fraud proof. An exposure cap (TIP-11) bounds a single session's loss.
- Metering must not be used to distinguish traffic content. Count frames, not
  payloads.
- Counting handshake frames gives an attacker a cheap way to run up a bill
  against a payer that has authorised a session. Sellers SHOULD treat
  handshake volume as rate-limited (see `ETH-01`) and SHOULD NOT let an
  unauthenticated handshake consume a paid balance.

## Evidence

Measured in a FIPS 0.6.0-dev lab (local Docker, test ecash only; no mainnet
funds, no upstream changes).

| Claim | Observation |
|---|---|
| The daemon counter is the FMP frame, not the payload | Session payloads of a known size produced a per-frame count equal to payload + 32 B of FMP header and tag |
| Encapsulation is not counted | The same payload over UDP and TCP produced different on-wire sizes but the same FMP frame count |
| Policy-dropped frames can be subtracted | A paused peer's frames appeared in a policy-block counter and were billed at zero |
| The measurement step sets the overshoot | A 40-packet batch overshot a 50 sat balance by 2.18x; per-frame metering held the overshoot to one frame |
| Counters are aggregate | The reference node could not separate control frames from data frames; the offer therefore stated an all-frames rate |

The full lab logs and harness are public at
https://github.com/c03rad0r/low-layer-tollgate-exploration.

## Open questions

1. Should handshake frames be chargeable? This draft says yes for the session
   being sold, because they cost the seller real bytes, but it is a small
   amount and the alternative (never charge them) is simpler.
2. Is `bytes per millisat` the right direction of the ratio, or should it be
   `msat per byte`? MONAD uses the former; this draft follows it.
3. Should the meter count at the socket or at the AEAD boundary? The socket is
   observable by both sides; the AEAD boundary is not.
4. How should a per-frame-class counter be specified so that existing
   aggregate counters can still be used as a fallback?
5. Is a stated tolerance plus pause enough for reconciliation, or does a link
   sale need a receipt the buyer can keep?

## Relationship to existing specs

- **TIP-01** defines `metric` and `step_size`. This TIP defines what `bytes`
  means for `scope=link` and separates measurement granularity from billing
  granularity.
- **TIP-09** defines `scope=link`, which is the only scope this TIP applies to.
- **TIP-11** defines how a session pauses when the balance reaches zero, which
  is where the measurement step becomes visible.
- **ETH-01** (proposed) covers per-source frame rate control before a session
  exists, which is a different mechanism from metering.
