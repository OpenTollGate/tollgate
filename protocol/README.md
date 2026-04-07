# TollGate Protocol

## What is TollGate?

TollGate is a protocol for selling network access in exchange for small, frequent bearer asset payments — streaming sats for connectivity. Any device that can gate connectivity — a WiFi router, an Ethernet switch, a Bluetooth tether — can act as a TollGate. Customers pay with ecash tokens for exactly what they use: a few minutes online, a handful of megabytes, no more. No accounts, no subscriptions, no sign-ups, no KYC — just pay-as-you-go internet access on open networks.

The protocol is designed to work across any combination of physical media and negotiation interfaces. A TollGate advertises its pricing, accepts payments, and manages sessions using a common data model regardless of whether the customer connects over WiFi, Ethernet, Bluetooth, or something else entirely.

## Why TollGate?

**Permissionless**: No accounts, no persistent identites, no registration.

**Granular metering**: Sessions are measured in small increments (time or data). Customers pay for what they use, not a fixed subscription.

**Bearer asset payments**: The customer does not need to be online to pay. A bearer asset token (e.g. ecash) is sufficient on its own to purchase connectivity — no prior internet connection, no account, no interactive payment protocol required.

**Decentralized**: Each TollGate operates independently. Every TollGate sets its own pricing and accepted mints independently.

**Multi-hop**: TollGates can buy connectivity from each other, extending reach beyond a single operator.

## Protocol Architecture

The TollGate protocol is organized into three layers. A working TollGate combines specs from each layer — a protocol spec defines the interaction between customer and service provider, an interface spec defines how events are exchanged, and a medium spec may add capabilities specific to the physical link. Some specs depend on others (e.g. HTTP-02 extends HTTP-01), and the [Recipes](#recipes) section shows possible combinations.

```
┌─────────────────────────────────────────────┐
│  Protocol                                   │
│  Data model, events, payment assets         │
├─────────────────────────────────────────────┤
│  Interface                                  │
│  How messages are sent/received             │
├─────────────────────────────────────────────┤
│  Medium                                     │
│  What physical link carries the sold data   │
└─────────────────────────────────────────────┘
```

### Protocol — *what* is the data?

The abstract data model. Event kinds, tag structures, payment asset definitions, session semantics. Protocol specs never mention how messages are delivered — only what they contain.

| # | Description |
|---|-------------|
| [TIP-01](TIP-01.md) | Base Events (Advertisement, Session, Notice) |
| [TIP-02](TIP-02.md) | Cashu payments |

### Interface — *how* do customer and TollGate talk?

The communication method used for negotiation (advertisement, payment, session management) between customer and TollGate. An interface runs over a medium, but the relationship is not 1:1. A single medium can support multiple interfaces — an Ethernet link can carry HTTP requests, Nostr relay messages, or raw UDP packets. And a single interface (like HTTP) can run over different media.

| # | Description |
|---|-------------|
| [HTTP-01](HTTP-01.md) | HTTP server |
| [HTTP-02](HTTP-02.md) | Restrictive OS compatibility |
| [HTTP-03](HTTP-03.md) | Usage endpoint |
| [NOSTR-01](NOSTR-01.md) | Nostr relay |

### Medium — *what physical link* carries the sold data?

The physical or link-layer technology over which the TollGate sells connectivity. A medium may constrain which interfaces are available, but the medium itself is not the interface.

| # | Description |
|---|-------------|
| [WIFI-01](WIFI-01.md) | Discovery through Beacon Frames |

---


# Recipes

Example combinations of Protocol + Interface + Medium specs.

| Name | Protocol | Interface | Medium | Description |
|------|----------|-----------|--------|-------------|
| WiFi hotspot (full) | TIP-01, TIP-02 | HTTP-01, HTTP-02 | WIFI-01 | Adds `/whoami` for restrictive OS support and beacon frame discovery. |
| WiFi hotspot (basic) | TIP-01, TIP-02 | HTTP-01 | — | Captive portal with HTTP payment. Device ID from MAC. |
| WiFi hotspot (stealth) | TIP-01, TIP-02 | HTTP-01 | — | No Captive portal, No beacon frame advertisement, no `/whoami`. Only customers who know the network can pay. |
| Ethernet port | TIP-01, TIP-02 | HTTP-01 | — | Wired access sold via HTTP. Same as WiFi hotspot but over Ethernet. |
| WiFi + Nostr relay | TIP-01, TIP-02 | HTTP-01, NOSTR-01 | WIFI-01 | HTTP for in-band payments, Nostr relay for out-of-band or remote management. |
| Nostr-only | TIP-01, TIP-02 | NOSTR-01 | — | Fully out-of-band. Customer pays via relay, device identity via pubkey/tags. |
| BLE tethering | TIP-01, TIP-02 | *(future BLE-01)* | — | Bluetooth internet sharing. Payment over GATT characteristics. |


## (Breaking) changes

| When | What |
|------|------|
| June 2025 | Cashu payments mint now is added to `<price_per_step>` tag, removing `<mint>` tag |
| June 2025 | Changed Discovery to no longer be ephemeral |
| March 2026 | Payment accepts pure bearer asset tokens instead of `kind=21000` Nostr event wrapper. Specs restructured into Protocol (TIP), Interface (HTTP, NOSTR), and Medium (WIFI) categories. |

## License

All TollGate protocol specs are public domain.
