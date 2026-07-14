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
        ["price_per_step", "<bearer_asset_type>", "<price>", "<unit>", "<mint_url>", "<min_steps>", "<max_steps>"],
        ["price_per_step", "...", "...", "...", "...", "...", "..."],
    ]
}
```

Tags:
- `price_per_step`: (one or more)
	- `<bearer_asset_type>` Always `cashu`.
	- `<price>` price for purchasing 1 time the `step_size`.
	- `<unit>` unit or currency.
	- `<mint_url>` Accepted mint. Example: 210 sats per minute (60000ms)
	- `<min_steps>` (Optional) Minimum amount of steps a customer must purchase using this mint. Positive whole number, default 1. Allows a TollGate to require a larger minimum purchase for mints with high processing overhead.
	- `<max_steps>` (Optional) Maximum amount of steps a customer may purchase using this mint. Positive whole number, default 0 (unlimited). Allows a TollGate to cap how much access it commits to upfront — for example, refusing to sell 100 GB to a stranger when the TollGate cannot guarantee that level of uptime (battery constraints, upstream reliability, etc.).

- The value of `<price>` MUST be the same across all occurrences of the same `<unit>` value.

### Example
```json
{
    "kind": 10021,
    // ...
    "tags": [
        // <TIP-01 tags>
        ["price_per_step", "cashu", "210", "sat", "https://mint.domain.net", 1, 0],
        ["price_per_step", "cashu", "210", "sat", "https://other.mint.net", 1, 0],
        ["price_per_step", "cashu", "500", "eur", "https://mint.thirddomain.eu", 3, 100],
    ]
}
```

## Payment

The customer sends a Cashu token directly over the TollGate's supported transport(s). The allotment the customer receives is relative to the value of the token sent.

```
cashuB...
```
