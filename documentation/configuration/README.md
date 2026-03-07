# Configuration

This section describes the system configuration structure and JSON schemas.

---

## Configuration Structure

The system uses JSON files stored on the SD card for persistent configuration:

<img src="../images/configuration/config_structure.png" width="800" alt="Configuration Structure">

---

## Main Configuration File

The primary configuration file (`config.json`) contains system-level settings:

```json
{
  "system": {
    "device_id": "sonda_001",
    "name": "Primary Monitoring Node",
    "timezone": "America/Bogota",
    "telemetry_interval": 60,
    "sensor_interval": 10
  },
  "wifi": {
    "ssid": "MyWiFiNetwork",
    "password": "secret_password"
  },
  "mqtt": {
    "broker": "broker.example.com",
    "port": 8883,
    "username": "device_user",
    "password": "mqtt_password",
    "client_id": "sonda_001"
  },
  "cultivos": [
    {
      "nombre": "Albahaca V1",
      "variables": ["ph", "ec", "temp_agua", "temp_amb", "humedad"]
    }
  ],
  "selected_cultivo": "Albahaca V1"
}
```

---

## Configuration Parameters

### System Section

| Parameter | Type | Description |
|-----------|------|-------------|
| `device_id` | string | Unique device identifier |
| `name` | string | Human-readable device name |
| `timezone` | string | IANA timezone string |
| `telemetry_interval` | int | MQTT publish interval (seconds) |
| `sensor_interval` | int | Sensor reading interval (seconds) |

### WiFi Section

| Parameter | Type | Description |
|-----------|------|-------------|
| `ssid` | string | WiFi network name |
| `password` | string | WiFi password |

### MQTT Section

| Parameter | Type | Description |
|-----------|------|-------------|
| `broker` | string | MQTT broker hostname |
| `port` | int | MQTT broker port (usually 8883 for TLS) |
| `username` | string | MQTT username |
| `password` | string | MQTT password |
| `client_id` | string | MQTT client ID |

### Crops Section

| Parameter | Type | Description |
|-----------|------|-------------|
| `cultivos` | array | List of configured crops |
| `nombre` | string | Crop name |
| `variables` | array | Variables to monitor for this crop |
| `selected_cultivo` | string | Currently active crop |

---

## Configuration Storage

- **Location**: SD card root directory
- **Filename**: `config.json`
- **Fallback**: Default values compiled into firmware if SD unavailable

---

## Configuration Update Flow

1. Device requests configuration via MQTT
2. Server responds with `CONFIG` message
3. Device validates and saves to SD
4. Device reboots or applies new configuration

---

## Related Documentation

- [System Overview](../system/README.md)
- [Architecture Details](../architecture/README.md)
- [Connectivity Protocol](../connectivity/README.md)

---

[← Back to Documentation](../README.md)
