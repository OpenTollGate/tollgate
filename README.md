# TollGate

TollGate enables WiFi routers to accept Bitcoin payments for internet access. This allows users with a router and an internet connection to operate as internet service providers. One of the reasons mesh networks haven't been widely adopted is that infrastructure operators didn't have a simple _permission-less way to transfer the operating cost of the infrastructure to its users_. Thanks to Bitcoin + e-cash, TollGate operators can transfer the cost of their internet gateway to the users of their access point in a granular manner.

Benefits for users:  
- **Shared gateway:** neighbours can share their internet gateway's cost. TollGates that resell access to this gateway expand the network geographically outwards from the gateway - thus increasing the purchasing power that the network can present to it.
- **Reliability and bandwidth:** users and/or TollGate operators who are concerned about the reliability of multi-hop internet connections can use multi-WAN to create redundancy for themselves and their users. Multi-WAN also allows TollGates to aggregate bandwidth from multiple gateways, thus giving them the ability to deliver faster internet than individual legacy ISPs provide.
- **Privacy:** Internet service provision typically involves certain regulatory compliance requirements regarding user data collection and retention. TollGate's distributed architecture naturally shifts operational responsibilities to individual network participants. The ability to process micro-payments through e-cash enables granular service delivery without requiring identity verification. This architecture, combined with frequent small transactions, can help preserve user privacy while maintaining network functionality.

Opportunity for TollGate operators:  
- **Arbitrage:** a TollGate that succeeds in connecting the non KYC network to a cheaper gateway benefits from an arbitrage opportunity and further lowers the price of internet access on the non KYC network.
- **Global services:** TollGate operators can sell access to the local network globally, thus enabling a market for IP addresses that otherwise wouldn't be available on the traditional VPN market.

Effect on legacy internet service providers:
- **Loss of moat:** ISPs that offer fiat/KYC contracts for unlimited data would need to adjust their offering to a per GB based model in order to remain competitive and to throttle the growth of TollGate based ISPs.
- **Fewer contracts sold:** since multiple users share a single contract legacy ISPs can expect to see a drop in sales and a rise in data usage from the contracts that remain.
- **Increased competition for users:** many Bitcoin miners run at a loss, because there is nothing stopping a miner who sees opportunity from participating in the market for hash-rate. Our goal is to ensure that nothing can stop TollGate operators from participating in the market for connectivity, thus delivering a close to optimal outcome for the user.
- **Decreased competition for limited spectrum:** since users can access their neighbour's routers, they no longer need to operate their own in areas where the spectrum is congested. This enables everyone to get a better throughput, because less can be more on a shared medium.

## Frequently Asked Questions

