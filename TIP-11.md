# TIP-11 - Pause and renewal: the four-state session

---
**Status: discussion draft. Not a ratified TIP.** The number `TIP-11` is a
placeholder; the number and scope may change during review. Nothing here is
normative yet.

**Depends on:** TIP-09 (session scope), TIP-10 (wire-byte metering).
**Related:** TIP-01 (session event), HTTP-01 (payment lane). **Prior art:**
MONAD (Cashu Spilman channels over onion routing).

## Problem

A link gate that disconnects at exhaustion also kills the payment lane. That is
the chicken-and-egg this design starts from: you cannot pay over a link you are
not admitted to, and on Ethernet and BLE there is no IP fallback.

Today the reference link layer gives a gate only `admit` (an ACL entry) and
`disconnect`. There is no way to pause one peer's forwarding while keeping its
link. The only available workaround is a cut and re-admit, and it is expensive
and unreliable:

- a bare `disconnect` is undone by the client's `auto_connect` in 6.5-6.9 s,
  with a new link id and **no new payment**;
- only a durable eviction (deny, wait for the ACL, then disconnect) holds;
- re-admission then costs 2.7-2.9 s.

So "ran out of credit" currently means "lose the link and hope the client comes
back", and a client that comes back is not distinguished from one that never
paid.

## Prior art and credit

MONAD already implements this shape:

- sessions start **paused by default**;
- `CONNECT` is refused before the first payment;
- during later pauses, `CONNECT` is accepted but **forwarding is blocked until
  payment**;
- underpayment stays paused until the balance is positive;
- eviction is a separate, advisory event (`ChannelEvicted`, scoped `session` or
  `relay`), not the same thing as running out.

This TIP adopts the shape and the vocabulary, not MONAD's channel
implementation. Credit: MONAD.

## Proposal

### 1. States

| State | Link | Chargeable forwarding | Payment lane | Signal |
|---|---|---|---|---|
| `unpaid` | open | **blocked** | open | `21023`, `level=info`, `code=payment-required` |
| `paid` | open | allowed, metered | open | - (session event is the receipt) |
| `grace` | open | **blocked** | open | `21023`, `level=error`, `code=session-exhausted` |
| `evicted` | closed, npub denied | - | closed | ACL deny, then `Disconnect`, reason *resource exhaustion* |

"Chargeable forwarding blocked" means the seller does not forward the
customer's session-layer traffic. Control traffic needed to keep the link, the
MMP session and the payment lane alive MUST continue.

This is why `unpaid` blocks forwarding rather than refusing the link: on
Ethernet and BLE, the payment lane runs **over the link being sold**, so an
admission refusal removes the only way to pay.

### 2. Transitions

```
unpaid --accepted payment--> paid
paid   --allotment exhausted--> grace
grace  --renewal--> paid
grace  --grace deadline--> evicted
```

- `unpaid -> evicted` MAY happen after a stated admission timeout.
- `paid -> evicted` MUST NOT happen on exhaustion; it goes through `grace`.
- A renewal in `grace` returns to `paid` without a new link.
- `evicted` MUST be durable: deny the npub, wait until the ACL reports the deny
  effective, then disconnect. A bare `disconnect` is re-established by
  `auto_connect` in 6.5-6.9 s with a new link id and no new payment.
- A gateway MAY re-admit a previously evicted payer once a new payment is
  verified, but this is a new session, not a resurrection of the old one.

### 3. Notices

Reuse `kind:21023`. Add:

```json
["retry_after", "<seconds>"]
["top_up", "<steps>"]
["session_id", "<id>"]
```

- `retry_after`: when the client may try again.
- `top_up`: the smallest useful purchase, in steps.
- `session_id`: correlates the notice with the session event.

Rules:

- idempotent - a repeated notice MUST NOT create a second session;
- rate-limited to at most one per 10 seconds per session;
- MAY be sent before a session exists (the `unpaid` case);
- MUST NOT be the only record of state; the session event is authoritative.

`payment-due` at 80-90 % of allotment is the early edge of the same machine,
not a separate event kind. Implementations SHOULD send it with the same tags.

### 4. Expiry

Sessions carry `["issued_at"]` and `["valid_until"]`.

The link layer has no time synchronisation, so absolute time is advisory. The
seller enforces:

```
deadline = min(valid_until, received_at + max_session_duration + skew_tolerance)
```

and a **monotonic local timer** is the authority. `valid_until` is the
cross-check that lets a restarted seller reconstruct state.

Keyset `final_expiry` is **not** the session clock. NUT-02 says that after
`final_expiry` the mint may "irrevocably remove all of the nullifiers", so it is
a **settlement deadline**:

- admission MUST reject proofs whose keyset expires before
  `now + settlement_window`;
- `valid_until` MUST NOT exceed `final_expiry - settlement_window`.

### 5. Implementation requirement

