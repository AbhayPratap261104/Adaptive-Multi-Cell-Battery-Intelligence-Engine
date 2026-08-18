# 🔋 ESP32 4-Cell Battery Management & Safety Monitoring System
![ESP32](https://img.shields.io/badge/MCU-ESP32-blue)
![Wokwi](https://img.shields.io/badge/Simulation-Wokwi-orange)
![Blynk](https://img.shields.io/badge/IoT-Blynk-20C997)
![Language](https://img.shields.io/badge/Language-C%2B%2B-informational)
![Project](https://img.shields.io/badge/Project-Internship-success)
## 📌 Project Overview
This project is an **ESP32-based 4-cell Battery Management and Safety Monitoring System** developed and tested in **Wokwi** with a **Blynk IoT dashboard**.
The system continuously monitors four individual cell voltages, calculates pack-level battery statistics, detects abnormal cell conditions, and activates a safety response when a cell crosses the defined under-voltage or over-voltage limits.
The project combines:
- Embedded systems
- Battery monitoring
- Event-driven safety logic
- ESP32
- IoT/cloud telemetry
- Blynk dashboard
- Wokwi simulation
- Local LCD-based HMI
---
## 🎯 Objectives
1. Monitor the voltage of four individual battery cells in real time.
2. Calculate minimum, maximum, and average cell voltage.
3. Calculate cell voltage spread (`maxCell - minCell`).
4. Classify the battery pack condition (`HEALTHY`, `MINOR_IMBALANCE`, `CRITICAL_IMBALANCE`, `PACK_FAILURE`).
5. Detect under-voltage (< 1.5V) and over-voltage (> 2.5V) faults.
6. Disconnect the safety relay during critical faults.
7. Provide visual and audible fault indication (LED + Buzzer beeping alarm).
8. Display diagnostic information locally on an LCD with flicker-free shadow rendering.
9. Send real-time monitoring data to Blynk.
10. Provide a cloud dashboard for monitoring and fault analysis.
---
## ⚙️ System Features
### 🔋 Cell Monitoring
