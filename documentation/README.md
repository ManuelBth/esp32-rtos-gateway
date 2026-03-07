# Documentation

This directory contains detailed technical documentation for the IoT Multiparametric Telemetry System.

---

## Table of Contents

### [System Overview](system/)
General system architecture, components, and high-level description.

- [Overview](system/README.md)

---

### [Architecture](architecture/)
FreeRTOS implementation, task separation, and core affinity decisions.

- [FreeRTOS Model](architecture/README.md)

---

### [Hardware](hardware/)
ESP32 selection rationale, pinout, and sensor specifications.

- [ESP32 Overview](hardware/README.md)

---

### [Connectivity](connectivity/)
MQTT protocol implementation, message formats, and security.

- [MQTT Protocol](connectivity/README.md)

---

### [Configuration](configuration/)
System configuration structure and JSON schemas.

- [Configuration Format](configuration/README.md)

---

## Quick Navigation

| Section | Description | Key Topics |
|---------|-------------|-------------|
| [System](system/) | High-level overview | Architecture, components |
| [Architecture](architecture/) | RTOS & tasks | FreeRTOS, task separation, core affinity |
| [Hardware](hardware/) | MCU & sensors | ESP32, pinout, sensors |
| [Connectivity](connectivity/) | Communication | MQTT, TLS, message formats |
| [Configuration](configuration/) | Settings | JSON config, parameters |

---

For questions or contributions, contact: jmanuelbth@gmail.com
