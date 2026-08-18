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

1. Monitor the voltage of four individual battery cells.
2. Calculate minimum, maximum and average cell voltage.
3. Calculate cell voltage spread.
4. Classify the battery pack condition.
5. Detect under-voltage and over-voltage faults.
6. Disconnect the relay during critical faults.
7. Provide visual and audible fault indication.
8. Display diagnostic information locally on an LCD.
9. Send real-time monitoring data to Blynk.
10. Provide a cloud dashboard for monitoring and fault analysis.

---

## ⚙️ System Features

### 🔋 Cell Monitoring

Four cell voltages are monitored independently:

| Cell | Blynk Datastream | ESP32 Input |
|---|---|---|
| Cell 1 | V0 | GPIO 34 |
| Cell 2 | V1 | GPIO 35 |
| Cell 3 | V2 | GPIO 32 |
| Cell 4 | V3 | GPIO 33 |

### 🛡️ Protection Logic

The current project uses the following voltage classification:

| Cell Voltage | Status | Action |
|---:|---|---|
| `< 1.5 V` | Under-Voltage | Fault / Protection |
| `1.5–2.5 V` | Normal | Normal operation |
| `> 2.5 V` | Over-Voltage | Fault / Protection |

When a critical cell fault is confirmed, the system:

```text
Fault detected
      ↓
Fault confirmation / debounce
      ↓
Relay cutoff
      ↓
Buzzer + fault indication
      ↓
Blynk fault status update
```

---

## 📊 Battery Analytics

The ESP32 calculates:

- **Minimum Cell Voltage**
- **Maximum Cell Voltage**
- **Average Cell Voltage**
- **Cell Voltage Spread**

The spread is calculated as:

```text
Cell Spread = Maximum Cell Voltage − Minimum Cell Voltage
```

These values are displayed locally and sent to Blynk.

---

## ☁️ Blynk IoT Dashboard

The Blynk dashboard provides:

- Four cell-voltage gauges
- Pack state
- Relay state
- Risk & action message
- Active fault information
- Minimum cell voltage
- Maximum cell voltage
- Average cell voltage
- Cell spread
- Signal RSSI
- Fault history/counter
- Cloud synchronization counter
- Uptime
- Cell voltage trend chart

### Normal Operating State

The normal dashboard demonstrates all four cells operating inside the defined safe range.

![Normal Blynk Dashboard](screenshots/blynk_normal.png)

### Active Fault State

For fault testing, one cell is intentionally driven outside the safe voltage range. The dashboard then displays the corresponding fault, pack fault state and relay cutoff response.

![Active Fault Blynk Dashboard](screenshots/blynk_active_fault.png)

> **Note:** Live Blynk access is not publicly exposed through the repository. Dashboard screenshots are provided for evaluation. A live demonstration can be provided separately if required.

---

## 📈 Blynk Datastreams

| Pin | Datastream | Type | Description |
|---|---|---|---|
| V0 | Cell 1 V | Double | Cell 1 voltage |
| V1 | Cell 2 V | Double | Cell 2 voltage |
| V2 | Cell 3 V | Double | Cell 3 voltage |
| V3 | Cell 4 V | Double | Cell 4 voltage |
| V4 | PackState | Integer | 0 = Normal, 1 = Warning, 2 = Fault |
| V5 | RelayState | Integer | Relay state |
| V6 | Signal RSSI | Integer | Wi-Fi signal strength |
| V7 | Fault History | Integer | Fault counter |
| V8 | Active Faults | String | Current active fault(s) |
| V9 | Risk & Action | String | Current safety action |
| V10 | Cloud Sync | Integer | Cloud synchronization counter |
| V11 | Min Cell | Double | Minimum cell voltage |
| V12 | Max Cell | Double | Maximum cell voltage |
| V13 | Avg Cell | Double | Average cell voltage |
| V14 | Cell Spread | Double | Maximum − Minimum voltage |
| V15 | Uptime | Integer | System uptime in minutes |

---

## 🧩 Hardware / Wokwi Configuration

| Component | Connection / Purpose |
|---|---|
| ESP32 DevKit | Main controller |
| Potentiometer 1 | Cell 1 simulation |
| Potentiometer 2 | Cell 2 simulation |
| Potentiometer 3 | Cell 3 simulation |
| Potentiometer 4 | Cell 4 simulation |
| I2C LCD 16×2 | Local HMI |
| Relay Module | Safety cutoff |
| Buzzer | Audible fault indication |
| Fault LED | Visual fault indication |

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

