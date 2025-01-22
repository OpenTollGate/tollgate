# TollGate

TollGate enables WiFi routers to accept Bitcoin payments for internet access. This allows users with a router and an internet connection to operate as internet service providers. The project is currently undergoing significant architectural improvements thanks to new possibilities that nostr brings to the table. The new architecture of TollGate is presented [here](https://opentollgate.github.io/tollgate/nostr-powering-tollgate).

## Frequently Asked Questions

- [For Non-Technical Bitcoiners](https://opentollgate.github.io/tollgate/markdown_faqs/12.01.2025_for_non_technical_bitcoiners) (last edited: January 12, 2025)
- [For Technical Bitcoiners](https://opentollgate.github.io/tollgate/markdown_faqs/12.01.2025_for_technical_bitcoiners) (last edited: January 12, 2025)
- [Written before we had the improved architecture](https://npub1suw0zfxerywd4zku4gjsjde22zhzye9dl2hsll6s3z2qap75p78s66lkhp.nsite.orangesync.tech) (last edited: October/November 2024)

Note: New FAQs will be added over time to reflect the latest developments in TollGate. The date indicates when each guide was last modified to ensure you're reading the most current information.

## Repositories
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

## Contributing

We welcome contributions!