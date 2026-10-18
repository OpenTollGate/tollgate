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

Where the TollGate issues its own vouchers rather than accepting external mint tokens, see
[TIP-04](TIP-04.md) for the acquisition/redemption split. Acquisition is intentionally out of scope
there: a voucher is an ordinary Cashu token, so any Cashu-native mechanism can supply it.

```
cashuB...
```

## Payment channels (discussion)

A TollGate advertising `payment_model: rate-reserved` (TIP-03) settles over a payment channel rather than
a per-purchase token. Channel lifecycle, durability and refund requirements are proposed in
[TIP-05](TIP-05.md) and are **not yet ratified**.

