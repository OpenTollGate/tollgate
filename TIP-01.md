# TIP-01 - Base Events

---
## TollGate Details / Advertisment
Tollgate Details / Advertisement is expressed through the following Nostr event
```json
{
    "kind": 10021,
    "pubkey": "24d6...3662", // TollGate identity
    // ...
    "tags": [
        ["metric", "<value>"],
        ["step_size", "<value>"],
        ["tips", "1", "2", "5"]
    ]
}
```

Tags:
- `metric`: `milliseconds` or `bytes`
- `min_steps`: (Optional) positive whole number. Defines a minimum amount of steps a customer must purchase at a time. Where a `price_per_step` entry (TIP-02) carries its own `<min_steps>`, that per-mint value takes precedence for that pricing option; the advertisement-level `min_steps` applies only to pricing options that do not carry one.
- `tips`: List of implemented TollGate TIP's represented as numbers

### Example
```json
{
    "kind": 10021,
    "pubkey": "24d6...3662",
    // ...
    "tags": [
        ["metric", "milliseconds"],
        ["step_size", "60000"], // 1 minute step size
        ["tips", "1", "2", "5"]
    ]
}
```

## Session
```json
{
	"kind": 1022,
	"pubkey": "63gy...9xvq", // TollGate identity
	// ...
	"tags": [
		["p", "6hjq...1fi6"], // Customer identity (pubkey)
		["device-identifier", "<type>", "<value>"],
		["allotment", "<amount>"],
		["metric", "<metric>"]
	]
}
```

Tags:
- `p`: pubkey of the TollCustomer Gate (from the Customer's Payment event)
- `device-identifier`: (hardware) identifier of the customer's device.
- `allotment`: Amount of `<metric>` allotted to customer after payment.
- `metric`: The purchased metric
- `rate`: (Optional) units of `<metric>` per second reserved for this session. Present when the
  advertisement carried `payment_model: rate-reserved` (TIP-03). Mutually informative with `allotment`:
  a rate-reserved session is a speed plus a deadline, not a lump sum.
- `deadline`: (Optional) Unix timestamp at which the funded budget expires.
- `balance`: (Optional) units of `<metric>` remaining when the event was published.
- `channel`: (Optional) identifier of the payment channel funding this session (TIP-05).

`allotment` remains REQUIRED for `payment_model: steps` (the default) so existing consumers are
unaffected. For `rate-reserved` sessions `allotment` SHOULD carry the total funded units, and `rate` +
`deadline` + `balance` describe how it is consumed.

## Notice Events

Notice events are used by TollGates to communicate issues, warnings, and informational messages to customers. These events provide structured error reporting and debugging information.

```json
{
    "kind": 21023,
    "pubkey": "<tollgate_pubkey>",
    "tags": [
        ["p", "customer_pubkey"], // Optional - only when addressing specific customer
        ["level", "<error|warning|info|debug>"],
        ["code", "<text-code>"]
    ],
    "content": "Human-readable message",
    // ...
}
```

### Tags

#### Required Tags
- `level`: Severity level of the notice
  - `error`: Critical errors preventing operation
  - `warning`: Non-critical issues that may affect experience
  - `info`: Informational messages
  - `debug`: Debug information for troubleshooting, should not be used in production settings.

- `code`: Machine-readable error code for programmatic handling. Examples:
  - `session-error`: Session creation or management error
  - `upstream-error-not-connected`: TollGate has no upstream connection
  - `payment-error-mint-not-accepted`: Payment from unsupported mint
  - `payment-error-token-spent`: Cashu token already spent

#### Optional Tags
- `p`: Customer pubkey when addressing a specific customer. Omitted for general notices.

### Content
Human-readable message describing the issue or information. Should be user-friendly and provide actionable guidance when possible.

### Examples

#### No Upstream Error
```json
{
    "kind": 21023,
    "pubkey": "24d6...3662",
    "tags": [
        ["level", "error"],
        ["code", "upstream-error-not-connected"]
    ],
    "content": "TollGate Currently has no upstream connection.",
    // ...
}
```

#### Payment Error
```json
{
    "kind": 21023,
    "pubkey": "24d6...3662",
    "tags": [
        ["p", "63gy...9xvq"],
        ["level", "error"],
        ["code", "payment-error-token-spent"]
    ],
    "content": "Payment processing failed: Token already spent",
    // ...
}
```