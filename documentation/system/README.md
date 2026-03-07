# System Overview

This section provides a high-level description of the IoT Multiparametric Telemetry System, including general architecture and component overview.

---

## System Architecture

The following diagram shows the general architecture of the system:

<img src="../images/system/maqueta_general.png" width="800" alt="System Overview">

---

## Components Diagram

The diagram shows the relationship between peripherals and the MCU:

<img src="../images/system/General.png" width="800" alt="System Components">

---

## System Features

- **Multi-variable acquisition**: Support for analog, digital, and RS485 Modbus sensors
- **Bidirectional telemetry**: Secure MQTT communication with TLS encryption
- **Local interface**: TFT touchscreen display for local configuration and visualization
- **Data persistence**: SD card storage for offline data logging
- **Time synchronization**: Hardware RTC with battery backup for accurate timestamping

---

## Sequence Diagrams

### Sensor Reading Sequence
<img src="../images/system/secuencia_lectura.png" width="800" alt="Sensor Reading Sequence">

### FreeRTOS Execution Model
<img src="../images/system/secuencia_sistema.png" width="800" alt="FreeRTOS Model">

### Telemetry Sequence
<img src="../images/system/secuencia_telemetria.png" width="800" alt="Telemetry Sequence">

### UI Interaction Sequence
<img src="../images/system/secuencia_UI.png" width="800" alt="UI Interaction Sequence">

---

## System Photos

Prototype physical implementation:

<img src="../images/system/foto1.jpeg" width="400" alt="Photo 1">
<img src="../images/system/foto2.jpeg" width="400" alt="Photo 2">
<img src="../images/system/foto3.jpeg" width="400" alt="Photo 3">

---

## Related Documentation

- [Architecture Details](../architecture/README.md)
- [Hardware Specifications](../hardware/README.md)
- [Connectivity Protocol](../connectivity/README.md)

---

[← Back to Documentation](../README.md)
