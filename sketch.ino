#define BLYNK_TEMPLATE_ID "TMPL3Ttz7y5cK"
#define BLYNK_TEMPLATE_NAME "BMS Safety Monitor"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

void readSensors();
void analyzeBattery();
void protectionKernel();
void runtimeSystem();
void updateBuzzer();
void updateHMI();
void cloudTelemetry();
void executeProtection();
void recoverProtection();
void logFault(String msg, String label);

// --- Configuration & Pin Mapping ---
const char ssid[] = "Wokwi-GUEST";
const char pass[] = "";

const int PIN_CELL1 = 34;
const int PIN_CELL2 = 35;
const int PIN_CELL3 = 32;
const int PIN_CELL4 = 33;
const int PIN_BUZZER = 25;
const int PIN_RELAY = 26;
const int PIN_FAULT_LED = 27;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Thresholds & Constants ---
const float V_HEALTHY_MAX = 2.5;
const float V_HEALTHY_MIN = 1.5;
const float OVP_TRIP = 2.5;
const float UVP_TRIP = 1.5;
const float SLEW_RATE_LIMIT = 0.5; // Max allowed change per sec
const float DELTA_TELEMETRY = 0.05;

// --- Enums ---
enum HealthState { HEALTHY, MINOR_IMBALANCE, CRITICAL_IMBALANCE, PACK_FAILURE };
enum SystemMode { SYSTEM_NORMAL, SYSTEM_DEGRADED, SYSTEM_FAILSAFE, SYSTEM_SHUTDOWN };

// --- Data Structures ---
struct BatteryData {
  float cell[4];
  float prevCell[4];
  float packAvg;
  float minCell;
  float maxCell;
  int minCellIdx;
  int maxCellIdx;
  float imbalancePct;
  HealthState health;
};

struct FaultLog {
  uint32_t timestamp;
  String message;
};

// --- Globals ---
BatteryData bat;
BatteryData lastCloudBat;
SystemMode currentMode = SYSTEM_NORMAL;
bool relayState = true;
bool actualRelayState = true;
bool buzzerState = false;

uint32_t lastSensorRead = 0;
uint32_t lastRotationUpdate = 0;
uint32_t lastLcdRender = 0;
uint32_t lastCloudUpdate = 0;
uint32_t lastHeartbeat = 0;
uint32_t tripStartTime = 0;
uint32_t clearStartTime = 0;

bool inFaultCondition = false;
bool tripDebouncing = false;
bool clearDebouncing = false;

int currentScreen = 0;
String lcdShadow[2] = {"", ""};

const int MAX_FAULTS = 10;
FaultLog faultHistory[MAX_FAULTS];
int faultIndex = 0;
int totalFaults = 0;
int queuedSent = 0;
String activeFaultLabel = "OK";

// --- Setup ---
void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_FAULT_LED, OUTPUT);
  digitalWrite(PIN_RELAY, HIGH); // Active High for relay ON
  
  lcd.init();
  lcd.backlight();
  lcdShadow[0] = "BMS Init...";
  lcd.print(lcdShadow[0]);
  
  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);
  
  // Init data
  for(int i=0; i<4; i++) {
    bat.cell[i] = 0.0;
    bat.prevCell[i] = 0.0;
  }
}

