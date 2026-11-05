# TIP-08 - Endpoint advertisement and the port as a hint

---
**Status: discussion draft. Not a ratified TIP.** The number `TIP-08` is a
placeholder; the number and scope may change during review. Nothing here is
normative yet.

**Depends on:** none. **Related:** TIP-01 (advertisement), HTTP-01 (default
port), NOSTR-01 (default port), TIP-09 (session scope).

## Problem

A TollGate is identified by its **npub**, not by a `host:port`. Today the
protocol text only describes a port in the interface specs (`2121` for HTTP,
`4242` for Nostr), where it is a default and not an identity. That is the right
shape, but it does not generalise:

1. **Not every transport has a port.** Ethernet carries frames under an
   EtherType, BLE uses L2CAP, Tor and Nym use their own addresses. A link-scoped
   offer cannot name a port for any of them.
2. **One npub, several addresses.** The same TollGate can be reachable over TCP
   and UDP at once, on different ports, and over Ethernet on the same device.
   The advertisement has no way to say which address belongs to which
   transport.
3. **NAT and roaming.** A port written into an offer is a promise the TollGate
   cannot keep behind CGNAT. The identity survives a rebind; the address does
   not.
4. **Link-scoped sales need a dial target.** TIP-09 introduces `scope=link`,
   which buys one hop. The customer must know which transport and address to
   use for that hop, without guessing a default.

This TIP adds an explicit endpoint to the advertisement and states plainly that
a port is a dialling hint, never part of the TollGate's identity.

## Proposal

### 1. The port is an interface default, not protocol identity

- No protocol text should require a TollGate to listen on, or a customer to
  dial, port `2121`.
- The defaults in HTTP-01 (`2121`) and NOSTR-01 (`4242`) remain **interface
  defaults**. An interface spec may keep a default; a TollGate may use any
  port its interface supports.
- A port MUST NOT be used to identify a TollGate or to validate an offer. The
  npub is the identity.

This does not require changing HTTP-01 or NOSTR-01. It records the rule they
already follow and removes the ambiguity for link-scoped offers.

### 2. An optional `endpoint` tag on the advertisement

A `kind:10021` advertisement MAY carry zero or more endpoint tags:

```json
["endpoint", "<transport>", "<address>"]
```

- `<transport>` is a transport identifier, for example `tcp`, `udp`,
  `ethernet`, `ble`, `tor`, `nym`, `http`, `nostr`, or a future identifier.
  It MUST be the identifier the customer will use to dial, not the interface
  name when the interface is carried over several transports.
- `<address>` is opaque to the protocol and is defined by the transport:

  | Transport | Address form (examples) |
  |---|---|
  | `tcp`, `udp`, `http`, `nostr` | `host:port`; IPv6 in brackets, `[2001:db8::1]:2121` |
  | `ethernet` | a MAC address or an interface-scoped peer identifier |
  | `ble` | a BLE address or service UUID, as the transport defines |
  | `tor`, `nym` | the onion or mixnet address |

- An endpoint with an unknown `<transport>` MUST be ignored, not treated as an
  error. Unknown future transports are expected.
- An endpoint with an empty or malformed address MUST be ignored. A customer
  MUST NOT invent a missing port or host.

Example:

```json
{
    "kind": 10021,
    "pubkey": "<tollgate npub>",
    "tags": [
        ["metric", "bytes"],
        ["step_size", "1000000"],
        ["endpoint", "http", "192.168.1.1:2121"],
        ["endpoint", "udp", "[2001:db8::1]:2121"],
        ["endpoint", "ethernet", "02:00:00:00:00:0b"]
    ]
}
```

### 3. Consistency with `transport` and `scope`

TIP-09 adds optional `["transport", "<transport>"]` and
`["scope", "link" | "e2e"]` tags.

- If an advertisement carries a `transport` tag, every `endpoint` with the same
  transport MUST be usable for that offer.
