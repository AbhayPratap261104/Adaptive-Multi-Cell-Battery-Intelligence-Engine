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

Four cell voltages are monitored independently (0.00V – 3.30V range):

| Cell | Blynk Datastream | ESP32 Input |
|---|---|---|
| Cell 1 | V0 | GPIO 34 |
| Cell 2 | V1 | GPIO 35 |
| Cell 3 | V2 | GPIO 32 |
| Cell 4 | V3 | GPIO 33 |

### 🛡️ Protection Logic

The project uses the following voltage classification:

| Cell Voltage | Status | Action |
|---:|---|---|
| `< 1.5 V` | Under-Voltage (UVP) | Fault / Protection Cutoff |
| `1.5–2.5 V` | Normal (HEALTHY) | Safe Operation |
| `> 2.5 V` | Over-Voltage (OVP) | Fault / Protection Cutoff |

When a critical cell fault is confirmed (via **50ms fast trip debounce**), the system executes:

```text
Fault detected (> 2.5V or < 1.5V)
      ↓
50ms Fast Trip Debounce
      ↓
Relay Cutoff (GPIO 26 -> LOW)
      ↓
Buzzer Alarm (GPIO 25) + Fault LED (GPIO 27)
      ↓
LCD Immediate Priority Override ("FAULT DETECTED!")
      ↓
Blynk Cloud Real-Time Update (V4=2, V5=0, V8="C1:OV C2:UV", V9="Immediate Relay Cutoff!")
```

When all cell voltages return to 1.5V–2.5V, a **2000ms recovery debounce** clears the fault, reconnects the relay (`V5=1`), and resets active faults to `"No Faults"`.

---

## 📊 Battery Analytics

The ESP32 calculates:
- **Minimum Cell Voltage** & Weakest Cell Index (`minCellIdx`)
- **Maximum Cell Voltage** & Strongest Cell Index (`maxCellIdx`)
- **Average Cell Voltage** (`packAvg`)
- **Cell Voltage Spread**:
```text
Cell Spread = Maximum Cell Voltage − Minimum Cell Voltage
```

These values are displayed locally on the LCD and sent continuously to Blynk.

---

## ☁️ Blynk IoT Dashboard

The Blynk dashboard provides:
- Four cell-voltage gauges (V0 – V3)
- Pack state (V4)
- Relay state (V5)
- Signal RSSI (V6)
- Fault history counter (V7)
- Active fault information (V8)
- Risk & action message (V9)
- Cloud synchronization counter (V10)
- Minimum, maximum, average cell voltage (V11 – V13)
- Cell spread (V14)
- System Uptime (V15)
- Live cell voltage trend chart

### Normal Operating State

The normal dashboard demonstrates all four cells operating inside the defined safe range (1.5V – 2.5V).

![Normal Blynk Dashboard](screenshots/blynk_normal.png)

### Active Fault State

For fault testing, cells are driven outside the safe voltage range. The dashboard updates in real time to display multi-cell fault codes (e.g. `C1:OV C2:UV`), `PackState: 2`, and `RelayState: 0`.

![Active Fault Blynk Dashboard](screenshots/blynk_active_fault.png)

> **Note:** Live Blynk access is not publicly exposed through the repository. Dashboard screenshots are provided for evaluation. A live demonstration can be provided separately if required.

---

## 📈 Blynk Datastreams

| Pin | Datastream | Type | Description |
|---|---|---|---|
| V0 | Cell 1 V | Double | Cell 1 voltage (0 - 3.3V) |
| V1 | Cell 2 V | Double | Cell 2 voltage (0 - 3.3V) |
| V2 | Cell 3 V | Double | Cell 3 voltage (0 - 3.3V) |
| V3 | Cell 4 V | Double | Cell 4 voltage (0 - 3.3V) |
| V4 | PackState | Integer | 0 = Normal, 1 = Warning, 2 = Fault |
| V5 | RelayState | Integer | 0 = Open, 1 = Closed |
| V6 | Signal RSSI | Integer | Wi-Fi signal strength (dBm) |
| V7 | Fault History | Integer | Fault counter |
| V8 | Active Faults | String | Live active fault(s) e.g. `C1:OV C2:UV` or `No Faults` |
| V9 | Risk & Action | String | Current safety action string |
| V10 | Cloud Sync | Integer | Cloud synchronization packet counter |
| V11 | Min Cell | Double | Minimum cell voltage (V) |
| V12 | Max Cell | Double | Maximum cell voltage (V) |
| V13 | Avg Cell | Double | Average cell voltage (V) |
| V14 | Cell Spread | Double | Maximum − Minimum voltage (V) |
| V15 | Uptime | Integer | System uptime in minutes |

---

## 🧩 Hardware / Wokwi Configuration

| Component | Connection / Purpose |
|---|---|
| ESP32 DevKit | Main controller |
| Potentiometer 1 | Cell 1 simulation (GPIO 34) |
| Potentiometer 2 | Cell 2 simulation (GPIO 35) |
| Potentiometer 3 | Cell 3 simulation (GPIO 32) |
| Potentiometer 4 | Cell 4 simulation (GPIO 33) |
| I2C LCD 16×2 | Local HMI (GPIO 21 SDA / GPIO 22 SCL) |
| Relay Module | Safety cutoff (GPIO 26) |
| Buzzer | Audible fault indication (GPIO 25) |
| Fault LED | Visual fault indication (GPIO 27) |

