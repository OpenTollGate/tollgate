# TIP-02 - Cashu payments

---

- A TollGate can accept multiple mints and multiple currencies.

## TollGate Discovery

A TollGate that accepts Cashu tokens as payment may advertise its pricing using the following tags.

```json
{
    "kind": 10021,
    // ...
    "tags": [
        // <TIP-01 tags>
        ["price_per_step", "<bearer_asset_type>", "<price>", "<unit>", "<mint_url>", "<min_steps>"],
        ["price_per_step", "...", "...", "...", "...", "..."],
    ]
}
```

Tags:
- `price_per_step`: (one or more)
	- `<bearer_asset_type>` Always `cashu`.
	- `<price>` price for purchasing 1 time the `step_size`.
	- `<unit>` unit or currency.
	- `<mint_url>` Accepted mint. Example: 210 sats per minute (60000ms)
	- `<min_steps>` Minimum amount of steps to purchase using this mint. Positive whole number, default 1: an absent value or `0` MUST be treated as `1`. (Known clients reject advertisements carrying `min_steps=0`, so flooring keeps such advertisements purchasable.)

- The value of `<price>` MUST be the same across all occurrences of the same `<unit>` value.

### Example
```json
{
    "kind": 10021,
    // ...
    "tags": [
        // <TIP-01 tags>
        ["price_per_step", "cashu", "210", "sat", "https://mint.domain.net", "1"],
        ["price_per_step", "cashu", "210", "sat", "https://other.mint.net", "1"],
        ["price_per_step", "cashu", "500", "eur", "https://mint.thirddomain.eu", "3"]
    ]
}
```

## Payment

The customer sends a Cashu token directly over the TollGate's supported transport(s). The allotment the customer receives is relative to the value of the token sent.

```
cashuB...
```

## Payment Modes

A TollGate processes Cashu tokens in one of two modes depending on whether it has direct access to the token's mint. Both modes use the same advertisement format and accept the same bare Cashu tokens.

### Terminal Mode

The TollGate has direct connectivity to the token's mint. It verifies the token (NUT-07 check-state), redeems it (NUT-04 melt), and grants the customer an allotment proportional to the token's face value.

This is the simplest mode and the default expectation for a TollGate with upstream internet access.

### Relay Mode

The TollGate does **not** have direct connectivity to the token's mint — for example, it is an edge device bootstrapping gateway access without internet yet. In relay mode the TollGate:

1. Verifies the token is unspent (NUT-07 check-state) against the mint's URL. If the mint is not reachable, the TollGate MAY accept the token on a best-effort basis and verify later.
2. Forwards the same token **upstream** to buy transit from a TollGate that does have mint connectivity (or is itself closer to the mint).
3. Does **not** redeem the token itself during bootstrap. The token acts as a bearer instrument that flows up the chain until it reaches a TollGate operating in terminal mode that can reach the mint and redeem it.

The terminal TollGate (the hop closest to the mint) redeems the token. Each relay in between earns its margin by delivering **less** connectivity to its downstream peer than it purchased upstream.

```
Customer ──token──> TollGate A (relay, no internet)
                        ──same token──> TollGate B (terminal, has internet)
                                            ──redeem──> Mint
```

TollGate A receives a token worth V sats. It forwards V sats worth upstream. TollGate B grants A connectivity worth V sats (at B's pricing). A delivers less than that to its downstream customer — the skim is A's margin for providing the relay service.

### Post-Bootstrap Transition

Once a relay TollGate gains connectivity to the mint (through its upstream peer), it enters terminal mode for subsequent payments. It can now verify and redeem tokens directly. The bootstrap window is over — the relay has graduated to a terminal TollGate.

A TollGate MAY operate in relay mode for some mints and terminal mode for others simultaneously, depending on which mints it can reach.

### Implications for Implementers

- **Relay mode is not a security gap.** The token is not double-spent. It is verified (check-state) at each hop and redeemed exactly once by the terminal TollGate.
- **No protocol-level distinction is needed in the advertisement.** A TollGate in relay mode advertises the same `price_per_step` tags. The customer does not need to know whether the TollGate operates in terminal or relay mode — the customer experience is identical: pay token, receive connectivity.
- **Mint selection matters.** A relay TollGate should accept tokens from mints that its upstream peer also accepts, since the token must ultimately be redeemed by the terminal hop. The multi-hop chain requires a common mint across all hops.