`grace` requires blocking forwarding for one peer while keeping its link. The
reference link layer (FIPS 0.6.0-dev) has no such hook. This TIP therefore
depends on a **per-peer forwarding role** consulted on the data path: transit
from the peer is dropped while local delivery, the link and the Noise session
stay.

Until that exists, an implementation that claims this TIP MUST either:

- implement the role in its own forwarding layer, or
- declare itself non-conformant and fall back to **durable eviction** (deny,
  wait for the ACL, then disconnect).

It MUST NOT silently disconnect and call the state `grace`. It also MUST NOT
call a bare `disconnect` `evicted` when `auto_connect` can undo it. With the
forwarding role, `grace` can be implemented without a cut; without it, the
implementation is non-conformant and must say so.

The counter needed for billing while paused is in TIP-10: frames dropped by the
seller's policy are frozen or subtracted, never billed.

### 6. Reconciliation is a pause trigger

If the buyer's and the seller's byte counters disagree beyond a stated
tolerance, the seller SHOULD enter `grace` rather than continue billing.
Pausing is reversible; a disputed invoice is not.

## Backwards compatibility

- Sellers that do not implement this TIP continue to disconnect at exhaustion.
  Clients see the existing behaviour.
- `21023` notices are additive; unknown tags MUST be ignored.
- No change to the payment format.
- `unpaid` and `grace` are states, not new wire messages. A client that does
  not know about them simply sees no transit until it pays.

## Security considerations

- A paused peer still occupies a link slot and a Noise session. Sellers MUST
  bound the number of paused sessions and the grace duration, or `grace`
  becomes a free-rental denial of service.
- `unpaid` before any payment is the same exposure with no revenue at all.
  Sellers MUST bound the admission timeout and MAY refuse new handshakes above
  a configured number of unpaid sessions.
- A `retry_after` in the past or an absurd `top_up` is a client-side input; the
  client MUST validate.
- Pausing is not a settlement guarantee. The seller MUST NOT credit a renewal
  it has not verified to its own satisfaction.
- Eviction must be durable in both directions: deny, confirm the ACL took
  effect, then disconnect. Evicting on the strength of a queued ACL change
  leaves the peer connected.

## Evidence

Two-node UDP lab, FIPS `0.6.0-dev`, passive seller, `auto_connect` buyer, strict
ACL. Test ecash only; no mainnet funds.

| Path | Result |
|---|---|
| Bare `disconnect` | link back in **6.5-6.9 s**, new `link_id`, no payment |
| Deny -> wait -> disconnect | held for 3 s, no link on either side |
| ACL effective | 0.8-1.0 s (756-984 ms, tick-bound) |
| Re-admission | **2.7-2.9 s** total (n=4) |
| Pause at exhaustion | A->C blocked; link id preserved; no disconnect |
| Renewal | second token; resume restores A->C on the same link |
| Repeatability | 3/3 UDP and 3/3 TCP; link id preserved every run |
| Blocked transit billing | excluded via a per-peer policy-block counter |
| Ethernet pause | A->C blocked, A->B and link kept, 3/3 |
| Unpaid link, demo | **14,274 bytes forwarded for 0 msat** |
| Prepaid re-admit | **about 1.75 s** (677 ms ACL + 1,077 ms link) |

The full sequence was then run with real offline ecash on an A-B-C chain:
unpaid blocked, payment restored transit, exhaustion paused with the link id
preserved, and a second payment resumed. No disconnect was issued. A four-node
star repeated the block with three peers on the seller: blocking A dropped
A->D while C->D, A->B and the A-B link stayed up, and the policy-block counter
grew under A only (n=3). A single-client test could not distinguish a per-peer
block from a node-wide one; this one can.

The full lab logs and harness are available from the author on request; they
are not part of this repository.

## Open questions

1. Is `grace` a separate state or a flag on `paid`? This draft keeps it
   separate because the forwarding rule differs.
2. How long may `grace` last? This draft leaves it to the offer, but a default
   would help clients decide when to give up.
3. Should renewal extend the same session or mint a new one? This draft keeps
   the same session and updates the balance, so the link is preserved.
4. Should eviction be permanent for the npub, or only until the next verified
   payment? This draft says the latter, but a seller might prefer a cooldown.
5. Is a monotonic local deadline enough, or does the protocol need a
   challenge-response for time so a restarted seller cannot be tricked by a
   forged `valid_until`?

## Relationship to existing specs

- **TIP-01** defines the session event; this TIP adds `issued_at`,
  `valid_until` semantics and reuses `21023` for notices.
- **TIP-09** defines what a `link` session buys, which is what `grace` pauses.
- **TIP-10** defines the meter that triggers `grace` and the exclusion of
  policy-dropped frames.
- **HTTP-01** remains the payment lane on IP transports; this TIP does not
  change it and does not make it mandatory for link-scoped sales.
- **ETH-01** (proposed) is the pre-session frame gate; it is not an
  authorisation mechanism and does not replace this state machine.
