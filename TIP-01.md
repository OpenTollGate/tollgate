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
        ["tips", "1", "2", "5", "..."]
    ]
}
```

Tags:
- `metric`: `milliseconds` and/or `bytes`. Multiple `metric` tags MAY be present, allowing a TollGate to meter and charge for multiple dimensions simultaneously (e.g. both time and data transfer).
- `step_size`: Size of one step for a given metric. When multiple metrics are advertised, each `step_size` includes the metric as its second value: `["step_size", "60000", "milliseconds"]`. When only a single metric is advertised, the metric qualifier MAY be omitted for backwards compatibility.
- `min_steps`: (Optional) positive whole number. Defines a minimum amount of steps a customer must purchase at a time.
- `tips`: List of implemented TollGate TIP's represented as numbers

### Example

#### Single metric (time only)
```json
{
    "kind": 10021,
    "pubkey": "24d6...3662",
    // ...
    "tags": [
        ["metric", "milliseconds"],
        ["step_size", "60000"], // 1 minute step size
        ["step_purchase_limits", "1", "0"], // Min 1 minute, max infinite minutes
        ["tips", "1", "2", "5", "..."]
    ]
}
```

#### Multiple metrics (time + data)
```json
{
    "kind": 10021,
    "pubkey": "24d6...3662",
    // ...
    "tags": [
        ["metric", "milliseconds"],
        ["metric", "bytes"],
        ["step_size", "60000", "milliseconds"], // 1 minute
        ["step_size", "1048576", "bytes"], // 1 MiB
        ["tips", "1", "2", "5", "..."]
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
		["allotment", "<amount>"]
		["metric", "<metric>"]
	]
}
```

Tags:
- `p`: pubkey of the TollCustomer Gate (from the Customer's Payment event)
- `device-identifier`: (hardware) identifier of the customer's device.
- `allotment`: Amount allotted to customer after payment. Each `allotment` tag includes the metric as its second value: `["allotment", "<amount>", "<metric>"]`. When only a single metric was purchased, the metric qualifier MAY be omitted.
- `metric`: The purchased metric(s). Echoes the advertisement when multiple metrics were involved.

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