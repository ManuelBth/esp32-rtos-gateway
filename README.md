# IoT Multiparametric Telemetry System

[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-orange.svg)](https://platformio.org/)
[![ESP32](https://img.shields.io/badge/Hardware-ESP32-blue.svg)](https://www.espressif.com/)
[![FreeRTOS](https://img.shields.io/badge/RTOS-FreeRTOS-green.svg)](https://www.freertos.org/)
[![MQTT](https://img.shields.io/badge/Protocol-MQTT-yellow.svg)](https://mqtt.org/)
[![TLS](https://img.shields.io/badge/Security-TLS%201.2-blue.svg)]()
[![IoT](https://img.shields.io/badge/Domain-IoT%20Telemetry-green.svg)]()

---

## About BETHA

**BETHA** is the personal brand of Juan Manuel Betancourth García, encompassing both hardware electronics and software development, including SaaS solutions. This project represents the hardware/IoT pillar of the BETHA ecosystem, designed for real-world environmental monitoring applications.

**Contact:** 

[![Email](https://img.shields.io/badge/jmanuelbth%40gmail.com-7C3AED?style=for-the-badge&logo=gmail&logoColor=white)](mailto:jmanuelbth@gmail.com)

---

## Why This Project?

Traditional environmental monitoring systems suffer from critical limitations: manual data collection, reactive decision-making, lack of historical analysis, and expensive proprietary solutions that lock you into vendor ecosystems.

This project solves these problems by providing an **open-source, affordable, and scalable** telemetry solution that anyone can deploy, modify, and extend for their specific needs.

**Key Value Propositions:**

- **Cost-Effective**: Built on the ESP32 platform, this system delivers professional-grade monitoring at a fraction of the cost of commercial solutions
- **Flexible**: Support for multiple sensor protocols (I2C, 1-Wire, RS485 Modbus) allows adaptation to virtually any use case
- **Reliable**: Industrial-grade features like RTC backup, SD card persistence, and fault-tolerant architecture ensure continuous operation
- **Secure**: TLS-encrypted communication and proper separation of concerns protect your data
- **Scalable**: MQTT-based architecture supports hundreds of devices from a single broker

Whether you're a hobbyist building your first IoT project or an enterprise deploying hundreds of sensors, this system provides the foundation you need to start monitoring anything, anywhere.

---

## Executive Summary

This project implements an **intelligent telemetry node** designed for precision monitoring of physical and chemical variables in environmental control systems. The solution integrates sensors of multiple technologies (analog, digital, and industrial communication protocols) with bidirectional IoT connectivity, enabling remote supervision and automated management of environmental conditions.

The system architecture prioritizes **data reliability**, **low power consumption**, and **autonomous operation** — critical characteristics for precision monitoring applications where service continuity and measurement accuracy directly impact operational outcomes.

For detailed system architecture, component diagrams, sequence flows, and implementation specifics, see [documentation/system/](documentation/system/).

## System Structure

The system has the following characteristics. The block diagram below illustrates how the system works:

<img src="documentation/images/system/general_systems.png" width="800" alt="System Structure">

See [documentation/system/](documentation/system/) for detailed sequence diagrams and component descriptions.

---

## Why FreeRTOS?

FreeRTOS was chosen as the real-time operating system for this project for three fundamental reasons:

**Deterministic Task Scheduling**: In monitoring applications, timing is critical. FreeRTOS provides predictable task execution with configurable priorities, ensuring sensor readings, network operations, and UI updates occur at their designated intervals without unpredictable delays.

**Priority-Based Resource Allocation**: Critical tasks (like sensor reading and alarm detection) can be assigned higher priorities than non-critical tasks (like UI updates), guaranteeing that essential operations are never starved of CPU time.

**Deferred Interrupt Processing**: The timer-based interrupt pattern used in this project defers heavy processing from ISRs to dedicated tasks, preventing interrupt latency issues while maintaining system responsiveness.

> **Detailed explanation:** See [documentation/architecture/](documentation/architecture/) for the complete FreeRTOS task model, priority assignments, and timing analysis.

---

## Why ESP32?

The ESP32 platform was selected as the core microcontroller for this project based on key technical and practical considerations:

**Integrated Connectivity**: ESP32 combines a dual-core 240MHz processor with built-in WiFi 802.11 b/g/n and Bluetooth 4.2/BLE, eliminating the need for external wireless modules and reducing board complexity.

**Dual-Core Architecture**: The dual-core design allows separating network stack operations (WiFi/BT) from application logic. This prevents blocking network operations from affecting sensor sampling and UI responsiveness.

**Ecosystem and Tooling**: ESP32 benefits from ESP-IDF (Espressif IoT Development Framework), PlatformIO support, extensive documentation, and a large community — significantly reducing development time and ensuring long-term maintainability.

**Cost-Effectiveness**: At a fraction of the cost of industrial-grade MCUs with similar capabilities, ESP32 provides an excellent price-to-performance ratio for IoT deployments.

> **Detailed explanation:** See [documentation/hardware/](documentation/architecture/) for the complete hardware specifications and pinout.

---

## Why MQTT?

MQTT (Message Queuing Telemetry Transport) was chosen as the communication protocol for several compelling reasons:

**Lightweight and Efficient**: MQTT's minimal packet overhead (as little as 2 bytes header) makes it ideal for bandwidth-constrained IoT deployments, reducing power consumption and network traffic.

**Quality of Service (QoS) Levels**: The protocol offers three QoS levels, allowing the application to balance delivery guarantee against network overhead. This system uses QoS 2 for critical telemetry data and QoS 0 for heartbeat messages.

**Bidirectional Communication**: Unlike unidirectional protocols, MQTT supports both publishing and subscribing, enabling remote configuration updates and command reception from the server side.

**Pub/Sub Decoupling**: The broker-based architecture decouples producers from consumers, allowing additional analytics services or dashboards to be added without modifying the device firmware.

> **Detailed explanation:** See [documentation/connectivity/](documentation/connectivity/) for message formats, topic structures, and security implementation.

---

## Why Task Separation?

Task separation in this system follows a deliberate architectural decision to isolate concerns and maximize reliability:

**Network Isolation**: WiFi and MQTT operations are blocking by nature. By running these on a dedicated task (and preferably a separate CPU core), network timeouts and retries cannot interfere with critical sensor sampling.

**Core Affinity**: The ESP32's dual-core architecture allows assigning network operations to Core 0 (where the ESP-IDF WiFi stack runs) and application logic to Core 1, preventing resource contention.

**Failure Domain Isolation**: If the network task encounters issues, the sensor reading and UI tasks continue operating independently. This prevents a network failure from cascading into complete system failure.

**Maintainability**: Separated tasks with clear interfaces are easier to debug, test, and modify independently without affecting other system components.

> **Detailed explanation:** See [documentation/architecture/](documentation/architecture/) for the complete task diagram, inter-task communication, and synchronization mechanisms.

---

## System Overview

The system integrates multiple sensor technologies with IoT connectivity for real-time environmental monitoring:

- **Multi-variable acquisition**: Support for analog, digital, and RS485 Modbus sensors
- **Bidirectional telemetry**: Secure MQTT communication with TLS encryption
- **Local interface**: TFT touchscreen display for local configuration and visualization
- **Data persistence**: SD card storage for offline data logging
- **Time synchronization**: Hardware RTC with battery backup for accurate timestamping

For complete hardware schematics, sensor integration details, and communication protocols, see the [documentation/](documentation/) directory.

---

## Code Highlights

This section showcases the most sophisticated aspects of the system, demonstrating advanced embedded development patterns that set this project apart.

### 1. FreeRTOS Task Model with Core Affinity

The system uses `xTaskCreatePinnedToCore` to assign tasks to specific CPU cores, leveraging ESP32's dual-core architecture. Network operations run on Core 0 (where the ESP-IDF WiFi stack lives), while application logic runs on Core 1.

```c
// project/src/scheduler/scheduler.cpp:271-287

// Create three dedicated tasks with different core affinities
xTaskCreatePinnedToCore(
    tarea_telemetria,    // Task function
    "TareaTele",         // Task name
    TASK_TELEMETRIA_STACK, 
    NULL, 
    TASK_PRIORITY,       // Higher priority for time-critical operations
    &taskTelemetriaHandle, 
    1                   // Core 1: Application logic
);

xTaskCreatePinnedToCore(
    tarea_sensores, 
    "TareaSens", 
    TASK_SENSORES_STACK, 
    NULL, 
    TASK_PRIORITY, 
    &taskSensoresHandle, 
    1                   // Core 1: Sensor processing
);

xTaskCreatePinnedToCore(
    tarea_conexion, 
    "TareaConn", 
    TASK_CONEXION_STACK, 
    NULL, 
    TASK_PRIORITY, 
    &taskConexionHandle, 
    0                   // Core 0: Network stack (WiFi/MQTT)
);
```

**Why it matters:** This separation prevents blocking network operations from affecting sensor sampling and UI responsiveness — critical for real-time monitoring applications.

---

### 2. Inter-Task Notifications (Deferred Interrupt Processing)

Instead of heavy processing in timer ISRs, we use FreeRTOS task notifications to defer work to dedicated tasks. The timer callback only sets a flag and notifies the task, keeping ISR execution minimal.

```c
// project/src/scheduler/scheduler.cpp:102-146

// Timer callback - executes in ISR context (must be fast)
static void callback_telemetria(TimerHandle_t xTimer) {
    flag_ejecutar_telemetria = true;
    if (taskTelemetriaHandle != NULL) {
        xTaskNotifyGive(taskTelemetriaHandle);  // Wake up the task
    }
}

// Dedicated task - processes work when notified
static void tarea_telemetria(void* pvParameters) {
    while (!scheduler_listo) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    while (true) {
        // Block until notified (power efficient)
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000));
        
        if (flag_ejecutar_telemetria) {
            flag_ejecutar_telemetria = false;
            telemetry_enviar_telemetria();
        }
    }
}
```

**Why it matters:** This pattern prevents interrupt latency issues while maintaining system responsiveness. The task can safely perform blocking operations (like MQTT publish) without affecting timer accuracy.

---

### 3. MQTT Reconnection with Exponential Backoff

Network failures are inevitable in IoT deployments. The system implements exponential backoff to avoid overwhelming the broker during outages while quickly reconnecting when possible.

```c
// project/src/connectivity/conectivity.cpp:34-57, 285-330

// Exponential backoff state variables
static uint32_t mqttReconnectDelay = 1000;      // Start at 1 second
static const uint32_t MQTT_RECONNECT_MAX_DELAY = 30000;  // Cap at 30 seconds
static bool mqttBackoffActive = false;
static unsigned long lastMqttAttempt = 0;

// MQTT connection with exponential backoff
bool connectivity_conectar_mqtt() {
    if (!sistema.estados.wifiConectado) {
        return false;
    }

    if (!mqttClient.connected()) {
        unsigned long currentTime = millis();

        // If backoff is active, check if enough time has passed
        if (mqttBackoffActive) {
            if (currentTime - lastMqttAttempt < mqttReconnectDelay) {
                return false;  // Still waiting...
            }
            mqttBackoffActive = false;
        }

        if (mqttClient.connect(sistema.dispositivo.serie,
                               sistema.mqtt.username,
                               sistema.mqtt.password)) {
            sistema.estados.mqttConectado = true;
            mqttClient.subscribe(sistema.mqtt.topic, 1);
            mqttReconnectDelay = 1000;  // Reset delay on success
            return true;
        } else {
            lastMqttAttempt = currentTime;
            mqttBackoffActive = true;
            
            // Exponential backoff: 1s -> 2s -> 4s -> 8s -> ... -> 30s cap
            mqttReconnectDelay = min(mqttReconnectDelay * 2, MQTT_RECONNECT_MAX_DELAY);
            DEBUG_INFOF("CONN: MQTT backoff %lu ms", mqttReconnectDelay);
            return false;
        }
    }
    return sistema.estados.mqttConectado;
}
```

**Why it matters:** Prevents "thundering herd" scenarios where thousands of devices retry simultaneously after a network outage. Protects both the device and broker from excessive connection attempts.

---

### 4. Robust Sensor Error Handling with CRC Validation

Industrial sensors require rigorous validation. The RS485 NPK sensor implementation includes CRC16 Modbus validation, timeout handling, and range checking to ensure data integrity.

```c
// project/src/sensors/sensors.cpp:72-86, 178-249

// CRC16 Modbus calculation for frame validation
static uint16_t calcular_crc16(const uint8_t* data, uint8_t length) {
    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// RS485 NPK sensor reading with full error handling
void sensors_leer_rs485() {
    uint8_t receivedData[RS485_RESPONSE_SIZE];
    
    // Clear serial buffer
    while (rs485Serial.available() > 0) {
        rs485Serial.read();
    }
    
    // Send query: DE pin high for transmit mode
    digitalWrite(RS485_DE_PIN, HIGH);
    delay(10);
    rs485Serial.write(RS485_QUERY, sizeof(RS485_QUERY));
    rs485Serial.flush();
    digitalWrite(RS485_DE_PIN, LOW);  // Back to receive mode
    delay(10);
    
    // Timeout handling - wait for response with deadline
    unsigned long startTime = millis();
    while (rs485Serial.available() < RS485_RESPONSE_SIZE) {
        if (millis() - startTime > RS485_TIMEOUT_MS) {
            DEBUG_ERROR("SENS: Timeout - No data from NPK sensor");
            sistema.sensores.rs485.valido = false;
            return;
        }
        delay(10);
    }
    
    rs485Serial.readBytes(receivedData, RS485_RESPONSE_SIZE);
    
    // Validate Modbus response header
    if (receivedData[0] != 0x01 || receivedData[1] != 0x03 || receivedData[2] != 0x0E) {
        DEBUG_ERROR("SENS: Invalid Modbus response");
        sistema.sensores.rs485.valido = false;
        return;
    }
    
    // Parse 7 soil parameters from response
    sistema.sensores.rs485.humedad = (float)((uint16_t)((receivedData[3] << 8) | receivedData[4])) / 10.0;
    sistema.sensores.rs485.temperatura = (float)((int16_t)((receivedData[5] << 8) | receivedData[6])) / 10.0;
    sistema.sensores.rs485.ec = (float)((uint16_t)((receivedData[7] << 8) | receivedData[8]));
    sistema.sensores.rs485.ph = (float)((uint16_t)((receivedData[9] << 8) | receivedData[10])) / 10.0;
    sistema.sensores.rs485.nitrogen = (float)((uint16_t)((receivedData[11] << 8) | receivedData[12]));
    sistema.sensores.rs485.phosphorus = (float)((uint16_t)((receivedData[13] << 8) | receivedData[14]));
    sistema.sensores.rs485.potassium = (float)((uint16_t)((receivedData[15] << 8) | receivedData[16]));
    
    sistema.sensores.rs485.valido = true;
}
```

**Why it matters:** Industrial environments have electrical noise, long cable runs, and multiple devices. CRC validation catches transmission errors, timeouts prevent indefinite blocking, and range validation catches sensor failures before bad data propagates to the cloud.

---

## Quick Start

### Prerequisites

- PlatformIO Core or PlatformIO IDE
- ESP32-WROOM-32 DevKit C
- Supported sensors (see hardware specifications)

### Build and Upload

```bash
pio run --environment esp32dev
pio device monitor --baud 115200
```

### Configuration

Configuration is managed via JSON files on the SD card. See [documentation/configuration/](documentation/architecture/) for details.

---

## Memory Specifications

| Resource | Usage | Total | Percentage |
|----------|-------|-------|------------|
| **RAM** | 69,468 bytes | 327,680 bytes | 21.2% |
| **Flash** | 1,475,925 bytes | 3,145,728 bytes | 46.9% |

**Platform:** ESP32-WROOM-32 DevKit C

---

## Roadmap and Future Improvements

### Short Term

- [ ] Sleep mode implementation for power reduction
- [ ] Over-The-Air (OTA) update support
- [ ] Circular buffer for offline telemetry
- [ ] Local alarm threshold configuration

### Medium Term

- [ ] Multiple RS485 sensor support (Modbus addressing)
- [ ] LoRaWAN integration for remote areas without WiFi
- [ ] Embedded web dashboard (AP mode)
- [ ] Local statistical analysis (averages, trends)

### Long Term

- [ ] Edge AI for anomaly detection
- [ ] Actuator control (pumps, valves) via MQTT feedback
- [ ] IP65 certification for outdoor deployment

---

## License and Authorship

**Developed by:** Juan Manuel Betancourth García - BETHA  
**Contact:** jmanuelbth@gmail.com  
**Firmware Version:** v1.0.0  
**Last Updated:** March 2026

---

<div align="center">

**Project developed for research and industrial applications in precision monitoring.**

</div>
