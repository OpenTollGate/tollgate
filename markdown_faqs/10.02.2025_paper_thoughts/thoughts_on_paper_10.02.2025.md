
# Thoughts on TollGate related [academic paper](https://dl.acm.org/doi/pdf/10.1145/3529097)

Recently someone shared an academic paper that investigates methods for WiFi routers to pay each-other. One take-away from the paper was that TollGate's documentation assumes readers are already familiar with Bitcoin, lightning and e-cash, which may not be the case for many people who could be interested in TollGate.
## Terminology
The paper frequently mentions "payment channel networks", though it doesn't explain what a payment channel network is. I concisely explained the [terminology](./terminology.md) here for those who are new to digital payments.
### Comments on quotes from the paper

> Leroy et al. [5] further notice that asymmetric communication channels cannot guarantee fairness regarding the billing of services without including a trustable intermediary or implementing expensive hardware for using complex consensus mechanisms. 

Exactly! In TollGate the e-cash mint is the trusted intermediary. The payer chooses the mint(s) that it wants to pay from and the payee chooses the mint(s) that it wants to receive payments on. They use lightning to swap between mints if there is no overlaps in both parties trusted mints. This could be the case if both the payer and the payee only trust their own mints for instance.

> DP7: Provide the system with a module to keep transaction costs to a minimum in order for the system to prevent large numbers of micro-payments.

e-cash transactions do have a cost, because the mint operator needs to keep track of all the spent e-cash notes inevitably to prevent double spending. However, this cost can be mitigated if mints are designed to retire and users know that they need to leave the mint by a certain date. 

> An Architecture for Blockchain-based Wi-Fi Sharing. The application layer manages all connections within a network and addresses many of the previously defined design principles. 

Agreed, lower layers don't have the fundamental building blocks that are required for this to work. A MAC address can't be used to sign messages for instance. However, we can just sign messages with a nostr private/public key pair if we work on the application layer.

> Ultimately, travellers are typically cautious when using services in foreign countries or unknown locations. Besides transaction and data security, they demand trust-building mechanisms that curtail fraudulent behaviour.

The internet gateway can keep its nostr public key to build up a reputation if it wants to be seen as trustworthy. 

Minimising trust requirements:
* The user needs to have a VPN running in their client device for privacy
* The user should make small frequent purchases to protect himself from rug pulls
* The gateway that KYCs with a fiat ISP needs a VPN to protect itself from the user's traffic
### Whats nostr?
There was a [talk explaining nostr](https://youtu.be/Tbt3jL1Ms0w?si=hpgs6IRNf7omoGPw) at fosdem recently.

