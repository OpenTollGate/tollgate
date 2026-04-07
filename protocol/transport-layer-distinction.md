# Transport Layer Distinction in TIPs

## Problem

TIPs currently mix three concerns:

1. **What** is being communicated (discovery, payment, session, notice)
2. **Over what physical medium** the data is sold and delivered (WiFi, Ethernet, Bluetooth, LoRa)
3. **Through what interface** the customer and TollGate negotiate (HTTP, Nostr relay, GATT, raw UDP)

These are three separate layers. A single physical medium can support multiple interfaces — an Ethernet link can carry HTTP requests, Nostr relay messages, or raw UDP packets. And a single interface type (like HTTP) could run over different physical media (WiFi, Ethernet). Conflating them leads to ambiguity in the specs.

## Three layers

### 1. Protocol — *what* is the data?

The abstract data model. Event kinds, tag structures, payment asset definitions, session semantics. Protocol TIPs never mention how messages are delivered — only what they contain.

Examples: Discovery event structure, Cashu token pricing tags, Session event fields, Notice event codes.

### 2. Medium — *what physical link* carries the sold data?

The physical or link-layer technology over which the TollGate sells connectivity. This is what the customer is actually paying for.

Examples: WiFi, Ethernet, Bluetooth, LoRa, mesh radio.

A medium may constrain which interfaces are available (Bluetooth doesn't natively speak HTTP), but the medium itself is not the interface.

### 3. Interface — *how* do customer and TollGate talk?

The communication method used for negotiation (discovery, payment, session management) between customer and TollGate. An interface runs over a medium, but the relationship is not 1:1.

Examples: HTTP server, Nostr relay, BLE GATT, raw UDP.

Important: the interface used for negotiation does not have to run over the same medium being sold. A WiFi tollgate could accept payments via:
- An HTTP server on the local network (in-band — same medium)
- A Nostr relay on the internet (out-of-band — different medium entirely)
- Both simultaneously

## How these layers compose

```
┌─────────────────────────────────────────────┐
│  Protocol                                   │
│  (TIP-01, TIP-02)                           │
│  Event kinds, tags, payment assets           │
├─────────────────────────────────────────────┤
│  Interface                                  │
│  (TIP-03, TIP-06, future BLE/UDP TIPs)      │
│  How messages are sent/received              │
├─────────────────────────────────────────────┤
│  Medium                                     │
│  (WiFi, Ethernet, BT, LoRa)                 │
│  What physical link carries the data         │
└─────────────────────────────────────────────┘
```

A TollGate implementation picks:
- One or more **media** it sells access to
- One or more **interfaces** for negotiation
- The **protocol** layer is always the same

### Examples

| Scenario | Medium (sold) | Interface (negotiation) | In-band? |
|----------|---------------|------------------------|----------|
| WiFi hotspot with captive portal | WiFi | HTTP server (TIP-03) | Yes |
| WiFi hotspot with relay | WiFi | Nostr relay (TIP-06) | No (relay is on the internet) |
| WiFi hotspot with both | WiFi | HTTP + Nostr relay | Mixed |
| BLE-tethered internet | Bluetooth | BLE GATT | Yes |
| LoRa mesh node | LoRa | Raw UDP or custom | Yes |
| Ethernet port at a café | Ethernet | HTTP server (TIP-03) | Yes |

## In-band vs out-of-band

This distinction falls out naturally from separating medium and interface:

- **In-band**: The negotiation interface runs over the same medium being sold. The customer can reach the TollGate without prior connectivity. Example: HTTP server on local WiFi.

- **Out-of-band**: The negotiation interface runs over a different channel. The customer needs existing connectivity to reach the interface. Example: Nostr relay on the internet for a WiFi tollgate.

Out-of-band interfaces have a bootstrapping problem — the customer needs some connectivity to pay for connectivity. This may be solved by the TollGate providing limited access for negotiation, or by the customer using a separate connection (mobile data, another network).

In-band interfaces have a device identification advantage — the TollGate can derive the customer's identity from the request itself (MAC from source IP, BLE address from connection).

## Terminology for TIPs

| Term | Meaning | TIP examples |
|------|---------|-------------|
| **Protocol** | Data model and semantics | TIP-01 (events), TIP-02 (Cashu) |
| **Interface** | Negotiation method between customer and TollGate | TIP-03 (HTTP), TIP-06 (Nostr relay) |
| **Medium** | Physical link over which data is sold | Not currently TIP-scoped |

The word "transport" is ambiguous — it could mean the physical medium or the interface. TIPs should avoid it in favor of the specific term.

## Mapping current TIPs

| TIP | Layer | Description |
|-----|-------|-------------|
| 01  | Protocol | Base events (Discovery, Session, Notice) |
| 02  | Protocol | Cashu payment asset |
| 03  | Interface | HTTP server |
| 04  | Interface | HTTP `/whoami` (extends TIP-03) |
| 05  | Protocol | Encrypted events (reserved) |
| 06  | Interface | Nostr relay |
| 10  | Interface | Beacon frame advertisement |

## What each Interface TIP must define

An Interface TIP is responsible for specifying:

1. **Payment delivery** — How does the customer send a bearer asset? (HTTP body, relay event, GATT write, etc.)
2. **Device identification** — How does the TollGate know which device is paying? (MAC from IP, BLE address, pubkey, explicit tag, etc.)
3. **Response delivery** — How does the TollGate return Session/Notice events? (HTTP response, relay event, GATT notification, etc.)
4. **Discovery** — How does the customer find the TollGate's pricing/capabilities? (HTTP GET, relay subscription, BLE service advertisement, etc.)

## Open questions

- Should TIPs explicitly label themselves `protocol` or `interface` in their header metadata?
- Should TIP-04 (`/whoami`) fold into TIP-03 as an HTTP interface detail?
- Does TIP-10 (beacon frames) belong to Interface or is it a discovery-only mechanism that crosses layers?
- Should there be Medium TIPs, or is the medium always implicit / out of scope?
- How to handle the bootstrapping problem for out-of-band interfaces?