### ESP32 Pin Mapping

```text
Cell 1 ADC  → GPIO 34
Cell 2 ADC  → GPIO 35
Cell 3 ADC  → GPIO 32
Cell 4 ADC  → GPIO 33

LCD SDA     → GPIO 21
LCD SCL     → GPIO 22

Relay       → GPIO 26
Buzzer      → GPIO 25
Fault LED   → GPIO 27
```

### 🔌 Circuit Diagram & Hardware Wiring Schematic

![ESP32 BMS Circuit Diagram](circuit_diagram.jpg)

---

## 🏗️ System Architecture

![BMS System Architecture](architecture_diagram.jpg)

---

## 🔄 Operating Workflow

![BMS Operating Workflow](workflow_diagram.jpg)

---

## 🧪 Fault Testing

### Test 1 — Normal Operation

Keep all four cells between **1.5 V and 2.5 V**.

Expected result:
```text
PackState     = 0
RelayState    = 1
Active Faults = No Faults
Risk & Action = All systems nominal
```

### Test 2 — Over-Voltage (Single Cell)

Increase Cell 1 above **2.5 V** (e.g. `Cell 1 = 2.84 V`).

Expected result:
```text
Active Faults = C1:OV
PackState     = 2
RelayState    = 0
Risk & Action = Immediate Relay Cutoff - Critical OVP/UVP!
```

### Test 3 — Multi-Cell Under-Voltage & Over-Voltage

Set Cell 1 = 2.85V, Cell 2 = 0.14V, Cell 3 = 3.21V.

Expected result:
```text
Active Faults = C1:OV C2:UV C3:OV
PackState     = 2
RelayState    = 0
Risk & Action = Immediate Relay Cutoff - Critical OVP/UVP!
```

---

## 📁 Repository Structure

```text
esp32-4cell-bms-blynk/
│
├── sketch.ino
├── diagram.json
├── README.md
│
├── config/
│   └── blynk_dashboard.json
│
├── docs/
│   ├── architecture.svg
│   ├── workflow.svg
│   └── demo_video_link.txt
│
└── screenshots/
    ├── blynk_normal.png
    └── blynk_active_fault.png
```

### File Description

| File | Purpose |
|---|---|
| `sketch.ino` | Final ESP32 firmware |
| `diagram.json` | Wokwi circuit configuration |
| `blynk_dashboard.json` | Blynk datastream configuration |
| `architecture.svg` | System architecture diagram |
| `workflow.svg` | Safety workflow diagram |
| `blynk_normal.png` | Normal dashboard screenshot |
| `blynk_active_fault.png` | Active fault dashboard screenshot |
| `README.md` | Complete project documentation |

---

## ▶️ Wokwi Simulation

**Public Wokwi Project:**  
`PASTE_PUBLIC_WOKWI_LINK_HERE`

The Wokwi simulation demonstrates the complete monitoring and protection workflow using potentiometers as simulated cell-voltage inputs.

---

## 🎥 Demo Video

**Demo Video:**  
`PASTE_LOOM_OR_MP4_LINK_HERE`

### Recommended demonstration sequence

1. Start the Wokwi simulation.
2. Show the four cell voltages on the LCD and Blynk dashboard.
3. Demonstrate normal operation (`PackState: 0`, `RelayState: 1`).
4. Increase Cell 1 above 2.5 V and lower Cell 2 below 1.5 V.
5. Show active `C1:OV C2:UV` fault code on Blynk and LCD override.
6. Show PackState changing to FAULT and RelayState changing to OPEN.
7. Return the cells to the safe range (1.5V – 2.5V).
8. Demonstrate 2000ms recovery debounce and automatic relay reconnection (`No Faults`).

---

## 🔐 Security

**No Blynk Auth Token, password, or private credential should be committed to this public repository.**

The public version of the firmware uses a placeholder:
```cpp
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"
```

---

## ⚠️ Project Scope & Limitation

This project is an **educational/simulation prototype** intended for embedded-system, IoT, and battery-monitoring development.

It is **not a production automotive or commercial BMS**. A production BMS would require real cell-voltage front-ends, current sensing, temperature sensing, cell balancing, ADC calibration, hardware protection, isolation, watchdog mechanisms, and non-volatile logging.

---

## 👨‍💻 Author

**Abhay Pratap Singh Bhati**  
**Branch:** Electronics & Communication Engineering  
**Project Area:** Embedded Systems • IoT • Battery Monitoring • ESP32 • Blynk • Wokwi

---

## 🔗 Project Links

| Resource | Link |
|---|---|
| GitHub Repository | `PASTE_GITHUB_REPOSITORY_LINK_HERE` |
| Wokwi Simulation | `PASTE_PUBLIC_WOKWI_LINK_HERE` |
| Demo Video | `PASTE_LOOM_OR_MP4_LINK_HERE` |
