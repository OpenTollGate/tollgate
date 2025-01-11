# TollGate goes Nostr native
There are a lot of moving parts to tollgate that all have to work together and need to run on the router itself. Which in the first prototypes has posed to be a challenge, often requiring to recompile all binaries from scratch.
# Previous Challenges 
- Slow Development cycle (recompiling) and limited use of available tooling
- Shell scripts not scalable
- ... - **@chandran** add stuff here...
# Why Nostr?
- Ability to Connect without captive portals
- Fast, immediate responses out of the box (relay websocket connections)
- *Little intro on what it is (maybe separate 'What is Nostr' heading)*
- Less firewall friction because of relay
- (Most) components can run anywhere with little to no configuration changes
- Developer friendly, Very extendable
- Standardized way for payments (across applications) 

# The new modular design


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











-------------------------

### Draft thoughts

This writeup details how nostr supercharges TollGate and how you can benefit from it as a developer. We will cover the benefits of using nostr to interact with devices that are behind a firewall, how nostr makes TollGate modular and the minimum requirements for turning your device into a TollGate.

![[tollgate_modules.jpeg]]


#### Interacting with  devices behind a firewall

Nostr follows a "smart client, dumb server" principle. Every user has public key (npub) and a corresponding secret key (nsec). Users who want to interact with each other use `nostr clients` to create `nostr events`, which are standardized JSONs signed by their public keys. Any user who receives a nostr event signed by an npub that they follow can check the integrity of the event by verifying the signature - irrespective of how the event reaches the user's nostr client. Hence, DNS and TCP/IP are *not required by the nostr protocol*. 

Each nostr user selects a set of relays that it wants to broadcast its events to and receive events from. Users can receive each other's events from the relays as long as they have an overlap in their relay lists. Nostr clients make *outbound connections* to publicly accessible relays, so they are not affected by firewalls that block inbound traffic.

#### How you can turn your device into a TollGate

* Minimal: Crows nest + merchant or valve + herald
* Nice to have: fleet manager, merchant, relay - citrine?