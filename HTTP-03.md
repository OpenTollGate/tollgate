# HTTP-03 - Usage, balance and Lightning-invoice endpoints

---

## GET /usage
A `GET` request on the `/usage` endpoint MUST return http status `200 OK` with the body containing the customer's current usage and allotment.

The TollGate derives the customer's device identifier (e.g. MAC address) from the request's network context.

Formatted as `[usage]/[allotment]`

Both values use the metric unit defined in the customer's active session (e.g. bytes or milliseconds).

If the customer has no active session, the response MUST be `-1/-1`.

http port MUST be the same as [HTTP-01](./HTTP-01.md)

### CURL Request Example:

```bash
curl -X GET http://192.168.1.1:2121/usage # TollGate IP
```

### Response Example (Active session, bytes metric):

```
HTTP/1.1 200 OK
Content-Type: text/plain

5242880/44040192
```

### Response Example (No active session):

```
HTTP/1.1 200 OK
Content-Type: text/plain

-1/-1
```

## GET /balance
A `GET` request on the `/balance` endpoint MUST return http status `200 OK` with a JSON body describing the requesting device's current session.

The TollGate derives the customer's device identifier (e.g. MAC address) from the request's network context — the same resolution as [HTTP-01](./HTTP-01.md) and `/usage`. A request whose device cannot be identified MUST still answer `200 OK` with `session_active: false` (the balance surface stays usable when a lease lookup fails), not an error.

Port and discovery are as [HTTP-01](./HTTP-01.md) (`:2121` by default).

### Response fields
- `status`: `1` on a successful lookup, `0` on a server-side failure (with `error`).
- `session_active`: whether the device currently holds allotment.
- `metric`, `usage`, `allotment`, `remaining`, `start_time`: present when a session is active. `usage`/`allotment`/`remaining` are in the session's metric unit (bytes or milliseconds); `remaining` is `allotment - usage`, floored at 0.
- `mac`: the device this balance describes, as resolved from the socket (additive; a client that ignores it parses the body it always did).

### CURL Request Example:

```bash
curl -X GET http://192.168.1.1:2121/balance
```

### Response Example (Active session, bytes metric):

```
HTTP/1.1 200 OK
Content-Type: application/json

{
  "status": 1,
  "session_active": true,
  "metric": "bytes",
  "usage": 5242880,
  "allotment": 44040192,
  "remaining": 38797312,
  "start_time": 1788179627,
  "mac": "aa:bb:cc:dd:ee:ff"
}
```

### Response Example (No active session):

```
HTTP/1.1 200 OK
Content-Type: application/json

{
  "status": 1,
  "session_active": false
}
```

## POST /ln-invoice
A `POST` request on the `/ln-invoice` endpoint asks the TollGate to create a Lightning invoice the customer can pay in order to buy access without first holding a bearer asset. The request body is JSON:

```json
{
  "amount": 210,
  "mint_url": "https://mint.domain.net"
}
```

- `amount`: required, the invoice amount in the mint's unit (sats).
- `mint_url`: required, the mint that will mint the purchased ecash.

The device identity is resolved from the request's network context; any `mac` field in the body is accepted for wire compatibility and ignored.

On success the TollGate MUST return `200 OK` with a JSON body carrying at least:

```json
{
  "status": 1,
  "quote": "<opaque quote id>",
  "invoice": "lnbc...",
  "mint_url": "https://mint.domain.net",
  "amount": 210,
  "state": "UNPAID"
}
```

`state` uses the Cashu NUT-04 quote states (`UNPAID`, `PAID`, `ISSUED`, `PENDING`, `UNKNOWN`).

Because quote creation mutates durable state and contacts a mint, a TollGate MAY refuse it with http `429 Too Many Requests` (a `Retry-After` header, and a JSON body with `status: 0`, a machine-readable `code` and a mirrored `retry_after`) — e.g. when its unpaid-invoice table is full or the mint is rate-limiting it. Malformed or incomplete requests are answered `400 Bad Request`; oversized bodies `413`.

## GET /ln-invoice
The customer polls the state of a quote created above:

```bash
curl "http://192.168.1.1:2121/ln-invoice?quote=<quote id>"
```

A quote MUST only be readable by the device that created it (identity resolved from the network context, bound at creation time). The response repeats the creation shape with an updated `state`, plus once payment has been applied:

- `access_granted`: `true` when the purchased allotment has been applied to the device;
- `allotment` and `metric`: what was granted.

A quote that settles but whose access could not yet be applied MUST NOT be reported as a failure to pay: the TollGate answers `503 Service Unavailable` with a machine-readable `code` and a message instructing the customer not to pay again, because the grant is retried server-side.

### Response Example (Paid, access applied):

```
HTTP/1.1 200 OK
Content-Type: application/json

{
  "status": 1,
  "quote": "<quote id>",
  "state": "PAID",
  "access_granted": true,
  "allotment": 44040192,
  "metric": "bytes"
}
```
