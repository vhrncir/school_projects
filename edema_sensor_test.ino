#include <Stepper.h>

// Stepper motor setup
const int stepsPerRevolution = 2048;
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

// Flex sensor setup
const int FLEX_PIN = A0;
const int SENSOR_POWER_PIN = 6;
const float VCC = 5.0;
const float R_DIV = 10000.0;
const float STRAIGHT_RESISTANCE = 10000.0;
const float BEND_RESISTANCE = 20000.0;

// Testing parameters
const int maxSteps = 400;      // Max range
const int extremeSteps = 550;  // Extreme bend limit
int currentPosition = 0;       // Position
const int testIterations = 15; // Number of runs

int selectedMode = 1; // 1 = All tests, 2 = Only hysteresis

void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(10);
  pinMode(FLEX_PIN, INPUT);

  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, HIGH);

  randomSeed(analogRead(A1));

  Serial.println("Select test mode via Serial Monitor:");
  Serial.println("1 - Run ALL tests (Random, Extreme, Disconnect, Straight, Hysteresis)");
  Serial.println("2 - Run ONLY Hysteresis test");
  Serial.println("Waiting for selection (defaults to 1 after 10 seconds)...");

  unsigned long startTime = millis();
  while (Serial.available() == 0) {
    if (millis() - startTime > 10000) {
      Serial.println("Timeout reached. Defaulting to Mode 1.");
      selectedMode = 1;
      break;
    }
  }

  if (Serial.available() > 0) {
    selectedMode = Serial.parseInt();
    while(Serial.available() > 0) Serial.read();
  }

  Serial.print("Selected Mode: ");
  Serial.println(selectedMode);
  Serial.println("------------------------------------------------------------------------------");
  Serial.println("Position\tOhms\tBend");
  
  delay(2000);
}

void loop() {
  if (selectedMode == 1) {
    // Random
    Serial.println("1. Random Bend Test");
    for (int i = 0; i < testIterations; i++) {
      int targetPosition = random(0, maxSteps + 1);
      int stepsToMove = targetPosition - currentPosition;
      
      myStepper.step(stepsToMove);
      currentPosition = targetPosition;
      delay(600); 

      zmeritA_Vypsat(currentPosition);
    }

    Serial.println("2. Extreme Bend Test");
    int stepsToExtreme = extremeSteps - currentPosition;
    myStepper.step(stepsToExtreme);
    currentPosition = extremeSteps;
    delay(1000);
    zmeritA_Vypsat(currentPosition);

    Serial.println("Returning to home position...");
    myStepper.step(-currentPosition);
    currentPosition = 0;
    delay(2000);

    Serial.println("3. Automated Disconnect Test");
    digitalWrite(SENSOR_POWER_PIN, LOW); 
    delay(500);
    Serial.print("Power OFF -> ");
    zmeritA_Vypsat(currentPosition);

    digitalWrite(SENSOR_POWER_PIN, HIGH); 
    delay(500);
    Serial.print("Power ON -> ");
    zmeritA_Vypsat(currentPosition);

    Serial.println("4. Fully Straight Sensor Check");
    myStepper.step(-currentPosition); 
    currentPosition = 0;
    delay(1000);
    zmeritA_Vypsat(currentPosition);

    runHysteresisTest();

    Serial.println("Full cycle finished.");
    delay(5000);

  } else if (selectedMode == 2) {
    // Hysteresis
    runHysteresisTest();

    Serial.println("Hysteresis cycle finished.");
    delay(5000);
  }
}

void runHysteresisTest() {
  Serial.println("5. Hysteresis Sweep Test");
  
  if (currentPosition != 0) {
    myStepper.step(-currentPosition);
    currentPosition = 0;
    delay(1000);
  }

  int sweepStep = 25;

  Serial.println("Sweep Up (Bending)...");
  for (int pos = 0; pos <= maxSteps; pos += sweepStep) {
    int stepsToMove = pos - currentPosition;
    myStepper.step(stepsToMove);
    currentPosition = pos;
    delay(400);
    zmeritA_Vypsat(currentPosition);
  }

  Serial.println("Sweep Down (Straightening)...");
  for (int pos = maxSteps; pos >= 0; pos -= sweepStep) {
    int stepsToMove = pos - currentPosition;
    myStepper.step(stepsToMove);
    currentPosition = pos;
    delay(400);
    zmeritA_Vypsat(currentPosition);
  }
}

void zmeritA_Vypsat(int pozice) {
  int flexADC = analogRead(FLEX_PIN);
  
  // Safety check - disconnect or short circuit detection
  if (flexADC <= 5 || flexADC >= 1018) {
    Serial.print(pozice);
    Serial.println("\tERROR: Sensor disconnected or shorted!");
    return;
  }

  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV * (VCC / flexV - 1.0);

  // Fully straight - the sensor fell off or is flat
  if (flexR <= STRAIGHT_RESISTANCE * 1.02) {
    Serial.print(pozice);
    Serial.print("\t");
    Serial.print(flexR);
    Serial.println("\tSTATUS: Fully straight (No bend)");
    return;
  }

  // Safety check - Extreme bend limit
  if (flexR > (BEND_RESISTANCE * 1.3)) {
    Serial.print(pozice);
    Serial.print("\t");
    Serial.print(flexR);
    Serial.println("\tWARNING: Extreme bend - Risk of physical damage!");
    return;
  }

  float angle = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE, 0, 90.0);

  // Report
  Serial.print(pozice);
  Serial.print("\t");
  Serial.print(flexR);
  Serial.print("\t");
  Serial.println(angle);
}