// --- Task 1: Sensor & Analytics ---
void readSensors() {
  if (millis() - lastSensorRead >= 20) {
    lastSensorRead = millis();
    
    bat.minCell = 99.0;
    bat.maxCell = -99.0;
    bat.minCellIdx = 1;
    bat.maxCellIdx = 1;
    float sum = 0;
    bool adcFreeze = true;
    
    for(int i=0; i<4; i++) {
      int pin = (i==0)?PIN_CELL1:(i==1)?PIN_CELL2:(i==2)?PIN_CELL3:PIN_CELL4;
      float v = analogRead(pin) * (3.3 / 4095.0); // 3.3V reference
      
      bat.prevCell[i] = bat.cell[i];
      bat.cell[i] = v;
      sum += v;
      
      if (v < bat.minCell) {
        bat.minCell = v;
        bat.minCellIdx = i + 1;
      }
      if (v > bat.maxCell) {
        bat.maxCell = v;
        bat.maxCellIdx = i + 1;
      }
      if (abs(bat.cell[i] - bat.prevCell[i]) > 0.01) adcFreeze = false;
    }
    
    bat.packAvg = sum / 4.0;
    if (bat.maxCell > 0) {
      bat.imbalancePct = ((bat.maxCell - bat.minCell) / bat.maxCell) * 100.0;
    } else {
      bat.imbalancePct = 0;
    }
    
    // Health Check
    if (bat.minCell < UVP_TRIP || bat.maxCell > OVP_TRIP) {
      bat.health = PACK_FAILURE;
    } else {
      bat.health = HEALTHY;
    }
    
    // Check ADC Freeze (Disabled for Wokwi simulation compatibility)
    // if (adcFreeze && millis() > 5000) {
    //   logFault("ADC Freeze Detected", "SENS");
    // }
  }
}

// --- Task 2: Protection Kernel ---
void protectionKernel() {
  bool faultDetected = false;
  
  // Checks
  if (bat.maxCell > OVP_TRIP || bat.minCell < UVP_TRIP) {
    faultDetected = true;
  }
  
  // Slew-rate protection disabled for Wokwi potentiometer simulation.
  // Enable later for real hardware after proper sensor filtering.
  /*
  for(int i=0; i<4; i++) {
    float slew = abs(bat.cell[i] - bat.prevCell[i]) / 0.1; // 100ms interval
    if (slew > SLEW_RATE_LIMIT) faultDetected = true;
  }
  */

  // Asymmetric Debounce
  if (faultDetected && !inFaultCondition) {
    if (!tripDebouncing) {
      tripDebouncing = true;
      tripStartTime = millis();
    } else if (millis() - tripStartTime >= 200) {
      inFaultCondition = true;
      tripDebouncing = false;
      executeProtection();
    }
  } else if (!faultDetected && inFaultCondition) {
    if (!clearDebouncing) {
      clearDebouncing = true;
      clearStartTime = millis();
    } else if (millis() - clearStartTime >= 2000) {
      inFaultCondition = false;
      clearDebouncing = false;
      recoverProtection();
    }
  } else {
    tripDebouncing = false;
    clearDebouncing = false;
  }

  // Real-time dynamic fault-label generator across all 4 cells.
  // Keep the active fault visible while protection/recovery is in progress.
  String faultStr = "";
  for (int i = 0; i < 4; i++) {
    if (bat.cell[i] > OVP_TRIP) {
      faultStr += "C" + String(i + 1) + ":OV ";
    }
    if (bat.cell[i] < UVP_TRIP) {
      faultStr += "C" + String(i + 1) + ":UV ";
    }
  }

  faultStr.trim();

  if (faultStr.length() > 0) {
    // A real fault is present: show the current fault(s).
    if (activeFaultLabel != faultStr) {
      activeFaultLabel = faultStr;
      if (Blynk.connected()) {
        Blynk.virtualWrite(V8, activeFaultLabel);
      }
    }
  } else if (!inFaultCondition && !clearDebouncing && !tripDebouncing) {
    // Only show OK after protection has fully recovered.
    if (activeFaultLabel != "OK") {
      activeFaultLabel = "OK";
      if (Blynk.connected()) {
        Blynk.virtualWrite(V8, activeFaultLabel);
      }
    }
  }
}

void executeProtection() {
  relayState = false;
  digitalWrite(PIN_RELAY, LOW);
  buzzerState = true;
  digitalWrite(PIN_FAULT_LED, HIGH);
  logFault("Protection Tripped", activeFaultLabel);
}

void recoverProtection() {
  relayState = true;
  digitalWrite(PIN_RELAY, HIGH);
  buzzerState = false;
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_FAULT_LED, LOW);
  activeFaultLabel = "OK";
  currentMode = SYSTEM_NORMAL;
  Serial.println("Protection Cleared");
  if (Blynk.connected()) {
    Blynk.virtualWrite(V4, 0); // PackState: NORMAL
    Blynk.virtualWrite(V5, 1); // RelayState: CLOSED
    Blynk.virtualWrite(V8, "OK");
    Blynk.virtualWrite(V9, "All systems nominal");
  }
}

