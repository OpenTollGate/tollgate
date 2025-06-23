
![tollgate-logo](TollGate_Logo-C-black.png)

[TollGate](https://tollgate.me/) enables WiFi routers to accept Bitcoin payments for internet access. This allows users with a router and an internet connection to operate as internet service providers. One of the reasons mesh networks haven't been widely adopted is that infrastructure operators didn't have a simple _permission-less way to transfer the operating cost of the infrastructure to its users_. Thanks to Bitcoin + e-cash, TollGate operators can transfer the cost of their internet gateway to the users of their access point in a granular manner.

## TollGate Protocol
You can find a draft of the TollGate protocol (split up in multiple TollGate Implementation Possibilities) [here](protocol/README.md)

## TollGate network
![](https://cdn.satellite.earth/fdbdf6be3d7f73612501ad39f23647a3dfda56563fe68f10b886da5fd6d94cb1.png)
The image above shows what a network built out of independently operated TollGates looks like. The is be somebody that connects a TollGate to their a ISP and then the TollGate starts reselling that connection. Other TollGates can buy access and extend the network without having to ask for permission or know who the initial operator is. This can continue as many levels deep as the market will find to be efficient. Anywhere in the network, consumers can do the same thing to purchase internet access on their personal devices. The only difference between the consumer app and another TollGate is that the consumer device most likely won't resell the connection.

## Components that make up TollGate
![](https://cdn.satellite.earth/62630eb4f8f2c9c8f088e397e6fd12dbb3c406125b3703ffa9e2f0bf331dffbd.png)
TollGate software is not just one app or component, but is made up out of several services that each handle their own specialized task. Together it forms the TollGate ecosystem. The communication between some of these components will be the TollGate protocol, but it's still to early to solidify it.

### TollGate Repositories
The TollGate ecosystem consists of four main projects:

- **tollgate-protocol**: The open protocol definition that standardizes how TollGate components communicate. Currently housed in this repository under the 'protocol' directory, but will be moved to its own dedicated repository.
  
- **tollgate-core**: The reference implementation of the TollGate protocol specifically designed for OpenWRT routers. Currently referred to as tollgate-module-basic-go, this component contains the core functionality needed to implement the TollGate protocol.
  
- **tollgate-os**: A ready-made OpenWRT image with all necessary components pre-installed. This allows you to transform supported routers into fully functional TollGates without much manual configuration.
  
- **tollgate-installer**: A tool made to simplify the process of flashing TollGateOS onto new routers, making deployment accessible for users of varying technical backgrounds.

The project is currently undergoing significant architectural improvements thanks to new possibilities that nostr brings to the table. The new architecture of TollGate is introduced [here](https://opentollgate.github.io/tollgate/nostr-powering-tollgate). When you modify one of the above modules, the module is package by an [SDK](https://github.com/OpenTollGate/tollgate-sdk) and pushed to blossom in a github action. You can later use the [imagae builder](https://github.com/OpenTollGate/tollgate-image-builder) to generate an OpenWRT image for the actions.


## Benefits
Benefits for users:
- **Sharing Costs:** Communities can share the cost of running internet infrastructure. TollGates that resell access to this gateway expand the network geographically outwards from the gateway - thus increasing the purchasing power that the network can present to it.
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

## Project Timeline

<table>
  <tr>
    <td align="center">📅 <strong>2012</strong></td>
    <td>💡 Initial idea of paying neighbouring routers for higher bandwidth via multi-wan</td>
  </tr>
  <tr>
    <td align="center">📅 <strong>Mid 2024</strong></td>
    <td>🧪 Early proof of concept by pasting ecash in captive portal password field</td>
  </tr>
  <tr>
    <td align="center">📅 <strong>Late 2024</strong></td>
    <td>🔄 Introduced golang / nostr to the routers</td>
  </tr>
  <tr>
    <td align="center">📅 <strong>April 2025</strong></td>
    <td>📝 First protocol draft</td>
  </tr>
</table>


## FAQ

### General
#### What is an ISP?
ISP stands for Internet Service Provider. ISP's are companies that provide your home with internet access and charge a monthly fee for it. For example: Comcast, Verizon and AT&T ar all ISP's.

#### Can I share my internet plan with my neighbors?
Yes, if you operate a TollGate, your neighbour can connect to your TollGate instead of their own ISP's access-point.

### Risks
#### Is TollGate safe to use?
TollGate networks are public/open WiFi networks, like the ones you find in hotels and airports. These kind of networks have risks associated to it and we recommend always using a VPN when using public networks.
Additionally, TollGate development is still in very early stages and the project still changes a lot. We cannot make any promises about security and privacy at this point in time.

### Using TollGate
#### Is TollGate free?
The software we publish is free and open-source, meaning you will not be charged for the use of TollGate software. However, the independent TollGate operators (the people providing the access-points) will charge you as the user for their services. The price they set it not controlled by the TollGate open-source project.


### Operating a TollGate

#### What is TollGateOS?
TollGateOS is a version of OpenWRT that has all software included that is needed to turn a router into a TollGate.

#### How can I operate a TollGate?
We provide TollGateOS images which you can install on our supported routers. Other routers may work but we do not currently test for those.

#### Which hardware is supported?
Currently TollGateOS is compatible with:
- GL.iNet MT-3000

TollGate's long-term aim is to support many OpenWRT compatible routers.

#### Is it legal to operate a TollGate?
This depends a lot on the jurisdiction you are operating the TollGate in. In some countries/regions you might take liability for the traffic that runs through your TollGate, be sure to check your local rules and regulations. Additionally, check your ISP's Terms & Conditions for any rules around reselling internet access.

It is your own responsibility to handle local rules and regulations, we cannot assist you with that.

## Additional reading

- [Thoughts academic paper about paying for internet access](markdown_faqs/10.02.2025_paper_thoughts/thoughts_on_paper_10.02.2025.md) (last edited: February 11, 2025)
- [TollGate android client works!](https://njump.me/nevent1qqsv7s9h4y0aklk5rfy28d3d6wgua0pafuaesjuewt6tfrq3lnuxh9cpzemhxue69uhk7unpdenk2umede3juar9vd5z7q3qzzt0d0s2f4lsanpd7nkjep5r79p7ljq7aw37eek64hf0ef6v0mxqxpqqqqqqz64slvx) (last edited: January 31, 2025)
- [Building TollGate Image](https://primal.net/e/nevent1qqsrkfkdwy8q693vtrs789t7gf0vt86avkp7ufcmn5j0rhuvpr35zsqpp4mhxue69uhkummn9ekx7mqzyze7y04hydanvkk2n8ya9rc8pgc56cdgxlnm6r7tkrcrjnwuld3psge2xzv) (last edited: January 28, 2025)
- [For Non-Technical Bitcoiners](https://opentollgate.github.io/tollgate/markdown_faqs/12.01.2025_for_non_technical_bitcoiners) (last edited: January 12, 2025)
- [For Technical Bitcoiners](https://opentollgate.github.io/tollgate/markdown_faqs/12.01.2025_for_technical_bitcoiners) (last edited: January 12, 2025)
- [Written before we had the improved architecture](https://npub1suw0zfxerywd4zku4gjsjde22zhzye9dl2hsll6s3z2qap75p78s66lkhp.nsite.orangesync.tech) (last edited: October/November 2024)

Note: New articles will be added over time to reflect the latest developments in TollGate. The date indicates when each guide was last modified to ensure you're reading the most current information.


## Events
Feel free add events to the list via pull request if you plan to present TollGate somewhere.
### Events TollGate contributors intend to visit
* [Zitadelle](https://pay.einundzwanzig.space/apps/4DtdvBYv6JpyvKtsGRUAjfSLmtKp/pos) - Wasserburg Heldrungen, `24.07.2025 - 27.07.2025`
* [BTC++ Riga](https://btcplusplus.dev/conf/riga) - Riga, `07.08.2025 - 08.08.2025`
* [Baltic Honey Badger](https://baltichoneybadger.com/buy-tickets) - Riga, `09.08.2025 - 10.08.2025`
* [Nostr Hackday](https://primal.net/p/npub1w8lxsgc67y5jmvgfhjqrxtkj0gnxtrcsvrtzh97kpd66pw35e5gstn94qr) - Berlin, `before BTC++`
* [BTC++ Berlin](https://btcplusplus.dev/conf/berlin25) - Berlin, `02.10.2025 - 04.10.2025`
* [Africa Bitcoin Conference](https://afrobitcoin.org/speakers/) - Caudan Arts Centre in Port Louis, Mauritius, `3.12.2025 - 5.12.2025`

#### Unconfirmed events
* [Nostr World]([https://baltichoneybadger.com/buy-tickets](https://nostr.world/)) - Buenos Aires, `Q4 2025`

### Past events
* [Battle Mesh](https://www.battlemesh.org/BattleMeshV17) - Sundhausen, `10.06.2025 - 16.06.2025` (presentation)
* [Freedom Forum](https://archive.hrf.org/apply-to-attend-the-2025-oslo-freedom-forum/) - Oslo, `26.05.2025 - 28.05.2025`
* [Pizza Day](https://2025.pizzaday.cz/) - Prague, `17.05.2025 - 18.05.2025`
* [SatsNFacts](https://satsnfacts.btc.pub/#agenda) - Chiang Mai - February 2025
	* [Presentation](https://www.figma.com/deck/uws5VZOIJLSta91UqhoBlj/Untitled?node-id=1-578&t=vpFgI0GNI29BH3qf-0&scaling=min-zoom&content-scaling=fixed&page-id=0%3A1) introducing TollGate

## Contributing
We welcome contributions! Feel free to reach out [on nostr](https://nostrudel.ninja/#/t/tollgate), [on signal](https://signal.group/#CjQKIFUj7wFVxIbHjujYAWGTKeMIvi1DdVYk2Zem3uNlmIwWEhAiFlw7arUvnitB1V0_cTaA) or in person.. [Endorsements on nostr](https://primal.net/e/nevent1qqspyv27cmc3j4dycyfypa2yxlr3qkqqumsnppk6mamxtx5njnyxmmqpp4mhxue69uhkummn9ekx7mqppamhxue69uhkummnw3ezumt0d5pzq6lwjq0mseqmr4lkt72jgqlemgcewuk0my8chwkuktmzjahfc5gdknd9qg), the TollGate echo chamber :)

### TollGate Repositories
The following repositories are currently part of TollGate:

* [TollGate documentation](https://vnext.gitworkshop.dev/npub1c03rad0r6q833vh57kyd3ndu2jry30nkr0wepqfpsm05vq7he25slryrnw/tollgate) repository

#### Actions
* TollGate [blossom upload action](https://vnext.gitworkshop.dev/npub1c03rad0r6q833vh57kyd3ndu2jry30nkr0wepqfpsm05vq7he25slryrnw/upload-blossom-action)
* TollGate [SDK action](https://vnext.gitworkshop.dev/npub1c03rad0r6q833vh57kyd3ndu2jry30nkr0wepqfpsm05vq7he25slryrnw/tollgate-sdk)
* TollGate [image builder](https://vnext.gitworkshop.dev/npub1c03rad0r6q833vh57kyd3ndu2jry30nkr0wepqfpsm05vq7he25slryrnw/tollgate-image-builder)
* Nostr file [metadata publishing action](https://vnext.gitworkshop.dev/npub1c03rad0r6q833vh57kyd3ndu2jry30nkr0wepqfpsm05vq7he25slryrnw/nostr-publish-file-metadata-action)

#### Modules
* TollGate [relay module](https://vnext.gitworkshop.dev/npub1c03rad0r6q833vh57kyd3ndu2jry30nkr0wepqfpsm05vq7he25slryrnw/tollgate-module-relay-go)
* TollGate [valve module](https://vnext.gitworkshop.dev/npub1c03rad0r6q833vh57kyd3ndu2jry30nkr0wepqfpsm05vq7he25slryrnw/upload-blossom-action)

#### Decomissioned
* [Custom nostr feed](https://vnext.gitworkshop.dev/npub1c03rad0r6q833vh57kyd3ndu2jry30nkr0wepqfpsm05vq7he25slryrnw/custom-nostr-feed-decomissioned) for packaging with the OpenWRT SDK


## UNLICENSE
This is free and unencumbered software - see the [UNLICENSE](UNLICENSE) file for details.
