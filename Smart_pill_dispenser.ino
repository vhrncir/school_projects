#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

int pSCK = 2;
int pDT = 3;
int zero = 0;
int S = 6;
int O = 6;
int V = 2;
float lastWeight = 0;
float weight = 0;
float kalibrace = 8.875;
Servo myServo;

#define kanal_A_zesil_128 1
#define kanal_A_zesil_64 3

const int buttonPin = 10;
int buttonState = 0;
int lastButtonState = 0;
int pressCount = 0;
int lidState = 0;

const int led1 = 7;
const int led2 = 8;
const int led3 = 9;
float threshold = 0;

int counter = 0; // Časovač pro detekci stabilního stavu
int counterlim = 10; // Zpoždění v milisekundách

const int photoResistorPin = A3;  // Analogový vstup, ke kterému je připojen fotorezistor
const float knownResistor = 5000.0; // Hodnota známého rezistoru v ohmech (10 kOhm)

void setup() {
  pinMode(pSCK, OUTPUT);
  pinMode(pDT, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);
  digitalWrite(pSCK, LOW);
  myServo.attach(5);
  lcd.init();
  lcd.backlight();
  lcd.print("Stanovuji nulu:");
  myServo.write(160);
  delay(5000);
  Serial.print("Stanovuji nulu: ");
  zero = (-1 * spusteniMereni(kanal_A_zesil_64));
  Serial.print(zero);
  Serial.print("Stanovuji threshold: ");
  float sensorthr = analogRead(photoResistorPin); // Čtení hodnoty z analogového pinu
  float voltagethr = sensorthr * (5.0 / 1023.0);  // Převod ADC hodnoty na napětí
  threshold = ((knownResistor * (5.0 - voltagethr)) / voltagethr*0.95);
  Serial.print(threshold);
  delay(1000);
  lcd.clear();
  lcd.print("Pripraven");
  Serial.println("Připraven");
}

void loop() {
  if (S <= 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nedostatek prášků");
    lcd.setCursor(0, 1);
    lcd.print("pro snídani");
    delay(60000);
  } else if (O <= 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nedostatek prášků");
    lcd.setCursor(0, 1);
    lcd.print("pro oběd");
    delay(60000);
  } else if (V <= 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nedostatek prášků");
    lcd.setCursor(0, 1);
    lcd.print("pro večeři");
    delay(60000);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S:");
  lcd.print(S);
  lcd.setCursor(4, 0);
  lcd.print("O:");
  lcd.print(O);
  lcd.setCursor(0, 1);
  lcd.print("V:");
  lcd.print(V);
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && lastButtonState == HIGH) {
    pressCount++; // Zvyšení počítadla stisknutí
    if (pressCount > 4){ 
      pressCount = 0; // Cyklické přepínání mezi 1, 2, 3
    }
    // Nastavení LED podle počtu stisknutí
    if (pressCount == 1) {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      myServo.write(120);
    } else if (pressCount == 2) {
      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
      myServo.write(60);
    } else if (pressCount == 3) {
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, HIGH);
      myServo.write(10);
    } else if (pressCount == 4 || pressCount == 0) {
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      pressCount = 0;
      myServo.write(160);
    }
    delay(200); // Debounce zpoždění
  }
  delay(50);
  lastButtonState = buttonState; // Uložení aktuálního stavu tlačítka

  Serial.print("Vysledek mereni: ");
  weight = (-1 * spusteniMereni(kanal_A_zesil_64) - zero)/kalibrace;
  Serial.print(weight);
  Serial.println(" g");

  // Měření odporu na fotorezistoru
  int sensorValue = analogRead(photoResistorPin); // Čtení hodnoty z analogového pinu
  float voltage = sensorValue * (5.0 / 1023.0);  // Převod ADC hodnoty na napětí
  // Výpočet odporu fotorezistoru
  float photoResistorResistance = (knownResistor * (5.0 - voltage)) / voltage;
  delay(1000);

  // Výpis hodnot na Serial a LCD
  Serial.print("\nFotorezistor: ");
  Serial.print(photoResistorResistance);
  Serial.println(" ohmů");

  if (photoResistorResistance > threshold && lidState == 0) {
    Serial.println("closed");
  } else if (photoResistorResistance > threshold && lidState == 1) {
    delay(1000);
    if (counter > counterlim) { 
            // Stav šuplíku je stabilně zavřený
            lidState = 0;
            Serial.println("closed");
            myServo.write(160);
            delay(3000);
            if (weight < lastWeight) {
            // Snížení počtu objektů podle aktivní LED
              if (digitalRead(led1) == HIGH && S > 0) {
                S = S - round((lastWeight - weight) / 3);; // Snížení počtu objektů pro R
              } else if (digitalRead(led2) == HIGH && O > 0) {
                O = O - round((lastWeight - weight) / 3);; // Snížení počtu objektů pro O
              } else if (digitalRead(led3) == HIGH && V > 0) {
                V = V - round((lastWeight - weight) / 3);; // Snížení počtu objektů pro V
              } else if (digitalRead(led1) == LOW && digitalRead(led2) == LOW && digitalRead(led3) == LOW)  {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Vzal jsi si nesprávně prášek");
                delay (60000);
              }
              digitalWrite(led1, LOW);
              digitalWrite(led2, LOW);
              digitalWrite(led3, LOW);
              lastWeight = weight; // Aktualizace poslední změřené váhy
              lidState = 0;
            } else {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Nevzal jsi si prášek!");
                delay (60000);
            }
            
    } else {
        // Reset časovače a stav šuplíku je otevřený
        counter = 0;
        lidState = 1;
        Serial.println("open");
    }
  } else {
    Serial.println("open");
    lidState = 1;
    counter += 1;
  }
  delay(1000);
}

long spusteniMereni(byte mericiMod) {
  byte index;
  long vysledekMereni = 0L;
  
  while (digitalRead(pDT));
  for (index = 0; index < 24; index++) {
    digitalWrite(pSCK, HIGH);
    vysledekMereni = (vysledekMereni << 1) | digitalRead(pDT);
    digitalWrite(pSCK, LOW);
  }
  
  for (index = 0; index < mericiMod; index++) {
    digitalWrite(pSCK, HIGH);
    digitalWrite(pSCK, LOW);
  }
  
  if (vysledekMereni >= 0x800000) {
    vysledekMereni = vysledekMereni | 0xFF000000L;
  }
  
  vysledekMereni = vysledekMereni / 64 / 2;
  return vysledekMereni;
}
