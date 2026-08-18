# 4-Cell ESP32 Battery Management System (BMS)

![ESP32](https://img.shields.io/badge/MCU-ESP32-blue)
![Simulation](https://img.shields.io/badge/Simulation-Wokwi-orange)
![Cloud](https://img.shields.io/badge/Cloud-Blynk-20C997)
![Language](https://img.shields.io/badge/Language-Embedded%20C%2B%2B-informational)

## Project Overview

A simulated 4-cell Battery Management System built around an ESP32, Wokwi and Blynk IoT. The system monitors four cell voltages, calculates battery analytics, detects under-voltage and over-voltage conditions, and activates a debounced safety-protection response.

The project includes a local I2C LCD HMI and a cloud dashboard for real-time monitoring, fault indication and analytics.

## Key Features

- Four-cell voltage monitoring
- Minimum, maximum and average cell voltage
- Cell spread / imbalance calculation
- Battery health classification
- Under-voltage protection (UVP)
- Over-voltage protection (OVP)
- Debounced trip and recovery
- Relay cutoff during fault conditions
- Buzzer and fault LED indication
- I2C 16×2 LCD diagnostic HMI
- Blynk cloud telemetry
- Fault counter/history
- Wi-Fi RSSI monitoring
- Cloud synchronization counter
- Voltage trend visualization
- Uptime monitoring

## Protection Thresholds

| Condition | Threshold | Response |
|---|---:|---|
| Under-voltage | `< 1.5 V` | UV fault |
| Healthy | `1.5–2.5 V` | Normal |
| Over-voltage | `> 2.5 V` | OV fault |

### Fault response

```text
Cell voltage outside safe range
            ↓
     Fault detected
            ↓
    Debounce / confirm
            ↓
       Protection
            ↓
 Relay OPEN + buzzer + fault LED
            ↓
 Blynk fault / risk status update
```

## Hardware / Simulation

| Component | Connection |
|---|---|
| ESP32 DevKit C | Main controller |
| Potentiometer 1 | GPIO34 — Cell 1 |
| Potentiometer 2 | GPIO35 — Cell 2 |
| Potentiometer 3 | GPIO32 — Cell 3 |
| Potentiometer 4 | GPIO33 — Cell 4 |
| I2C LCD 16×2 | SDA GPIO21, SCL GPIO22 |
| Relay module | GPIO26 |
| Buzzer | GPIO25 |
| Fault LED | GPIO27 |

## Architecture

![Architecture](docs/architecture.svg)

## Workflow

![Workflow](docs/workflow.svg)

## Blynk Datastreams

| Pin | Name | Type | Purpose |
|---|---|---|---|
| V0 | Cell 1 V | Double | Cell 1 voltage |
| V1 | Cell 2 V | Double | Cell 2 voltage |
| V2 | Cell 3 V | Double | Cell 3 voltage |
| V3 | Cell 4 V | Double | Cell 4 voltage |
| V4 | PackState | Integer | 0 Normal / 1 Warning / 2 Fault |
| V5 | RelayState | Integer | 0 Open / 1 Closed |
| V6 | Signal RSSI | Integer | Wi-Fi RSSI |
| V7 | Fault History / Counter | Integer | Fault count |
| V8 | Active Faults | String | Current fault labels |
| V9 | Risk & Action | String | Recommended action |
| V10 | Cloud Sync | Integer | Sync count |
| V11 | Min Cell | Double | Minimum cell voltage |
| V12 | Max Cell | Double | Maximum cell voltage |
| V13 | Avg Cell | Double | Average cell voltage |
| V14 | Cell Spread | Double | Max − Min |
| V15 | Uptime | Integer | Uptime in minutes |

Full datastream configuration: [`config/blynk_dashboard.json`](config/blynk_dashboard.json)

## Dashboard Screenshots

### Monitoring / normal test

![Monitoring dashboard](screenshots/dashboard_monitoring.png)

### Fault test

![Fault-test dashboard](screenshots/dashboard_fault_test.png)

## Repository Structure

```text
.
├── sketch.ino
├── diagram.json
├── config/
│   └── blynk_dashboard.json
├── docs/
│   ├── architecture.svg
│   ├── workflow.svg
│   └── demo_video_link.txt
├── screenshots/
│   ├── dashboard_monitoring.png
│   └── dashboard_fault_test.png
└── README.md
```

## Wokwi Project

**Public Wokwi link:** `PASTE_PUBLIC_WOKWI_PROJECT_LINK_HERE`

## Demo Video

**Public Loom / MP4 link:** `PASTE_LOOM_OR_MP4_LINK_HERE`

Recommended 2–3 minute demo:

1. Start the Wokwi simulation.
2. Show all four cell voltages.
3. Show the Blynk dashboard.
4. Increase one cell above 2.5 V.
5. Show the corresponding `C#:OV` fault.
6. Show PackState changing to FAULT.
7. Show RelayState changing to OPEN/cutoff.
8. Return the cell to the safe range.
9. Demonstrate recovery.

## Setup

### Blynk credentials

The public firmware intentionally contains a placeholder:

```cpp
#define BLYNK_TEMPLATE_ID "TMPL3Ttz7y5cK"
#define BLYNK_TEMPLATE_NAME "BMS Safety Monitor"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"
```

Insert your own token locally before compiling.

For Wokwi, the current firmware uses:

```cpp
const char ssid[] = "Wokwi-GUEST";
const char pass[] = "";
```

### Required libraries

- Blynk
- ESP32 Wi-Fi support
- LiquidCrystal_I2C
- Wire

## Security

**Do not publish a real Blynk Auth Token or Wi-Fi password.**

The original working firmware contained a Blynk token during development. The GitHub-ready `sketch.ino` in this package has that token redacted. If that original token has been exposed anywhere public, regenerate/revoke it in Blynk before publishing the repository.

## Testing

### Normal

Keep all cells between 1.5 V and 2.5 V.

Expected:

```text
PackState  = 0
RelayState = 1
Active Faults = OK
```

### Over-voltage

Increase any cell above 2.5 V.

Expected:

```text
C#:OV
PackState = 2
RelayState = 0
Risk & Action = Immediate Relay Cutoff - Critical OVP/UVP!
```

### Under-voltage

Decrease any cell below 1.5 V.

Expected:

```text
C#:UV
PackState = 2
RelayState = 0
```

## Limitations

This is a simulation/educational BMS prototype, not a production automotive BMS. A real implementation would require an appropriate cell-tap measurement front-end, ADC validation, current and temperature sensing, balancing circuitry, hardware protection, isolation/level shifting where required, and extensive safety testing.

## Future Improvements

- Current sensing
- Temperature monitoring
- State-of-charge estimation
- Cell balancing
- Non-volatile fault storage
- Hardware watchdog
- ADC calibration/filtering
- Real battery measurement front-end
- Secure credential storage
- Downloadable fault logs

## Author

**Abhay Pratap Singh Bhati**  
Electronics & Communication Engineering  
Embedded Systems / IoT / BMS Project

## Submission Links

- GitHub Repository: `PASTE_PUBLIC_GITHUB_REPOSITORY_LINK_HERE`
- Wokwi Project: `PASTE_PUBLIC_WOKWI_PROJECT_LINK_HERE`
- Demo Video: `PASTE_LOOM_OR_MP4_LINK_HERE`