// --- Task 4: Runtime System ---
void runtimeSystem() {
  actualRelayState = digitalRead(PIN_RELAY);
  if (actualRelayState != relayState) {
    logFault("Relay Mismatch!", "HW");
    currentMode = SYSTEM_FAILSAFE;
  }
  
  if (!inFaultCondition && bat.minCell >= UVP_TRIP && bat.maxCell <= OVP_TRIP) {
    bat.health = HEALTHY;
  }

  if (bat.health == PACK_FAILURE || inFaultCondition) {
    currentMode = SYSTEM_SHUTDOWN;
  } else if (bat.health == CRITICAL_IMBALANCE || bat.health == MINOR_IMBALANCE) {
    currentMode = SYSTEM_DEGRADED;
  } else {
    currentMode = SYSTEM_NORMAL;
  }
}

void updateBuzzer() {
  static uint32_t lastBuzzerToggle = 0;
  static bool buzzerActive = false;
  
  if (inFaultCondition || currentMode == SYSTEM_SHUTDOWN || currentMode == SYSTEM_FAILSAFE) {
    if (millis() - lastBuzzerToggle >= 200) {
      lastBuzzerToggle = millis();
      buzzerActive = !buzzerActive;
      digitalWrite(PIN_BUZZER, buzzerActive ? HIGH : LOW);
    }
  } else if (currentMode == SYSTEM_DEGRADED) {
    if (buzzerActive && (millis() - lastBuzzerToggle >= 100)) {
      lastBuzzerToggle = millis();
      buzzerActive = false;
      digitalWrite(PIN_BUZZER, LOW);
    } else if (!buzzerActive && (millis() - lastBuzzerToggle >= 900)) {
      lastBuzzerToggle = millis();
      buzzerActive = true;
      digitalWrite(PIN_BUZZER, HIGH);
    }
  } else {
    if (buzzerActive) {
      buzzerActive = false;
      digitalWrite(PIN_BUZZER, LOW);
    }
  }
}

void logFault(String msg, String label) {
  faultHistory[faultIndex].timestamp = millis();
  faultHistory[faultIndex].message = msg;
  faultIndex = (faultIndex + 1) % MAX_FAULTS;
  totalFaults++;
  activeFaultLabel = label;
  Serial.println("FAULT: " + msg);
  if (Blynk.connected()) {
    Blynk.virtualWrite(V7, totalFaults);
    Blynk.virtualWrite(V8, activeFaultLabel);
  }
}

// --- Task 3: HMI ---
void renderLine(int line, String text) {
  text += "                ";
  text = text.substring(0, 16);
  if (lcdShadow[line] != text) {
    lcd.setCursor(0, line);
    lcd.print(text);
    lcdShadow[line] = text;
  }
}

