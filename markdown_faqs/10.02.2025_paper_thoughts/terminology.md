# Terminology around digital payments
Here is a quick explanation of terms like "payment channel network", "lightning network" and "e-cash".

### Why use lightning
People who want to make on-chain transactions are competing for space on the blockchain. Storing these transactions on the blockchain needs to be costly, otherwise our nodes will get filled with spam transactions and the money will no longer be decentralised. Hence, the health of the money depends on proof of work to rate limit the transactions.

Lightning allows users to make smaller, more frequent transactions once they have established payment channels. Essentially, nodes in the lightning network establish a 2 of 2 multisig wallet (called a channel). Now the two users exchange signatures among each-other to modify who's side of the channel the balance is on.

This setup is non custodial, because users can unilaterally close the lightning channel and withdraw their Bitcoin on chain at any time. This setup also doesn't require that users publish their lightning transactions. They only need to make a public Bitcoin transaction once when they open the channel and once to close the channel. However, a user must have sufficient balance on his side of the channel in order to make a payment. The user can only receive payments if all the balance is on the other side of the channel.

The combination of multiple users with lightning channels results in a payment channel network, where users can pay each other even though they aren't direct neighbours in the network:
![[lightning_network.png]]

### Drawbacks of using lightning in mesh networks
* Both the payer and the payee need periodic access to an online Bitcoin node to ensure that their counter-party can't do anything malicious.
* Lightning addresses (LNURLw) can't be used as bearer assets without trusting the counter-party.

### Prior work on payment channels in mesh networks
* [LNMesh: Who Said You need Internet to send Bitcoin? Offline Lightning Network Payments using Community Wireless Mesh Networks](https://arxiv.org/abs/2304.14559)
	* This paper investigates the limitations of making payments locally when access to an internet gateway has been lost
* [LOT49](https://github.com/global-mesh-labs)
	* This research project investigated the limitations of reducing the overhead in lightning transactions over mesh networks so that they could be conducted with limited bandwidth.

### E-cash
#### Advantages of e-cash in this context
* **Privacy:** the payer and payee don't need to know anything about each-other to do business.
* **Bearer asset:** the payer just needs to have a string of text (e-cash note) to make a payment and he can submit this e-cash note to the recipient without already having internet.

#### Draw backs of e-cash
* **Custodial:** the bearer of an e-cash token needs to trust the mint, though the bearer can operate his own mint if he likes. 
* **Workaround with limitations:** the payee can redeem money from the mint before treating the transaction as settled if he doesn't want to trust the mint that the payer is using.

## Summarising the stack described above
The following diagram depicts the payment stack described above. At the bottom of the stack there are on-chain payments, which are slow and expensive, so they are best used for large critical transactions. The lightning network depends on the blockchain to create trust-less payment channels, but these channels can be used to make any amount of transactions as long as the liquidity is on the right side of the channel. E-cash relies on lightning and/or on-chain payments to deposit in a mint and it is no longer non custodial, but it allows for privacy, small + frequent transactions and e-cash is a bearer asset.
![[state_of_the_art_payment_stack.jpg]]


