# HTTP-01 - Http server

---

Minimal setup for allowing payments on LAN networks. Default port `2121`

## POST /
The Server MUST take a http `POST` request containing a bearer asset token directly in the request body.

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