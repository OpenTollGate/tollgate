# HTTP-03 - Usage endpoint

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