void updateHMI() {
  static bool prevFaultCondition = false;
  bool faultChanged = (inFaultCondition != prevFaultCondition);
  prevFaultCondition = inFaultCondition;

  if (faultChanged && !inFaultCondition) {
    currentScreen = 0;
    lastRotationUpdate = millis();
  }

  if (millis() - lastRotationUpdate >= 3000) {
    lastRotationUpdate = millis();
    currentScreen = (currentScreen + 1) % 4;
  }

  if (faultChanged || millis() - lastLcdRender >= 100) {
    lastLcdRender = millis();
    
    if (inFaultCondition || currentMode == SYSTEM_SHUTDOWN) {
      renderLine(0, "FAULT DETECTED!");
      renderLine(1, "RELAY OPEN      ");
      return;
    }
    
    String l0, l1;
    switch(currentScreen) {
      case 0:
        l0 = "C1:" + String(bat.cell[0],2) + " C2:" + String(bat.cell[1],2);
        l1 = "C3:" + String(bat.cell[2],2) + " C4:" + String(bat.cell[3],2);
        break;
      case 1:
        l0 = "Avg:" + String(bat.packAvg,2) + " Imb:" + String(bat.imbalancePct,1) + "%";
        l1 = "Min:C" + String(bat.minCellIdx) + " Max:C" + String(bat.maxCellIdx);
        break;
      case 2:
        l0 = "Relay:" + String(relayState?"ON":"OFF") + " Buzz:" + String((inFaultCondition || currentMode != SYSTEM_NORMAL)?"ON":"OFF");
        l1 = "Pack:" + String((bat.health == HEALTHY)?"NORMAL":((bat.health == MINOR_IMBALANCE || bat.health == CRITICAL_IMBALANCE)?"DEGRADED":"FAULT"));
        break;
      case 3:
        l0 = "Mode:" + String((currentMode == SYSTEM_NORMAL)?"NORMAL":((currentMode == SYSTEM_DEGRADED)?"DEGRADED":"SHUTDOWN"));
        l1 = "Faults:" + String(totalFaults) + " Code:" + activeFaultLabel;
        break;
    }
    renderLine(0, l0);
    renderLine(1, l1);
  }
}

// --- Task 5 & 6: Cloud Telemetry ---
void cloudTelemetry() {
  Blynk.run();

  static uint32_t lastWiFiCheck = 0;
  static uint32_t lastRssiUpdate = 0;

  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastWiFiCheck >= 10000) {
      lastWiFiCheck = millis();
      WiFi.reconnect();
    }
    return;
  }

  // Fast live telemetry: update the dashboard every 500 ms.
  // This makes cell voltages and protection status feel responsive
  // without sending data every 20 ms sensor cycle.
  if (millis() - lastCloudUpdate >= 500) {
    lastCloudUpdate = millis();

    // Live cell voltages.
    for (int i = 0; i < 4; i++) {
      Blynk.virtualWrite(V0 + i, bat.cell[i]);
      lastCloudBat.cell[i] = bat.cell[i];
    }

    // Pack state: 0=NORMAL, 1=WARNING, 2=FAULT.
    int packState = 0;
    if (currentMode == SYSTEM_SHUTDOWN || inFaultCondition) {
      packState = 2;
    } else if (currentMode == SYSTEM_DEGRADED) {
      packState = 1;
    }

    int relayInt = relayState ? 1 : 0;

    // Risk recommendation.
    String riskRec = "All systems nominal";
    if (inFaultCondition || !relayState) {
      riskRec = "Immediate Relay Cutoff - Critical OVP/UVP!";
    } else if (bat.health == MINOR_IMBALANCE || bat.health == CRITICAL_IMBALANCE) {
      riskRec = "Inspect weak cell";
    }

    Blynk.virtualWrite(V4, packState);
    Blynk.virtualWrite(V5, relayInt);
    Blynk.virtualWrite(V7, totalFaults);
    Blynk.virtualWrite(V8, activeFaultLabel);
    Blynk.virtualWrite(V9, riskRec);
    Blynk.virtualWrite(V10, queuedSent);
    Blynk.virtualWrite(V11, bat.minCell);
    Blynk.virtualWrite(V12, bat.maxCell);
    Blynk.virtualWrite(V13, bat.packAvg);
    Blynk.virtualWrite(V14, bat.maxCell - bat.minCell);
    Blynk.virtualWrite(V15, (int)(millis() / 60000));

  }

  // RSSI changes much more slowly, so update it every 5 seconds.
  if (millis() - lastRssiUpdate >= 5000) {
    lastRssiUpdate = millis();
    Blynk.virtualWrite(V6, WiFi.RSSI());
  }
}

// --- Main Loop ---
void loop() {
  readSensors();
  analyzeBattery(); // Already integrated in readSensors, but separated logically here
  protectionKernel();
  runtimeSystem();
  updateBuzzer();
  updateHMI();
  cloudTelemetry();
}

void analyzeBattery() {
  // Handled inside readSensors to avoid redundant looping
}


