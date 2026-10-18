# TIP-03 - Payment model advertisement (DISCUSSION DRAFT)

**Status: proposal for discussion. Not a ratified TIP.** The number `TIP-03` is a placeholder.

## Problem

TIP-01's `10021` describes **price**, not **payment model**. A TollGate selling prepaid blocks of steps
and one selling continuous rate-reserved access over a payment channel are indistinguishable to a
client until a payment attempt fails. Two implementations now share the name TollGate and sell
differently: the TIP/HTTP design (a token worth N steps) and `tollgate-rs` (one voucher = one byte over
a Cashu Spilman channel). Neither spec mentions the other.

## Proposal

One optional tag on `10021`:

| Tag | Values | Meaning |
|---|---|---|
| `payment_model` | `steps` (default), `rate-reserved` | how access is bought and metered |

- **Absent MUST be treated as `steps`**, so every deployed advertisement keeps its current meaning.
- `steps` = today's TIP-02 behaviour.
- `rate-reserved` = a budget with a deadline and a reserved rate (see below).

### Tags meaningful only with `payment_model: rate-reserved`

| Tag | Value | Notes |
|---|---|---|
| `voucher_mint` | mint URL | the mint that issues this TollGate's vouchers |
| `min_rate` | integer, base units/second | floor for a reservation |
| `max_rate` | integer, base units/second | ceiling, so a buyer can refuse before paying |
| `from_payer_weight` | integer | upload unit cost relative to download (`10` = a tenth; `0` = free) |

`from_payer_weight` exists in `tollgate-rs` (`grants.from_payer_weight`) and is load-bearing: without it
a buyer cannot apply its documented refusal — *"refuse a provider whose weight is too high, before any
money moves."*

### Example

```json
{
    "kind": 10021,
    "tags": [
        ["metric", "bytes"],
        ["step_size", "1000000"],
        ["tips", "1", "2", "3"],
        ["payment_model", "rate-reserved"],
        ["voucher_mint", "https://mint.example.net"],
        ["min_rate", "12500"],
        ["max_rate", "2500000"],
        ["from_payer_weight", "10"]
    ]
}
```

## Open questions

1. Is one `payment_model` tag enough, or should it be a capability list like `tips`?
2. Does `step_size` keep its meaning under `rate-reserved`, or is a `min_unit` needed?
3. A new TIP, or a section inside TIP-02?

## Compatibility

Additive only. A TollGate emitting none of these tags behaves exactly as today; a client that ignores
`payment_model` still works against a `steps` TollGate.
