# System Architecture

This section covers the FreeRTOS implementation, task separation rationale, and core affinity decisions.

---

## Why FreeRTOS?

FreeRTOS was chosen as the real-time operating system for this project for three fundamental reasons:

**Deterministic Task Scheduling**: In monitoring applications, timing is critical. FreeRTOS provides predictable task execution with configurable priorities, ensuring sensor readings, network operations, and UI updates occur at their designated intervals without unpredictable delays.

**Priority-Based Resource Allocation**: Critical tasks (like sensor reading and alarm detection) can be assigned higher priorities than non-critical tasks (like UI updates), guaranteeing that essential operations are never starved of CPU time.

**Deferred Interrupt Processing**: The timer-based interrupt pattern used in this project defers heavy processing from ISRs to dedicated tasks, preventing interrupt latency issues while maintaining system responsiveness.

---

## FreeRTOS Task Model

The following diagram illustrates the FreeRTOS task model and their interactions:

<img src="../images/architecture/freeRTOS.png" width="800" alt="FreeRTOS Task Model">

---

## Why Task Separation?

Task separation in this system follows a deliberate architectural decision to isolate concerns and maximize reliability:

**Network Isolation**: WiFi and MQTT operations are blocking by nature. By running these on a dedicated task (and preferably a separate CPU core), network timeouts and retries cannot interfere with critical sensor sampling.

**Core Affinity**: The ESP32's dual-core architecture allows assigning network operations to Core 0 (where the ESP-IDF WiFi stack runs) and application logic to Core 1, preventing resource contention.

**Failure Domain Isolation**: If the network task encounters issues, the sensor reading and UI tasks continue operating independently. This prevents a network failure from cascading into complete system failure.

**Maintainability**: Separated tasks with clear interfaces are easier to debug, test, and modify independently without affecting other system components.

---

## Core Affinity

The diagram below shows how tasks are distributed across ESP32 cores:

<img src="../images/architecture/tasks.png" width="800" alt="Core Affinity Diagram">

### Core Assignment Rationale

- **Core 0 (Protocol Core)**: WiFi/BT stack operations (native ESP-IDF affinity)
- **Core 1 (Application Core)**: Sensor reading, UI, telemetry construction

---

## Timing Analysis

Timing diagram showing task execution and synchronization:

<img src="../images/architecture/timing_diagram.png" width="800" alt="Timing Diagram">

### Task Timing Specifications

| Task | Core | Priority | Stack | Period |
|------|------|----------|-------|--------|
| Connection Task | 0 | 1 | 4KB | Event-driven |
| Telemetry Task | 1 | 1 | 4KB | 60s configurable |
| Sensor Task | 1 | 1 | 3KB | 10s configurable |
| Timer Callbacks | - | - | - | RTC, flags |

---

## Concurrency Model

```
Timers (ISR) → Flags → Tasks (Deferred Processing)

Timer → flag_ejecutar_sensores = true
     → Notifies sensor_task
     → sensor_task executes sensors_leer_todos()
```

This pattern prevents:
- ISR blocking by slow operations
- Task starvation
- Race conditions on shared resources

---

## Related Documentation

- [System Overview](../system/README.md)
- [Hardware Specifications](../hardware/README.md)
- [Connectivity Protocol](../connectivity/README.md)

---

[← Back to Documentation](../README.md)
