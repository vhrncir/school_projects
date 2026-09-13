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
const int maxSteps = 400;     // Max range
const int extremeSteps = 550; // Extreme bend limit
int currentPosition = 0;      // Position
const int testIterations = 15; // Number of runs

void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(10);
  pinMode(FLEX_PIN, INPUT);

  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, HIGH);

  // Random
  randomSeed(analogRead(A1));

  Serial.println("START NÁHODNÉHO TESTOVÁNÍ: Motor vs. Flex Senzor");
  Serial.println("----------------------------------------------");
  Serial.println("Position\tOhms\tBend");
  
  delay(3000);
}

void loop() {
  // 1. Standard random testing
  Serial.println("--- PHASE 1: Standard Random Range ---");
  for (int i = 0; i < testIterations; i++) {
    int targetPosition = random(0, maxSteps + 1);
    int stepsToMove = targetPosition - currentPosition;
    
    myStepper.step(stepsToMove);
    currentPosition = targetPosition;
    delay(600); 

    zmeritA_Vypsat(currentPosition);
  }

  // 2. Extreme bend testing
  Serial.println("--- PHASE 2: Extreme Bend Test (Over-bending) ---");
  int stepsToExtreme = extremeSteps - currentPosition;
  myStepper.step(stepsToExtreme);
  currentPosition = extremeSteps;
  delay(1000);
  zmeritA_Vypsat(currentPosition);

  Serial.println("Returning to home position...");
  myStepper.step(-currentPosition);
  currentPosition = 0;
  delay(2000);

  // 3. Disconnection test
  Serial.println("--- PHASE 3: Automated Disconnect Test ---");
  
  // Cut power to simulate sensor unplugging / supply drop
  digitalWrite(SENSOR_POWER_PIN, LOW); 
  delay(500);
  Serial.print("Power OFF -> ");
  zmeritA_Vypsat(currentPosition);

  // Restore power automatically
  digitalWrite(SENSOR_POWER_PIN, HIGH); 
  delay(500);
  Serial.print("Power ON -> ");
  zmeritA_Vypsat(currentPosition);

  // 4. Fully straight sensor check (verification after stress)
  Serial.println("--- PHASE 4: Fully Straight Sensor Check ---");
  myStepper.step(-currentPosition); 
  currentPosition = 0;
  delay(1000);
  zmeritA_Vypsat(currentPosition);

  Serial.println("Cycle finished. Restarting in 5 seconds...");
  Serial.println("--------------------------------------------------");
  delay(5000);
}

void zmeritA_Vypsat(int pozice) {
  int flexADC = analogRead(FLEX_PIN);
  
  // Safety check: disconnect or short circuit detection
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

  // Safety check: Extreme bend / potential sensor damage limit
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