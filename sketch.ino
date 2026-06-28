#define CELL1_PIN 34
#define CELL2_PIN 35
#define CELL3_PIN 32
#define CELL4_PIN 33

#define ADC_REF 3.3
#define ADC_MAX 4095.0

float cellVoltage[4];

float packVoltage;
float avgVoltage;
float imbalancePercent;

float maxVoltage;
float minVoltage;

int strongestCell;
int weakestCell;

// ============================
// Read Cell Voltages
// ============================
void readCells() {

  int pins[4] = {
    CELL1_PIN,
    CELL2_PIN,
    CELL3_PIN,
    CELL4_PIN
  };

  packVoltage = 0;

  for (int i = 0; i < 4; i++) {

    int adc = analogRead(pins[i]);

    cellVoltage[i] =
      (adc * ADC_REF) / ADC_MAX;

    packVoltage +=
      cellVoltage[i];
  }

  avgVoltage =
    packVoltage / 4.0;
}

// ============================
// Analyze Battery Pack
// ============================
void analyzePack() {

  maxVoltage = cellVoltage[0];
  minVoltage = cellVoltage[0];

  strongestCell = 0;
  weakestCell = 0;

  for (int i = 1; i < 4; i++) {

    if (cellVoltage[i] > maxVoltage) {

      maxVoltage =
        cellVoltage[i];

      strongestCell = i;
    }

    if (cellVoltage[i] < minVoltage) {

      minVoltage =
        cellVoltage[i];

      weakestCell = i;
    }
  }

  imbalancePercent =
    ((maxVoltage - minVoltage)
      / avgVoltage) * 100.0;
}

// ============================
// Individual Cell Status
// ============================
String getCellStatus(float voltage) {

  if (voltage < 1.5)
    return "LOW BATTERY";

  if (voltage <= 2.5)
    return "HEALTHY";

  return "OVERLOAD RISK";
}

// ============================
// Pack Status
// ============================
String getPackStatus() {

  if (imbalancePercent < 5)
    return "HEALTHY";

  if (imbalancePercent < 10)
    return "MINOR IMBALANCE";

  if (imbalancePercent < 20)
    return "CRITICAL IMBALANCE";

  return "PACK FAILURE";
}

// ============================
// Setup
// ============================
void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.println("==============================================");
  Serial.println(" ADAPTIVE MULTI-CELL BATTERY ENGINE ");
  Serial.println("==============================================");
}

// ============================
// Main Loop
// ============================
void loop() {

  readCells();

  analyzePack();

  String packStatus =
    getPackStatus();

  Serial.println();
  Serial.println("==============================================");

  for (int i = 0; i < 4; i++) {

    Serial.print("Cell ");
    Serial.print(i + 1);
    Serial.print(" Voltage : ");

    Serial.print(cellVoltage[i], 3);
    Serial.print(" V (");

    Serial.print(
      getCellStatus(
        cellVoltage[i]
      )
    );

    Serial.print(")");

    if (i == strongestCell) {
      Serial.print(" [STRONGEST]");
    }

    if (i == weakestCell) {
      Serial.print(" [WEAKEST]");
    }

    Serial.println();
  }

  Serial.println();

  Serial.print("Pack Voltage   : ");
  Serial.print(packVoltage, 3);
  Serial.println(" V");

  Serial.print("Average Cell   : ");
  Serial.print(avgVoltage, 3);
  Serial.println(" V");

  Serial.println();

  Serial.print("Highest Voltage: ");
  Serial.print(maxVoltage, 3);
  Serial.println(" V");

  Serial.print("Lowest Voltage : ");
  Serial.print(minVoltage, 3);
  Serial.println(" V");

  Serial.println();

  Serial.print("Imbalance      : ");
  Serial.print(imbalancePercent, 2);
  Serial.println(" %");

  Serial.println();

  Serial.print("Battery State  : ");
  Serial.println(packStatus);

  Serial.println("==============================================");

  delay(1000);
}
