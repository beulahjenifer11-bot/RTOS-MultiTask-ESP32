# RTOS-MultiTask-ESP32
FreeRTOS multi-task sensor and alert system on ESP32 demonstrating dual-core scheduling, queues, and semaphores
# ⚡ RTOS — Multi-Task Sensor & Alert System (FreeRTOS on ESP32)

**Intern ID:** URK23EC1047
**Full Name:** Beulah Jenifer P
**No. of Weeks:** 4
**Project Name:** Real-Time Operating System (RTOS)
**Project Scope:** Dual-core multitasking system on ESP32 using FreeRTOS, demonstrating task scheduling, inter-task communication via queues, synchronization via semaphores and mutexes, and priority-based real-time alert handling — fully simulated on Wokwi

---

## 📌 Project Overview

This project demonstrates real-time embedded multitasking using FreeRTOS on the ESP32's dual-core architecture. Four independent tasks run concurrently across both cores with different priorities — a sensor reader, a display handler, an alert monitor, and a heartbeat indicator — communicating safely through queues, semaphores, and mutexes without blocking each other.

---

## 🧠 RTOS Architecture

| Task | Core | Priority | Function |
|---|---|---|---|
| SensorReader | 0 | 3 (High) | Reads DHT22 every 1s, pushes data to queue |
| AlertMonitor | 0 | 3 (High) | Waits on binary semaphore, triggers alert LED |
| DisplayHandler | 1 | 2 (Medium) | Receives queue data, prints formatted output |
| Heartbeat | 1 | 1 (Low) | Blinks status LED, reports free heap memory |

---

## 🔧 RTOS Concepts Demonstrated

- **Task Creation** — `xTaskCreatePinnedToCore()` for explicit core assignment
- **Queues** — `QueueHandle_t` for safe inter-task data transfer (SensorReader → DisplayHandler)
- **Binary Semaphore** — `SemaphoreHandle_t` for event-driven alert triggering
- **Mutex** — Protects shared Serial output from race conditions across cores
- **Task Priorities** — Higher priority tasks (SensorReader, AlertMonitor) preempt lower priority tasks
- **Dual-Core Scheduling** — Core 0 handles sensing/alerts, Core 1 handles display/heartbeat

---

## 🛠️ Tech Stack

| Component | Detail |
|---|---|
| Microcontroller | ESP32 DevKit C V4 (Dual-Core) |
| RTOS | FreeRTOS (built into ESP32 Arduino core) |
| Temperature Sensor | DHT22 |
| Alert Indicator | Red LED |
| Heartbeat Indicator | Green LED |
| Simulation | Wokwi |
| Language | C++ (Arduino Framework) |

---

## 🔌 Circuit Connections

| Component | ESP32 Pin |
|---|---|
| DHT22 VCC | 3V3 |
| DHT22 GND | GND |
| DHT22 DATA | GPIO 4 |
| Alert LED (Red) | GPIO 2 |
| Heartbeat LED (Green) | GPIO 15 |

---

## 🚨 Alert Logic Flow

SensorReader Task → reads DHT22 → checks thresholds

↓ (if exceeded)

gives Semaphore

↓

AlertMonitor Task → wakes up → activates Alert LED → logs alert

---

## 🎯 Key Results

| Metric | Value |
|---|---|
| Total RTOS Tasks | 4 |
| CPU Cores Utilized | 2 (Core 0 & Core 1) |
| Sensor Read Interval | 1 second |
| Heartbeat Interval | 1 second (500ms on/off) |
| Synchronization Primitives | Queue, Binary Semaphore, Mutex |
| Simulation | Wokwi — 100% online |

---

*Developed as part of CodSoft Embedded Systems Internship*