---

## 🏗️ System Architecture

```text
              ┌─────────────────────┐
              │   4 Cell Inputs     │
              │ C1 C2 C3 C4 Voltage │
              └──────────┬──────────┘
                         │
                         ▼
              ┌─────────────────────┐
              │       ESP32         │
              │ ADC + Processing    │
              └──────────┬──────────┘
                         │
             ┌───────────┴───────────┐
             ▼                       ▼
   ┌──────────────────┐    ┌──────────────────┐
   │ Battery Analytics│    │ Safety Protection│
   │ Min/Max/Avg/Spread│   │ UVP / OVP / Relay│
   └─────────┬────────┘    └─────────┬────────┘
             │                       │
             ▼                       ▼
   ┌──────────────────┐    ┌──────────────────┐
   │  Blynk Dashboard  │    │ LCD / Buzzer / LED│
   └──────────────────┘    └──────────────────┘
```

---

## 🔄 Operating Workflow

```text
Start
  ↓
Initialize ESP32 peripherals
  ↓
Connect to Wi-Fi / Blynk
  ↓
Read four cell voltages
  ↓
Calculate Min / Max / Average / Spread
  ↓
Check UVP / OVP conditions
  ↓
┌─────────────────────────────┐
│ Is a critical fault present?│
└──────────────┬──────────────┘
       No      │       Yes
       ↓       │        ↓
 Normal state  │   Confirm fault
       ↓       │        ↓
 Relay normal  │   Relay cutoff
       ↓       │        ↓
 Dashboard     │   Buzzer / LED
 update        │        ↓
       └───────┴──→ Dashboard update
                         ↓
                    Repeat cycle
```

---

## 🧪 Fault Testing

### Test 1 — Normal Operation

Keep all four cells between **1.5 V and 2.5 V**.

Expected result:

```text
PackState  = 0
RelayState = 1
Active Faults = OK
Risk & Action = All systems nominal
```

### Test 2 — Over-Voltage

Increase one cell above **2.5 V**.

Example:

```text
Cell 1 = 2.84 V
```

Expected result:

```text
Active Faults = C1:OV
PackState      = 2
RelayState     = 0
Risk & Action  = Immediate Relay Cutoff
```

### Test 3 — Under-Voltage

Reduce one cell below **1.5 V**.

Expected result:

```text
Active Faults = C#:UV
PackState      = 2
RelayState     = 0
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
2. Show the four cell voltages.
3. Show the Blynk dashboard.
4. Demonstrate normal operation.
5. Increase one cell above 2.5 V.
6. Show the active `C#:OV` fault.
7. Show PackState changing to FAULT.
8. Show RelayState changing to OPEN/cutoff.
9. Return the cell to the safe range.
10. Demonstrate recovery.

---

## 🔐 Security

**No Blynk Auth Token, password, or other private credential should be committed to this public repository.**

The public version of the firmware should use a placeholder such as:

```cpp
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"
```

If a real Blynk Auth Token has previously been exposed publicly, it should be **revoked/regenerated** before final submission.

---

## ⚠️ Project Scope & Limitation

This project is an **educational/simulation prototype** intended for embedded-system, IoT and battery-monitoring development.

It is **not a production automotive or commercial BMS**.

A production battery-management system would require additional hardware and validation, including appropriate cell-voltage measurement circuitry, current sensing, temperature sensing, cell balancing, ADC calibration, hardware protection, isolation/level shifting where required, watchdog mechanisms, fault logging and extensive safety testing.

---

## 🚀 Future Improvements

- 🔋 Real battery-cell measurement front-end
- 🌡️ Cell temperature monitoring
- ⚡ Pack current measurement
- 🔄 Automatic cell balancing
- 📊 State-of-charge estimation
- ❤️ State-of-health estimation
- 💾 Non-volatile fault logging
- 🔐 Secure credential management
- 🛡️ Hardware watchdog and redundant protection
- 📱 Mobile Blynk monitoring
- 📈 Advanced battery analytics

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

---

## ⭐ Internship Project Submission

This repository contains the firmware, simulation configuration, dashboard documentation, architecture/workflow diagrams and test evidence for the **ESP32 4-Cell Battery Management & Safety Monitoring System**.
