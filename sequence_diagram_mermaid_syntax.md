sequenceDiagram
    participant CN as Crows Nest
    participant R as Relay
    participant M as Merchant
    participant V as Valve
    
    Note over CN: Scans for WiFi APs
    Note over CN: Rotates MAC & npub
    
    CN->>R: Submit payment event
    M-->>R: Poll for new events
    
    Note over M: Redeem e-cash
    
    alt Payment Valid
        M->>R: Issue access grant event
        V-->>R: Poll for new events
        Note over V: Execute ndsctl to grant access
        V-->>CN: Internet Access Granted
    else Payment Invalid
        M->>R: Issue rejection event
        CN-->>R: Poll for new events
    end
