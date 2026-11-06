# ETH-01 - Frame-level rate control for link-scoped Ethernet

---
**Status: discussion draft. Not a ratified spec.** `ETH-01` is a placeholder
number, following the medium-spec naming used by WIFI-01. Nothing here is
normative yet.

**Depends on:** TIP-08 (endpoint advertisement), TIP-10 (wire-byte metering).
**Related:** TIP-11 (pause and renewal), WIFI-01 (Wi-Fi medium spec).

## Problem

FIPS over Ethernet carries FMP inside EtherType `0x2121`. Before an FMP
handshake completes, there is no proven npub to authorise on, so a seller that
controls the L2 segment can only see a MAC address.

A seller on a shared L2 segment (a Wi-Fi AP, a bridge, a switch port) has three
problems the link gate cannot solve, because they happen before or below FMP:

1. **Unknown devices.** A device that never completes a handshake can still
   emit `0x2121` frames and consume airtime or uplink.
2. **Flooding.** A known or unknown MAC can saturate the segment; the link gate
   only sees an established link's FMP traffic.
3. **Spoofing.** A MAC is not an identity, but it is the only selector visible
   before `msg1`.

Captive portals do not necessarily solve this: `ndsctl` installs IP-family
rules, while FIPS uses a non-IP EtherType. Whether a given portal drops
`0x2121` is implementation-specific and unverified here (our lab's `ndsctl`
half is blocked on a real AP). A protocol cannot assume the portal gates FIPS
frames.

At the same time, a frame gate is dangerous if it is mistaken for
authorisation. It is trivial to spoof a MAC, and dropping unknown MACs can lock
a paying customer out of the very path they need to reach checkout. The
protocol needs to say both what the gate is for and where it must stop.

## Proposal

### 1. Terms

- **Frame gate** - an L0/L2 mechanism that inspects Ethernet frames and applies
  a verdict (pass, drop, rate-limit) before FMP is parsed.
- **Gate MAC** - the source MAC observed on a frame, as seen by the seller's
  ingress hook.
- **Session MAC** - the source MAC observed during a successful FMP handshake
  on the Ethernet transport, bound to the session's npub for the session's
  lifetime.
- **Authorised set** - gate MACs the seller currently admits. Advisory only.

### 2. The gate is rate control, not authorisation

A frame gate MUST NOT be used as:

- proof of payment;
- proof of identity;
- the sole basis for admitting a peer to the FMP mesh;
- a reason to skip the FMP link gate.

A seller MAY use it for:

- dropping or rate-limiting `0x2121` frames from MACs with no session;
- bounding a single MAC's share of the segment;
- protecting the seller's uplink and airtime from a flood;
- enforcing a per-device fair-use ceiling alongside the per-session meter.

These are abuse controls. They do not create a session, credit a balance, or
change a session state.

### 3. First contact and the checkout path

A gate that default-drops unknown MACs MUST NOT be the only path to payment.
The seller MUST provide at least one of:

1. a checkout path outside the gated EtherType (captive portal, HTTPS, QR,
   out-of-band invoice); or
2. a low-rate admission lane for `0x2121` handshake traffic from unknown MACs,
   sufficient to complete `msg1` and the payment exchange.

A seller MUST NOT drop the frames of a session that is in `unpaid` or `grace`
at L0. Those states are defined at L1/L2: the link stays up and transit is
paused (TIP-11). Dropping at L0 would destroy the link and with it the evidence
needed to resume. L0 drop is for devices that are not peers, or for `evicted`
sessions after the durable eviction completes.

### 4. Per-MAC rate limiting

If a seller rate-limits, it SHOULD use an independent token bucket per gate MAC.

- The bucket MUST be per MAC, not global. A global bucket lets one sender
  consume the allowance of every other sender.
- The seller SHOULD express the bucket in **frames per second** when frame
  sizes are uniform, and in **bytes per second** when they are not. A
  bytes-based bucket calibrated for one frame size is not fair across sizes.
- The seller SHOULD allow a bounded burst; a bucket with no burst penalises
  legitimate on/off traffic.
- Bucket state MUST be bounded: a size cap and an idle timeout for per-MAC
  state. Unbounded state is a memory-exhaustion vector.
- The bucket MAY be stricter for MACs with no session than for MACs bound to a
  `paid` session.

Reference parameters, not normative: our lab used 1,000 pps and a 100-frame
burst for 64-byte frames.

### 5. Charging

Frames dropped or delayed by the frame gate are **not chargeable**. They
deliver no service and must not appear in the metered byte count (TIP-10).

If the gate is implemented outside the FMP transport, the seller MUST subtract
the gate's drop delta from the session's chargeable bytes for the same window,
exactly as it subtracts forwarding-policy drops for a paused peer. Double
counting a dropped frame is a billing defect.

The frame gate MUST NOT credit a session. Only a verified payment does that.

### 6. Session binding

On Ethernet, the seller SHOULD learn the session MAC from the `msg1` frame's L2
source and bind it to the session npub for the session lifetime.

- The binding MUST be refreshed from a new handshake, not from unsolicited
  frames claiming the same MAC.
- A gate MAC that later completes a handshake is the same device only if the
  seller observed it; the protocol does not require trusting prior frames.
- A seller that cannot observe the source MAC (UDP, TCP, Tor, BLE) MUST NOT
  pretend to have a frame gate for that transport. The gate is an Ethernet/L2
  facility, not a universal one.

### 7. State and observability

The seller SHOULD expose, per gate MAC:

- frames and bytes passed and dropped;
- current bucket state or an equivalent summary;
- the session npub bound to the MAC, if any.

