# TIP-09 - Session scope: `link` and `e2e`

---
**Status: discussion draft. Not a ratified TIP.** The number `TIP-09` is a
placeholder; the number and scope may change during review. Nothing here is
normative yet.

**Depends on:** TIP-08 (endpoint advertisement). **Related:** TIP-01
(advertisement and session), TIP-10 (wire-byte metering), TIP-11 (pause and
renewal).

## Problem

A TollGate can sell two different things without saying which one it is
selling:

- **one hop** - transit on the link between the customer and the TollGate, at
  the link's byte cost; or
- **the path** - access to a service somewhere beyond the TollGate, priced
  end to end.

Today the protocol is implicitly end to end: a payment buys access to a
service, and the hops in between are not priced. That is fine for a hotspot,
but it cannot describe a link sale. Without an explicit scope:

1. a customer cannot tell whether it is paying for one hop or for a path;
2. two resellers can charge "link" for the same bytes on two hops and each
   claim to be the link seller;
3. an implementation cannot say which counter it is supposed to read, or where
   metering stops.

"Per transport" is a **policy scope**, not a place to put code. A transport
moves bytes between addresses; the npub appears at the link layer. The protocol
should name the scope and leave placement to the implementation.

## Proposal

### 1. Optional tags on the advertisement and the session

An advertisement (`kind:10021`) MAY carry:

```json
["scope", "link" | "e2e"]
["transport", "<transport>"]
```

A Session event (`kind:1022`) created from that advertisement MUST mirror both
fields. A session whose `scope` or `transport` does not match its advertisement
is invalid.

Defaults:

- absent `scope` means `e2e`, which is today's behaviour;
- absent `transport` means "any transport the TollGate advertises", which is
  also today's behaviour.

### 2. `scope=link`

- Covers exactly one link: one `(TollGate npub, transport, remote address)`
  tuple and one authenticated session on that link.
- The metering boundary is the link frame on that link, as defined in TIP-10.
- The TollGate MUST be the peer at the far end of that link. A node that only
  forwards for the link is not a `link` seller; its role is `e2e` or resale.
- The session ends when the link ends, or at `valid_until`, whichever comes
  first.
- A `link` session does not promise reachability beyond the TollGate. It
  promises transit on the sold hop.

### 3. `scope=e2e`

- Covers the path from the customer to the service named by the offer.
- Metering, if any, is at the service boundary, not per hop.
- Intermediate forwarders MUST NOT open a second `link` charge for the same
  bytes.
- An `e2e` offer MUST NOT require a `link` session between the same two peers.

### 4. The no-double-charge rule

For a given payer, a given byte, and a given scope, the byte MUST be charged at
most once.

The distinction is per **(payer, hop, scope)**, not per literal tag value:

- A `link` session on hop A and a `link` session on hop B are different hops,
  even though both say `link`. Charging both is correct.
- A `link` session and an `e2e` session covering the same bytes are a double
  charge. Implementations MUST NOT create both for the same flow.
- One TollGate charging twice for the same hop is a violation, whatever the
  labels say.

### 5. Resale

A node MAY be a customer on one link and a TollGate on another. Additional
requirements:

- The downstream session's `transport` is the downstream link's transport, not
  the upstream one.
- Downstream unsettled credit SHOULD be bounded by upstream prepaid remaining,
  less a reserve. This is policy, not a wire field, but a reseller MUST
  document its bound.
- `TollGate npub == customer npub` MUST be rejected. A node cannot sell to
  itself.
- A reseller MUST NOT advertise an `e2e` path it cannot actually reach.

### 6. Metering interaction

- `scope=link` uses the wire-byte rule in TIP-10.
- `scope=e2e` is out of scope for TIP-10 except for one rule: it MUST NOT be
  metered with the link rule. An implementation that cannot meter at the
  service boundary MUST say so and use a time or flat price instead.
- The `metric` tag in TIP-01 still applies. `scope` says **what** is sold;
  `metric` says **how it is counted**.

## Backwards compatibility

- Absent `scope` means `e2e`, so existing advertisements and sessions remain
  valid.
- Absent `transport` means "any", so existing multi-transport TollGates remain
  valid.
- No new required field and no wire-format change. A customer that does not
  understand either tag ignores it and gets today's behaviour.

## Security considerations

- **Scope is a claim by the TollGate.** A customer cannot verify that a `link`
  TollGate is really the far end of the link without checking the link
  identity, where the transport exposes it. Customers SHOULD do so.
- **Mislabeling.** A reseller can label an `e2e` flow as `link` to charge
  twice. The rule makes that a protocol violation, but detection is social
  unless the customer can see the path. The exposure cap bounds the damage.
- **Resale chains.** Each hop can add latency and failure. A customer buying
  `e2e` should know that it is buying a path, not a specific hop.
- **Scope inflation.** A TollGate could advertise `link` while forwarding
  nothing. The customer's remedy is to leave; metering and pause (TIP-11) give
  it a signal.

## Evidence

Measured in a FIPS 0.6.0-dev lab (local Docker, test ecash, no mainnet funds).
FIPS is the reference link transport.

| Claim | Observation |
|---|---|
| A link-scoped session can be paid and paused without cutting the link | A two-node UDP lab ran unpaid -> paid -> exhausted -> paused -> renewed; the link and session survived |
| The failure this scope prevents is real | With the link up and the session `unpaid`, 14,274 bytes were forwarded for 0 msat |
| A link sale is not an e2e sale | In a three-node chain, blocking A's transit at B stopped A->C while A->B and the A-B link stayed up; a second client C was unaffected in a four-node star (n=3) |
| The same identity can sell different transports | One npub carried UDP on `:2121` and TCP on `:443`; scope and transport were independent of the port |
| Multiplexing is not double charging | Two clients behind one TollGate were metered and paused independently |

The full lab logs and harness are public at
https://github.com/c03rad0r/low-layer-tollgate-exploration.

## Open questions

1. Is `scope` a property of the advertisement, the session, or both? This draft
   mirrors it on both, as with `metric`.
2. Should `transport` be required whenever `scope=link`? Without it, a
   multi-transport TollGate cannot say which link the price applies to.
3. How can a customer verify "the TollGate is the far end of this link"? The
   link handshake proves the npub; proving that the npub is the last hop
   depends on the transport.
4. Is the resale bound a protocol number or an operator policy? This draft
   leaves it as policy.
5. Does an `e2e` session need an explicit service identifier, or is the offer's
   existing content enough?

## Relationship to existing specs

- **TIP-01** defines the advertisement and session events. This TIP adds two
  optional tags to each.
- **TIP-08** defines how a customer finds the endpoint for the chosen
  transport.
- **TIP-10** defines the byte boundary for `scope=link`.
- **TIP-11** defines what happens when a link-scoped session runs out.
- **ETH-01** (proposed) covers Ethernet frame-level rate control, which is a
  medium facility and not part of this scope.
