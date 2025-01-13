sequenceDiagram
    participant CN as Crows Nest
    participant R as Relay
    participant M as Merchant
    participant V as Valve
    
    Note over CN: Scans for WiFi APs
    Note over CN: Rotates MAC & npub
    
    CN->>R: Submit payment event
    R->>M: Payment event
    
    Note over M: Redeem e-cash
    
    alt Payment Valid
        M->>R: Issue access grant event
        R->>V: Access grant event
        Note over V: Execute ndsctl to grant access
    else Payment Invalid
        M->>R: Issue rejection event
        R->>CN: Rejection event
    end
