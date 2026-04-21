<p align="center">
  <img src="images/TollGate_Logo-C-black.png" alt="TollGate" width="480">
</p>

# TollGate Protocol

## Index

- [What is TollGate?](#what-is-tollgate)
- [Why TollGate?](#why-tollgate)
- [Protocol Architecture](#protocol-architecture)
- [Recipes](#recipes)
- [Implementations](#implementations)
- [(Breaking) changes](#breaking-changes)
- [Project Timeline](#project-timeline)
- [Additional reading](#additional-reading)
- [Events](#events)
- [License](#license)

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


## Implementations

- [tollgate-module-basic-go](https://github.com/OpenTollGate/tollgate-module-basic-go) — reference implementation for OpenWRT routers.
- [tollgate-os](https://github.com/OpenTollGate/tollgate-os) — ready-made OpenWRT image with TollGate pre-installed.

## (Breaking) changes

| When | What |
|------|------|
| June 2025 | Cashu payments mint now is added to `<price_per_step>` tag, removing `<mint>` tag |
| June 2025 | Changed Discovery to no longer be ephemeral |
| March 2026 | Payment accepts pure bearer asset tokens instead of `kind=21000` Nostr event wrapper. Specs restructured into Protocol (TIP), Interface (HTTP, NOSTR), and Medium (WIFI) categories. |

## Project Timeline

| When | Milestone |
|------|-----------|
| 2012 | Initial idea of paying neighbouring routers for higher bandwidth via multi-WAN |
| Mid 2024 | Early proof of concept by pasting ecash in captive portal password field |
| Late 2024 | Introduced Go / Nostr to the routers |
| April 2025 | First protocol draft |
| March 2026 | Protocol restructured into Protocol (TIP) / Interface (HTTP, NOSTR) / Medium (WIFI) layers |

## Additional reading

| Title | Last edited |
|-------|-------------|
| [TollGate android client works!](https://njump.me/nevent1qqsv7s9h4y0aklk5rfy28d3d6wgua0pafuaesjuewt6tfrq3lnuxh9cpzemhxue69uhk7unpdenk2umede3juar9vd5z7q3qzzt0d0s2f4lsanpd7nkjep5r79p7ljq7aw37eek64hf0ef6v0mxqxpqqqqqqz64slvx) | January 31, 2025 |
| [Building TollGate Image](https://primal.net/e/nevent1qqsrkfkdwy8q693vtrs789t7gf0vt86avkp7ufcmn5j0rhuvpr35zsqpp4mhxue69uhkummn9ekx7mqzyze7y04hydanvkk2n8ya9rc8pgc56cdgxlnm6r7tkrcrjnwuld3psge2xzv) | January 28, 2025 |

## Events

Feel free to add events via pull request.


### Upcoming (2026)

| Confirmed | Event | Dates | Where | Details |
|-----------|-------|-------|-------|---------|
| ✅ | [Wireless Community Weekend](https://forum.freifunk.net/t/wireless-community-weekend-2026/24552) | 15 may – 17 may | Germany, Berlin | c-base, Rungestr. 20 |
| ✅ | [btc++ Nairobi](https://btcplusplus.dev/) | 17 jun – 20 jun | Kenya, Nairobi | Pride Inn Azure |
| ✅ | [Adopting Bitcoin Nairobi](https://x.com/AdoptingBTC/status/2017621046598226329) | 23 jun – 24 jun | Kenya, Nairobi | Near Afribit Kibera |
| ✅ | [DWeb Berlin](https://dwebcamp.org/) | 8 jul – 12 jul | Germany, Berlin | Kluckstraße 25 |
| ✅ | [Battlemesh v18](https://battlemesh.org/BattleMeshV18/) | 7 sep – 13 sep | Croatia, Lika region | LikaNet project area |
| ✅ | [btc++ Berlin](https://btcplusplus.dev/) | 1 oct – 3 oct | Germany, Berlin | w3 hub |
| ✅ | [Dark Prague](https://darkprague.com/) | 2 oct – 4 oct | Czechia, Prague | Stará čistírna odpadních vod 1906, Bubeneč |
| ✅ | [Nostr Hackday](https://primal.net/e/nevent1qqsgq3dyyea83am64u736lw0mm3fkm8jhctlqkr2au9rvf8sw5cn5hq3nkv4p) | 4 oct | Germany, Berlin | Probably c-base |
| ❓ | [OsmoDevCon](https://osmocom.org/projects/osmo-dev-con/wiki/OsmoDevCon) | TBD | Germany, Berlin | Lehrter Str. 53 |

### Past (2026)

| Event | Dates | Where | Details |
|-------|-------|-------|---------|
| [FOSDEM](https://fosdem.org/2026/) | 31 jan – 2 feb | Belgium, Brussels | ULB Campus Solbosch, Avenue Franklin D. Roosevelt 50 |
| [TechFurMeet](https://www.techfurmeet.org/en/schedule) | 12 feb – 16 feb | Czechia, Horní Bradlo-Nasavrky | 538 25 |
| [Handycon](https://2026.handycon.xyz/) | 4 mar – 6 mar | Remote | — |
| [Madeira Bitcoin Meetup](https://www.satlantis.io/events/1692/FREE-Madeira-Bitcoin-Meetup) | 25 mar | Portugal, Funchal | Cowork Funchal, R. das Mercês 41 |

<details>
<summary><strong>Past (2025)</strong></summary>

| Event | Dates | Where | Details |
|-------|-------|-------|---------|
| [SatsNFacts](https://satsnfacts.btc.pub/#agenda) | feb | Thailand, Chiang Mai | — |
| [Pizza Day](https://2025.pizzaday.cz/) | 17 may – 18 may | Czechia, Prague | — |
| [Freedom Forum](https://archive.hrf.org/apply-to-attend-the-2025-oslo-freedom-forum/) | 26 may – 28 may | Norway, Oslo | — |
| [Battle Mesh v17](https://www.battlemesh.org/BattleMeshV17) | 10 jun – 16 jun | Germany, Sundhausen | — |
| [Zitadelle](https://pay.einundzwanzig.space/apps/4DtdvBYv6JpyvKtsGRUAjfSLmtKp/pos) | 24 jul – 27 jul | Germany, Wasserburg Heldrungen | — |
| [BTC++ Riga](https://btcplusplus.dev/conf/riga) | 7 aug – 8 aug | Latvia, Riga | — |
| [Baltic Honey Badger](https://baltichoneybadger.com/buy-tickets) | 9 aug – 10 aug | Latvia, Riga | — |
| [BTC++ Berlin](https://btcplusplus.dev/conf/berlin25) | 2 oct – 4 oct | Germany, Berlin | — |
| [Dark Prague](https://secondculture.cz/cs/events/dp25/) | 3 oct – 5 oct | Czechia, Prague | Dělnická 475/43, Praha 7 |
| [Nostr Hackday](https://nostrhackday.orangesync.tech) | 5 oct | Germany, Berlin | Holzmarktstraße 25 |
| [The Canadian Bitcoin Conference](https://canadianbitcoinconf.com/) | 16 oct – 18 oct | Canada, Montreal | Rialto Theatre |
| [Bitcoin Builders Meetup Berlin](https://meetu.ps/e/Ps9mB/TVMWr/i) | 28 oct | Germany, Berlin | Möckernstrasse 120 |
| [Mining Disrupt](https://miningdisrupt.com/) | 11 nov – 13 nov | USA, Dallas | — |
| [Bitcoin Amsterdam](https://www.bitcoin.amsterdam/) | 13 nov – 14 nov | Netherlands, Amsterdam | Sugar Factory |
| [Adopting Bitcoin El Salvador](https://adoptingbitcoin.org/) | 14 nov – 15 nov | El Salvador, Nuevo Cuscatlán | Carretera Huizucar |
| [Bitfest Manchester](https://bitfest.uk/) | 21 nov – 23 nov | UK, Manchester | Pendulum Hotel |
| [BTC++ Taipei](https://btcplusplus.dev/conf/taipei) | 15 dec – 17 dec | Taiwan, Taipei | No. 13, Section 2, Nangang Rd, Nangang District |

</details>

## License

Licensed under the [MIT License](LICENSE).
