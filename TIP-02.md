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
	- `<min_steps>` Minimum amount of steps to purchase using this mint. Positive whole number, default 0 ⚠️ TENTATIVE: Strucuture of incorporation of fees/min purchases is not final

- The value of `<price>` MUST be the same across all occurrences of the same `<unit>` value.

### Example
```json
{
    "kind": 10021,
    // ...
    "tags": [
        // <TIP-01 tags>
        ["price_per_step", "cashu", "210", "sat", "https://mint.domain.net", 1],
        ["price_per_step", "cashu", "210", "sat", "https://other.mint.net", 1],
        ["price_per_step", "cashu", "500", "eur", "https://mint.thirddomain.eu", 3],
    ]
}
```

## Payment

The customer sends a Cashu token directly over the TollGate's supported transport(s). The allotment the customer receives is relative to the value of the token sent.

```
cashuB...
```