- [Thoughts academic paper about paying for internet access](markdown_faqs/10.02.2025_paper_thoughts/thoughts_on_paper_10.02.2025.md) (last edited: February 11, 2025)
- [TollGate android client works!](https://njump.me/nevent1qqsv7s9h4y0aklk5rfy28d3d6wgua0pafuaesjuewt6tfrq3lnuxh9cpzemhxue69uhk7unpdenk2umede3juar9vd5z7q3qzzt0d0s2f4lsanpd7nkjep5r79p7ljq7aw37eek64hf0ef6v0mxqxpqqqqqqz64slvx) (last edited: January 31, 2025)
- [Building TollGate Image](https://primal.net/e/nevent1qqsrkfkdwy8q693vtrs789t7gf0vt86avkp7ufcmn5j0rhuvpr35zsqpp4mhxue69uhkummn9ekx7mqzyze7y04hydanvkk2n8ya9rc8pgc56cdgxlnm6r7tkrcrjnwuld3psge2xzv) (last edited: January 28, 2025)
- [For Non-Technical Bitcoiners](https://opentollgate.github.io/tollgate/markdown_faqs/12.01.2025_for_non_technical_bitcoiners) (last edited: January 12, 2025)
- [For Technical Bitcoiners](https://opentollgate.github.io/tollgate/markdown_faqs/12.01.2025_for_technical_bitcoiners) (last edited: January 12, 2025)
- [Written before we had the improved architecture](https://npub1suw0zfxerywd4zku4gjsjde22zhzye9dl2hsll6s3z2qap75p78s66lkhp.nsite.orangesync.tech) (last edited: October/November 2024)

Note: New FAQs will be added over time to reflect the latest developments in TollGate. The date indicates when each guide was last modified to ensure you're reading the most current information.

## Repositories

The project is currently undergoing significant architectural improvements thanks to new possibilities that nostr brings to the table. The new architecture of TollGate is presented [here](https://opentollgate.github.io/tollgate/nostr-powering-tollgate).

### Modules
- [Valve (go)](https://github.com/OpenTollGate/tollgate-module-valve-go), Opens/closes gate for customer
- [Relay (go)](https://github.com/OpenTollGate/tollgate-module-relay-go), Nostr relay
- [Crowsnest (go)](https://github.com/OpenTollGate/tollgate-module-crowsnest-go), Searches for upstream Tollgates
- [Whoami (go)](https://github.com/OpenTollGate/tollgate-module-whoami-go), Service that allows mobile clients to fetch their own MAC address
- [Merchant (go)](https://github.com/OpenTollGate/tollgate-module-merchant-go), Handles finances, create sessions
- Herald (go), Creates announcements + Beacon Frames ##TODO

### User interface
- Crowsnest (Tauri), Native user-interface  ##TODO
  - MacOS 
  - Windows
  - Linux
  - Android
  - IOS

## Roadmap (December 2024)
- [ ] Switch to GoLang
	- [x] Get golang binary running on GL-AR300M
	- [x] Get go-nostr library running on GL-AR300M
	- [ ] Create OPKG package
- [ ] Whitelist relay for unauthenticated users
	- [x] Public relay
	- [ ] Relay within network
	- [x] Relay on-device (router)
- [ ] Router support:
	- [x] Low-tier: GL-AR300M
	- [ ] Mid-tier: GL-MT3000
	- [ ] High-tier GL-MT6000
- [ ] POC
	- [ ] Machine-to-machine purchasing
		- [x] Build Valve component (open/close gate)
		- [ ] Build Herald component (announcements)
			- [ ] Add pubkey to Beacon frames
		- [ ] Build Crowsnest component (scanning)
		- [ ] Build Merchant component (decision-making)
	- [ ] Human-to-machine purchasing
		- [ ] Captive portal
			- [ ] Serve JS-Compatible portal
			- [ ] Portal can display/access MAC address
			- [ ] Portal can display/access router IP
- [ ] Nostr based MVP
	- [ ] Strict authorization between components
	- [ ] Encrypt internal Nostr messages
- [ ] Product V1
	- [ ] Automatic updates (+ verify binaries)
	- [ ] Dashboard to mange Tollgates
	- [ ] ...
- [ ] Product V2
	- [ ] Android app to auto-purchase from tollgate
	- [ ] ...

## Events
Feel free add events to the list via pull request if you plan to present TollGate somewhere.
### Events TollGate contributors intend to visit
* [Pizza Day](https://2025.pizzaday.cz/) - Prague - May 2025
* [Freedom Forum](https://archive.hrf.org/apply-to-attend-the-2025-oslo-freedom-forum/) - Oslo - May 2025
* [Battle Mesh](https://www.battlemesh.org/BattleMeshV17) - Sundhausen - June 2025
* [Budapest Bitcoin](https://budapestbitcoin.com/) - Budapest - August 2025
* [Nostr Hackday](https://primal.net/p/npub1w8lxsgc67y5jmvgfhjqrxtkj0gnxtrcsvrtzh97kpd66pw35e5gstn94qr) / [BTC++](https://btcplusplus.dev/) - Berlin - October 2025

### Past events
* [SatsNFacts](https://satsnfacts.btc.pub/#agenda) - Chiang Mai - February 2025
	* [Presentation](https://www.figma.com/deck/uws5VZOIJLSta91UqhoBlj/Untitled?node-id=1-578&t=vpFgI0GNI29BH3qf-0&scaling=min-zoom&content-scaling=fixed&page-id=0%3A1) introducing TollGate

## Contributing
We welcome contributions! Feel free to reach out on nostr or in person.. 

### Endorsements
* [On nostr](https://primal.net/e/nevent1qqspyv27cmc3j4dycyfypa2yxlr3qkqqumsnppk6mamxtx5njnyxmmqpp4mhxue69uhkummn9ekx7mqppamhxue69uhkummnw3ezumt0d5pzq6lwjq0mseqmr4lkt72jgqlemgcewuk0my8chwkuktmzjahfc5gdknd9qg): the TollGate echo chamber :)
