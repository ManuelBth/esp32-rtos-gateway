# Connectivity

This section covers the MQTT protocol implementation, message formats, and security considerations.

---

## Why MQTT?

MQTT (Message Queuing Telemetry Transport) was chosen as the communication protocol for several compelling reasons:

**Lightweight and Efficient**: MQTT's minimal packet overhead (as little as 2 bytes header) makes it ideal for bandwidth-constrained IoT deployments, reducing power consumption and network traffic.

**Quality of Service (QoS) Levels**: The protocol offers three QoS levels, allowing the application to balance delivery guarantee against network overhead. This system uses QoS 2 for critical telemetry data and QoS 0 for heartbeat messages.

**Bidirectional Communication**: Unlike unidirectional protocols, MQTT supports both publishing and subscribing, enabling remote configuration updates and command reception from the server side.

**Pub/Sub Decoupling**: The broker-based architecture decouples producers from consumers, allowing additional analytics services or dashboards to be added without modifying the device firmware.

---

## MQTT Communication Flow

The following diagram illustrates the MQTT communication flow:

<img src="../images/connectivity/mqtt_flow.png" width="800" alt="MQTT Flow Diagram">

---

## Message Sequence

This diagram shows the message sequence between device and server:

<img src="../images/connectivity/message_sequence.png" width="800" alt="Message Sequence">

---

## Message Types

The system uses three message types for bidirectional communication:

### 1. TELEMETRY (Sensor Data)

Periodic message with filtered sensor readings for the selected crop.

```json
{
  "type": "TELEMETRY",
  "ts": 1737659400000,
  "cultivo": "Albahaca V1",
  "readings": [
    {"var": "ph", "val": 6.5},
    {"var": "ec", "val": 1200},
    {"var": "temp_agua", "val": 22.5},
    {"var": "temp_amb", "val": 24.1},
    {"var": "humedad", "val": 65.0},
    {"var": "nitrogeno", "val": 150},
    {"var": "fosforo", "val": 45},
    {"var": "potasio", "val": 200}
  ]
}
```

**QoS:** 2 (exactly once delivery)

---

### 2. HEARTBEAT (Health Status)

Periodic message indicating device health status.

```json
{
  "type": "HEARTBEAT",
  "ts": 1737659400000,
  "fw": "v1.0.0",
  "hw": "v1.0.0",
  "bat": {"lvl": "85"},
  "wifi": {"rssi": -53},
  "heap": 44894,
  "uptime": 86460
}
```

**QoS:** 0 (at most once)

---

### 3. REQUEST_CONFIG / CONFIG (Remote Configuration)

**Request (device → server):**
```json
{"type": "REQUEST_CONFIG"}
```

**Response (server → device):**
```json
{
  "type": "CONFIG",
  "cultivos": [
    {
      "nombre": "Albahaca V1",
      "variables": [
        {"var": "ph"},
        {"var": "ec"},
        {"var": "temp_agua"},
        {"var": "temp_amb"},
        {"var": "humedad"},
        {"var": "nitrogeno"},
        {"var": "fosforo"},
        {"var": "potasio"}
      ]
    }
  ]
}
```

---

## Topic Structure

| Topic | Direction | QoS | Purpose |
|-------|-----------|-----|---------|
| `device/{id}/telemetry` | Out | 2 | Sensor data |
| `device/{id}/heartbeat` | Out | 0 | Health status |
| `device/{id}/config/response` | In | 1 | Configuration |
| `server/config/request` | In | 1 | Configuration request trigger |

---

## Security

- **TLS 1.2** encryption via WiFiClientSecure
- **Certificate validation** for broker connection
- **MQTT over TLS** (port 8883)

---

## Related Documentation

- [System Overview](../system/README.md)
- [Architecture Details](../architecture/README.md)
- [Hardware Specifications](../hardware/README.md)

---

[← Back to Documentation](../README.md)