Counters MUST NOT be attributed to an npub without a handshake-derived
binding; a spoofed MAC would otherwise let an attacker inflate or exhaust
another payer's accounting.

A set membership is **not** evidence of a violation. Implementations that use a
dynamic set (see the implementation note) create an entry on first contact,
including for senders that are under the rate. Only the drop counter says
otherwise.

### 8. Media and EtherType

This document applies to media where the seller can match the FIPS EtherType
before IP:

- Ethernet (`0x2121`), including bridged and 802.11s paths;
- equivalent L2 transports that expose an EtherType.

It does not apply to UDP, TCP, Tor or BLE, where the seller cannot key on a
pre-FMP frame. Those transports rely on the FMP link gate alone.

## Implementation note: nft is sufficient

The gate can be built with an `nftables` `netdev` ingress chain on the seller's
port, matching `ether type 0x2121` and using a dynamic set for per-MAC token
state:

```
set bucket { type ether_addr; flags dynamic; size 65535; timeout 10m; }
ether type 0x2121 update @bucket { ether saddr limit rate over 1000/second \
    burst 100 packets } counter drop
ether type 0x2121 counter accept
```

This needs only `NET_ADMIN` and one rule. The alternatives:

| Mechanism | What it does | Cost |
|---|---|---|
| 802.1X / EAP-TLS | per-device port authorisation | RADIUS, certificates, supplicant |
| Per-client VLAN | isolation | switch/AP config, one VLAN per client |
| 802.11s association | association only | does not gate frames |
| nft `netdev` + MAC set + per-MAC bucket | advisory rate control | kernel >= 4.10, `NET_ADMIN` |
| tc/BPF classifier | advisory rate control, richer actions | `CAP_BPF` + control plane |
| Captive portal (`ndsctl`) | IP-family traffic | unverified for `0x2121` |

`tc police` is a worse fit for many clients: it needs one filter per MAC and
accounts bytes excluding the MAC header, so a burst calibrated in pps shifts
with frame size. tc/BPF is the right choice only when the gate needs per-MAC
counters, marks or actions beyond drop.

For a seller who controls the AP, 802.1X or per-client VLAN is the honest
physical gate. For a seller who does not, the frame gate is the best available
and must be labelled as rate control.

## Backwards compatibility

- No FMP wire change. The gate operates below FMP.
- No new fields in session or payment messages.
- Sellers without an L2 gate are unaffected; the rules are opt-in.
- Buyers need no change. A buyer cannot tell whether a frame was dropped by a
  gate or lost on the medium; retransmission and the session deadline cover
  both.

## Security considerations

- **Spoofing.** An attacker can use another device's MAC and either steal its
  allowance or exhaust its bucket. Per-MAC limiting is fairness, not security.
- **Bucket DoS.** If the bucket is per MAC and MACs are spoofable, an attacker
  can force a victim's bucket to empty by sending as the victim. The gate MUST
  NOT use the bucket to deny the victim's *link*; it may only rate-limit.
- **State exhaustion.** Per-MAC state is attacker-controlled. Cap the number of
  entries and expire idle ones.
- **First-contact lockout.** A default-drop gate without an out-of-band
  checkout path is a self-DoS. See section 3.
- **Charging.** Never bill dropped frames; never credit from gate state.
- **Privacy.** A MAC is a persistent device identifier. Sellers SHOULD NOT log
  it longer than needed for the session and abuse window.

## Evidence

Lab: local Alpine container, three veth pairs and a bridge, nftables 1.0.9,
iproute2 6.9.0. Test frames only; no production traffic.

| Claim | Observation |
|---|---|
| `0x2121` can be matched and dropped per MAC | 11/11 cases pass on a `netdev` ingress hook |
| nft drop/accept is lossless at high rate | Receiver counter matches sender to 0.01 % at up to ~99k pps; userspace forwarding also lossless at 1k and 10k pps |
| The bucket is per sender, not global | Three concurrent senders (500, 5,000, 5,000 pps) against a 1,000 pps bucket: A received 1,500/1,500, B and C each received ~3,099/15,000 (n=3) |
| nft, tc-police and tc/BPF agree | 3,099 / 3,101 / 3,099 frames, n=3 |
| Set membership is not a verdict | The nft dynamic set holds an element for every sender seen, including under-rate A; only the drop counter distinguishes them |
| A MAC is spoofable, so this is not authentication | No proof of identity is derived from the gate |

The full lab logs and harness are public at
https://github.com/c03rad0r/low-layer-tollgate-exploration (test ecash only;
no upstream changes).

## Open questions

1. Does a real captive portal (`ndsctl` or equivalent) drop `0x2121` when it
   deauthorises a client? Our AP half is still open.
2. Should the session MAC be advertised by the seller as part of the session
   record, or learned locally? Local learning avoids a new wire field but is
   transport-specific.
3. Is a standard burst/rate profile useful, or should it remain a seller
   policy? The lab used 1,000 pps + 100 frames as a reference only.
4. Should `evicted` at L2 always imply an L0 drop, or only after the ACL write
   is durable? TIP-11 says eviction is deny, wait, disconnect; an L0 drop can
   follow the same ordering.
5. Should this be a standalone medium spec, or a section of a broader
   `ETH-01` that also covers neighbour discovery and addressing?

## Relationship to existing specs

- **WIFI-01** is the closest existing medium spec; this document follows its
  placement and naming.
- **TIP-08** defines how a customer finds an Ethernet endpoint.
- **TIP-10** defines the meter that must exclude gate-dropped frames.
- **TIP-11** defines `unpaid`, `grace` and `evicted`; this document says where
  L0 may and may not act on them.
