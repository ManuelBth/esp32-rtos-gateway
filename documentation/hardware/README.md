# Hardware

This section covers the ESP32 selection rationale, pinout assignments, and sensor specifications.

---

## Why ESP32?

The ESP32 platform was selected as the core microcontroller for this project based on key technical and practical considerations:

**Integrated Connectivity**: ESP32 combines a dual-core 240MHz processor with built-in WiFi 802.11 b/g/n and Bluetooth 4.2/BLE, eliminating the need for external wireless modules and reducing board complexity.

**Dual-Core Architecture**: The dual-core design allows separating network stack operations (WiFi/BT) from application logic. This prevents blocking network operations from affecting sensor sampling and UI responsiveness.

**Ecosystem and Tooling**: ESP32 benefits from ESP-IDF (Espressif IoT Development Framework), PlatformIO support, extensive documentation, and a large community — significantly reducing development time and ensuring long-term maintainability.

**Cost-Effectiveness**: At a fraction of the cost of industrial-grade MCUs with similar capabilities, ESP32 provides an excellent price-to-performance ratio for IoT deployments.

---

## Hardware Overview

<img src="../images/hardware/esp32-devkitc-v4-functional-overview.png" width="800" alt="ESP32 Hardware Overview">

---

## Pinout Configuration

The following diagram shows the complete pinout assignment:

<img src="../images/hardware/pinout.png" width="800" alt="Pinout Diagram">

### Pin Assignment Table

| Pin | Function | Peripheral | Notes |
|-----|----------|------------|-------|
| GPIO21 | SDA | I2C | DHT22, RTC |
| GPIO22 | SCL | I2C | DHT22, RTC |
| GPIO4 | OneWire | DS18B20 | Water temp sensor |
| GPIO16 | RX | UART2 | RS485 NPK sensor |
| GPIO17 | TX | UART2 | RS485 NPK sensor |
| GPIO15 | CS | SPI | SD Card |
| GPIO23 | MOSI | SPI | SD Card |
| GPIO18 | SCK | SPI | SD Card |
| GPIO19 | MISO | SPI | SD Card |
| GPIO5 | CS | SPI | TFT Display |
| GPIO2 | DC | SPI | TFT Display |
| GPIO4 | RST | SPI | TFT Display |
| GPIO33 | IRQ | Interrupt | Touch controller |

---

## Sensor Wiring

The following diagram illustrates sensor connections:

<img src="../images/hardware/sensor-wiring.png" width="800" alt="Sensor Wiring Diagram">

---

## Supported Sensors

| Sensor | Protocol | Variables | Range | Accuracy |
|--------|----------|-----------|-------|----------|
| DHT22 | 1-Wire | Temperature, Humidity | -40~80°C, 0~100% | ±0.5°C, ±2% |
| DS18B20 | 1-Wire | Water Temperature | -55~125°C | ±0.5°C |
| RS485 NPK | Modbus | pH, EC, N, P, K, Temp, Humidity | See specs | ±2-5% |

---

## Memory Specifications

| Resource | Usage | Total | Percentage |
|----------|-------|-------|------------|
| RAM | 69,468 bytes | 327,680 bytes | 21.2% |
| Flash | 1,475,925 bytes | 3,145,728 bytes | 46.9% |

**Platform:** ESP32-WROOM-32 DevKit C

---

## Related Documentation

- [System Overview](../system/README.md)
- [Architecture Details](../architecture/README.md)
- [Connectivity Protocol](../connectivity/README.md)

---

[← Back to Documentation](../README.md)
