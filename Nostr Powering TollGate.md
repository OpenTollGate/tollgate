
This writeup details how nostr supercharges TollGate and how you can benefit from it as a developer. We will cover the benefits of using nostr to interact with devices that are behind a firewall, how nostr makes TollGate modular and the minimum requirements for turning 
your device into a TollGate.

##  What is a TollGate?

TollGate is a set of tools that enables WiFi routers to accept Bitcoin payments for internet access. This allows anyone with suitable hardware and an internet gateway to operate as an internet service provider.

### What are the hardware requirements?

A TollGate must have at least two TCP/IP interfaces. One interface connects to an internet gateway, while the other interface acts as a WiFi access point for users or other TollGates. Each TollGate runs its own DHCP server with a unique address range to avoid collisions with its gateway or its clients - who could also be running a DHCP server. Hence, each TollGate is a stand-alone system from a networking perspective. Finally, each TollGate has gate-keeping logic that manages firewall rules for allowing clients to access the internet gateway once they have paid and to cut them off again when their session ends.

Currently we are targeting [GL-AR300m](https://www.gl-inet.com/products/gl-ar300m/), [GL-MT300](https://www.gl-inet.com/products/gl-mt3000/?utm_source=website&utm_medium=menubar) and [GL-MT600](https://www.gl-inet.com/products/gl-mt6000/) because they come with uboot and they support OpenWRT out of the box.

### TollGate's New Architecture

Previously we used WiFi captive portals like [nodogsplash](https://github.com/nodogsplash/nodogsplash) and [OpenNDS](https://github.com/openNDS/openNDS) for managing access to the internet gateway. These captive portals allowed us to quickly build a proof of concept. However, both captive portals came with severe UX and maintainability limitations - see appendix.

A key benefit of building on `nostr` is that `services` can generate their own public key based identity (`npub`) in order to send and receive `json` content from other services as long as they have an outbound internet connection that can connect to public IP addresses. Nostr abstracts away the networking layer entirely, so developers don't need to think about things like firewalls, IP-addresses, ports and network boundaries.

Thanks recent success that @origami74 had in running a nostr [relay](https://github.com/fiatjaf/khatru) and a [client](https://github.com/OpenTollGate/tollgate-module-valve-go) on the GL-AR300m, we are now able to build in ways that we could only have dreamt of before. 


![[tollgate_modules.jpeg]]

TollGate is now comprised of three main types of services:

* **Crows Nest:** the crows nest scans for available WiFi access points in search of gateways that are worth connecting to. Upon finding a suitable gateway, the crows nest rotates this devices `MAC address` and `npub` (for privacy), connects to the WiFi access point and submits a `payment event` if required.
* **Valve:** the valve is a service that manages access to the internet gateway. Currently the valve is just a `nostr` wrapper around `ndsctl`, which is a CLI tool for managing user sessions in captive portals. Thanks to the fact that the valve now has an `npub`, it can be controlled via events that are issued by the `merchant` service.
* **Merchant:** the merchant sets prices, redeems e-cash notes from `payment events` that customers issued and it issues events that the valve responds to. Merchants can be light weight and fit on the router if they outsource the e-cash redemption logic to third party APIs. However, thanks to nostr, the merchants don't need to run on the router.
* **Relay:** it simplifies things for internet connected services if the relay has a public IP address - which is the case for most relays. However, it makes sense for each TollGate to have its own relay on the router or in the local network as well so that critical services that are physically located in the local network continue to function when the TollGate loses internet access.

TODO: UML diagram

### What runs where?

Now that merchants are controlling their `valves` via nostr events, it is perfectly fine for the merchant to be a more complex software bundle with an e-cash wallet and lightning support on an `x86 machine`. The merchant can just issue `nostr events` that control the valve irrespective of how the network connecting the the `x86 machine` and the `router` looks. Now that we are able to shuffle services around between devices, TollGate can progress more quickly by making use of whatever skill sets contributors bring to the table.

| Module           | Low hanging fruits | Mid term targets | Long term targets |
| ---------------- | ------------------ | ---------------- | ----------------- |
| Crows Nest       | Router, Mobile     | x86, armhf       | IoT               |
| Valve            | Router             | x86, armhf       | mobile            |
| Merchant         | x86, armhf         | router           | mobile            |
| Limited Merchant | router             | -                | -                 |

### Reasoning for order in which devices are being targeted

### Benefits of using crows nest over captive portal





## On-Device
### 1) Valve
### 2) Herald
### 3) Crowsnest

## Anywhere
- Still recommend to run on device, but not required
### 4) Relay
### 5) Merchant
### 6) Fleet Manager


# Roadmap
- Provide basic protocol, allow everyone to build
- ...

#### How you can turn your device into a TollGate

* Minimal: Crows nest + merchant or valve + herald
* Nice to have: fleet manager, merchant, relay - citrine?

# Benefits of using nostr
- Ability to Connect without captive portals
- Fast, immediate responses out of the box (relay websocket connections)
- *Little intro on what it is (maybe separate 'What is Nostr' heading)*
- Less firewall friction because of relay
- (Most) components can run anywhere with little to no configuration changes
- Developer friendly, Very extendable
- Standardized way for payments (across applications) 

#### Interacting with  devices behind a firewall

Nostr follows a "smart client, dumb server" principle. Every user has public key (npub) and a corresponding secret key (nsec). Users who want to interact with each other use `nostr clients` to create `nostr events`, which are standardized JSONs signed by their public keys. Any user who receives a nostr event signed by an npub that they follow can check the integrity of the event by verifying the signature - irrespective of how the event reaches the user's nostr client. Hence, DNS and TCP/IP are *not required by the nostr protocol*. 

Each nostr user selects a set of relays that it wants to broadcast its events to and receive events from. Users can receive each other's events from the relays as long as they have an overlap in their relay lists. Nostr clients make *outbound connections* to publicly accessible relays, so they are not affected by firewalls that block inbound traffic.







# Appendix: early mistakes to be avoided
TollGate started off using an old `c` based captive portal called `nodogsplash` to send e-cash strings to a shell script that redeemed the payment using 

https://github.com/openwrt/openwrt



- Slow Development cycle (recompiling) and limited use of available tooling
- Shell scripts not scalable
- ... - **@chandran** add stuff here...
- Signer client


