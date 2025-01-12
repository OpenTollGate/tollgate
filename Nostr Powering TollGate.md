This writeup details how nostr supercharges TollGate and how you can benefit from it as a developer. We will cover the benefits of using nostr to interact with devices that are behind a firewall, how nostr makes TollGate modular and the minimum requirements for turning 
your device into a TollGate.

##  What is a TollGate?
TollGate is a set of tools that enables WiFi routers to accept Bitcoin payments for internet access. This allows anyone with suitable hardware and an internet gateway to operate as an internet service provider.

### What are the hardware requirements?
A TollGate must have at least two TCP/IP interfaces. One interface connects to an internet gateway, while the other interface acts as a WiFi access point for users or other TollGates. Each TollGate runs its own DHCP server with a unique address range to avoid collisions with its gateway or its clients - who could also be running a DHCP server. Hence, each TollGate is a stand-alone system from a networking perspective. Finally, each TollGate has gate-keeping logic that manages firewall rules for allowing clients to access the internet gateway once they have paid and to cut them off again when their session ends.

Currently we are targeting [GL-AR300m](https://www.gl-inet.com/products/gl-ar300m/), [GL-MT300](https://www.gl-inet.com/products/gl-mt3000/?utm_source=website&utm_medium=menubar) and [GL-MT600](https://www.gl-inet.com/products/gl-mt6000/) because they come with uboot and they support OpenWRT out of the box.

### TollGate's New Architecture
Previously we used WiFi captive portals like [nodogsplash](https://github.com/nodogsplash/nodogsplash) and [OpenNDS](https://github.com/openNDS/openNDS) for managing access to the internet gateway. These captive portals allowed us to quickly build a proof of concept. However, both captive portals came with severe UX and maintainability limitations - see [[#Appendix early mistakes to be avoided]].

A key benefit of building on `nostr` is that `services` can generate their own public key based identity (`npub`) in order to send and receive `json` content from other services as long as they have an outbound internet connection that can connect to public IP addresses. Nostr abstracts away the networking layer entirely, so developers don't need to think about things like firewalls, IP-addresses, ports and network boundaries.

Thanks recent success that @origami74 had in running a nostr [relay](https://github.com/fiatjaf/khatru) and a [client](https://github.com/OpenTollGate/tollgate-module-valve-go) on the GL-AR300m, we are now able to build in ways that we could only have dreamt of before. 

This figure illustrates the modules that TollGate is now comprised of:
![[tollgate_modules.jpeg]]

And this sequence diagram illustrates the interaction between the services:
![[tollgate-sequence-diagram.png]]


TollGate is now comprised of three main types of services and a relay:

* **Crows Nest:** scans for available WiFi access points in search of gateways that are worth connecting to. Upon finding a suitable gateway, the crows nest rotates this devices `MAC address` and `npub` (for privacy), connects to the WiFi access point and submits a `payment event` if required.
* **Valve:** manages access to the internet gateway. Currently the valve is just a `nostr` wrapper around a CLI tool for managing user sessions in captive portals called [ndsctl](https://opennds.readthedocs.io/en/stable/ndsctl.html). Thanks to the fact that the valve now has an `npub`, it can be controlled via events that are issued by the `merchant` service.
* **Merchant:** sets prices, redeems e-cash notes from `payment events` that customers issued and it issues events that the valve responds to. Merchants can be light weight and fit on the router if they outsource the e-cash redemption logic to third party APIs. However, thanks to nostr, the merchants don't need to run on the router.
* **Relay:** simplifies connectivity for services. Nostr relays usually have a public IP address, though TollGates will also have a local relay so that critical services that are in the local network continue to function when the TollGate loses internet access.

### What runs where?
Now that merchants are controlling `valves` via nostr events, it is perfectly fine for the merchant to be a more complex software bundle with an e-cash wallet and lightning support on an `x86 machine`. The merchant can just issue `nostr events` that control the valve irrespective of how the network connecting the `x86 machine` with the `router` looks. Having the ability to shuffle services around between devices like this enables TollGate contributors to create value with the skill sets that they already have more easily.
#### Reasoning for order in which devices are being targeted
The following table provides an overview of the order in which we hope to target various devices for each of TollGate's services:

| Module         | Low hanging fruits | Mid term targets | Long term targets |
| -------------- | ------------------ | ---------------- | ----------------- |
| Crows Nest     | Router, Mobile     | x86, armhf       | IoT               |
| Valve          | Router             | x86, armhf       | mobile            |
| Full Merchant  | x86, armhf         | router           | mobile            |
| Light Merchant | router             | -                | -                 |

##### Crows Nest
Routers need the ability to buy from other routers so that the network can grow outwards from the gateway. Hence, routers need a crows nest for TollGate based networks to scale. Fortunately we already know how to work with OpenWRT routers, so this is becoming a well trodden path for us. Previously, users interated with the WiFi captive portals manually - they still can if they like (see section [[#### Benefits of using crows nest over captive portal]]). 

Users who own a TollGate can have the WiFi UX that they are already used to because they can connect to their own device using a WiFi password like they already do with their home router. They are not making any compromises since they are authenticating their devices on their own TollGate. The crows nest of their TollGate now purchases internet on their behalf and without their device needing to know anything about TollGate.

Requiring that users own a router to get the full TollGate experience could hinder adoption. Sure, they can paste e-cash into the captive portal manually or with a script, but the UX depends on their client device and they have all the down sides of not making granular payments.  Hence, users who want good UX without owning a TollGate would benefit from having a crows nest on their client devices for more granular and reliable data purchases.
##### Valve
Each TollGate needs to have a valve in order to manage access to the internet gateway. The valve needs run on the router we are currently building TollGate on-top of OpenWRT routers. However, ndsctl seems to already have [debian support](https://manpages.debian.org/testing/opennds-daemon-common/ndsctl.1.en.html) so running a valve on an x86 or armhf machine might just be a question of getting the interfaces and network configurations under control. 

Currently there isn't a clear line between ndsctl (which we rely on for gatekeeping) and the captive portal which brings a lot of complexity to TollGate. Hopefully the valve can be more a stand alone solution rather 

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

#### Benefits of using crows nest over captive portal
* **Granularity:** 
* **Control over UX:**
* **Interoperability:**



- Slow Development cycle (recompiling) and limited use of available tooling
- Shell scripts not scalable
- ... - **@chandran** add stuff here...
- Signer client


