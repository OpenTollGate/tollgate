# HTTP-01 - Http server

---

Minimal setup for allowing payments on LAN networks. Default port `2121`

## POST /
The Server MUST take a http `POST` request containing a bearer asset token directly in the request body.

### Legacy payment envelope (deprecated)

Before March 2026 the payment was carried as a `kind=21000` Nostr event with the token in a `payment` tag. The bearer-token body above replaced it, but clients from that era still exist, so a Server MAY accept the legacy envelope for backwards compatibility: a JSON body whose `kind` is `21000`, with the token read from the first `["payment", "<token>"]` tag. Clients SHOULD send the raw token; the envelope is deprecated and MAY be removed by any Server.

The envelope is a container, not an authenticated event: a Server accepting it does so for shape compatibility and MUST NOT treat that acceptance as verification of the event's signature (or of any other field the raw path would not see). The bearer token itself is the only authenticated value either way — it is validated by spending it.

The TollGate derives the customer's device identifier (e.g. MAC address) from the request's network context.

If the TollGate accepts the provided payment it MUST return http `200 OK` response where the body is a `kind=1022` TollGate Session event.

If the payment is invalid, it SHOULD return a `kind=21023` Notice event with an appropriate error code, and MAY use http `402 Payment Required` or `400 Bad Request` status.

### CURL Request Example (Successful Payment):

```bash
curl -X POST http://192.168.1.1:2121/ \
  -d 'cashuB...'
```

### Response Example (Successful Payment):

```
HTTP/1.1 200 OK
Content-Type: application/json

{
  "kind": 1022,
  ...
}
```

### Response Example (Invalid Payment):

```
HTTP/1.1 400 Bad Request
Content-Type: application/json

{
  "kind": 21023,
  ...
}
```

## GET /
A `GET` request on the root endpoint MAY return http `200 OK` response where the  body is a `kind=10021` TollGate Discovery event.

### CURL Request Example:

```bash
curl -X GET http://192.168.1.1:2121/ # TollGate IP
```

### Response Example:

```
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 398

{
  "kind": 10021,
  ...
}
```