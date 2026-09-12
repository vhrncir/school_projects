#include <SoftwareSerial.h>

int counter = 0;  // Initialize the integer value to 0
String str;
const int FLEX_PIN = A0;
const float VCC = 5.0;
const int BUTTON_PIN = 2;
const float R_DIV = 10000.0;
const float STRAIGHT_RESISTANCE = 10000.0; // resistance when straight
const float BEND_RESISTANCE = 20000.0; // resistance at 90 deg
const int buzzerPin = 7; // pin bzučáku
bool buzzerState = false;
bool control = false;
float Limit = 0;


SoftwareSerial espSerial(0, 1);

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  espSerial.begin(9600); 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FLEX_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(buzzerPin, OUTPUT);
  Serial.println("START");
  int flexADC = analogRead(FLEX_PIN);
  float flexV = flexADC * VCC / 1023.0;
  Limit = (R_DIV * (VCC / flexV - 1.0) + 1500);
  Serial.println(Limit);
  delay(3000);
}

void startBuzzer() {
  tone(buzzerPin, 1000); // Alarm Frequency
  buzzerState = true;
}

void stopBuzzer() {
  noTone(buzzerPin); // Stop Alarm
  buzzerState = false;
}

void loop() {
  while (control == false) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);                
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(1000);

    int flexADC = analogRead(FLEX_PIN);
    float flexV = flexADC * VCC / 1023.0;
    float flexR = R_DIV * (VCC / flexV - 1.0);
    Serial.println("Resistance: " + String(flexR) + " ohms");
    float angle = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE, 0, 90.0);

    if (flexR >= Limit && control == false) { 
      Serial.println("OTOK");
      delay(10000);
      if (espSerial.available() > 0) { // Serial input Available
        String odpoved = espSerial.readStringUntil('\n');
        Serial.println(odpoved);
        control = true;
        Serial.println("Acknowledgement");
        stopBuzzer();
        break;
      } else {
        startBuzzer();
        delay(100);
        break;
      }
    } else {
      stopBuzzer();
      Serial.println("All good");
    }
    int buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == LOW) { // Button down
      stopBuzzer(); // Alarm Stop
      control == true;
    }  
  }
}