- An advertisement with `scope=link` and no endpoint for the customer's
  transport is **not purchasable over that transport**. The customer MUST NOT
  guess a default port or address.
- If several endpoints are advertised for one transport, the TollGate SHOULD
  list them in preference order. A customer SHOULD try them in order and MAY
  fall back to the next on failure.
- An endpoint does not have to be reachable from every customer (NAT, link-local
  addressing). Failure to connect is not a refusal of payment and does not
  invalidate the offer.

### 4. Legacy forms

- A bare `host:port` in an interface spec (for example HTTP-01's
  `192.168.1.1:2121`) is an `endpoint` with the matching interface transport.
- If a future advertisement carries a legacy `["port", "<n>"]` tag, it MUST be
  interpreted as `["endpoint", "tcp", "<host>:<n>"]` **only** when the
  advertisement also names a host. Otherwise it is ignored. New
  advertisements SHOULD NOT use a bare port tag.

## Backwards compatibility

- Existing advertisements without `endpoint` keep working. The customer uses
  the interface default (HTTP-01's `2121`, NOSTR-01's `4242`, or the address it
  already knows).
- Existing customers that hard-code a default still work against TollGates
  that use it. They do not work against other ports, which is already true.
- No change to the Session event, the payment format, or any wire protocol.
  This is an advertisement change only.

## Security considerations

- **An endpoint is a hint, not authentication.** Reaching an address proves
  nothing about the TollGate's npub. Authorisation remains the link handshake
  keyed on the npub.
- **Stale and translated addresses.** NAT, roaming and DHCP make endpoints
  perishable. A customer MUST NOT treat a failed connection as a payment
  refusal, and a TollGate SHOULD advertise only endpoints it can serve.
- **Amplification.** Advertising an endpoint the TollGate cannot serve is a
  denial-of-service vector against its own customers. Implementations SHOULD
  validate endpoints before publishing.
- **Enumeration.** Advertising every transport reveals more about the device.
  A TollGate MAY advertise a subset; absence of an endpoint is not a promise
  that the transport is unsupported.

## Evidence

Measured in a FIPS 0.6.0-dev lab (local Docker, test ecash only; no upstream
changes). FIPS is used as the reference link transport, not as a requirement of
this TIP.

| Claim | Observation |
|---|---|
| The same identity can be reached over different transports | One npub carried a UDP session on `:2121` and a TCP session on `:443`; the identity did not change |
| Ethernet has no port | Two nodes met over EtherType `0x2121` and exchanged FMP frames; no TCP/UDP port was involved |
| A fixed port is not needed for admission | Link admission used the npub and the transport address; the sidecar synthesised the dial target from `show transports` |
| One node can expose several addresses | The lab node advertised both UDP and TCP addresses while keeping one npub |

The full lab logs and harness are available from the author on request; they
are not part of this repository.

## Open questions

1. Should `endpoint` be one tag with a transport/address pair, or a nested
   object? The tag form matches TIP-01's existing flat tags.
2. Is preference order useful, or should the customer choose by measured
   latency? A TollGate cannot know the customer's path.
3. Should endpoints be signed separately from the advertisement, so a relay can
   not rewrite them? Today the advertisement is a Nostr event and inherits its
   signature; the address is inside the signed event.
4. Does an interface default belong in the protocol index or only in the
   interface spec? This draft keeps it in the interface spec.
5. How should a link-local address (Ethernet, BLE) be represented so it is
   unambiguous across interfaces?

## Relationship to existing specs

- **TIP-01** defines the `kind:10021` advertisement and its tags. This TIP adds
  an optional tag and does not change existing ones.
- **TIP-09** adds `scope` and `transport`, which constrain which endpoints are
  usable.
- **HTTP-01**, **HTTP-02**, **HTTP-03**, **NOSTR-01** keep their default ports.
  This TIP only says those defaults are not identity.
- **WIFI-01** is a medium spec; an Ethernet medium spec (`ETH-01`) is proposed
  separately for frame-level rate control.
