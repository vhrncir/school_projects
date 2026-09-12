#include <Servo.h>

Servo myservo;
int fotoDioda = A1;
int hodnota = 0;
int prahHodnota = 250;
int led = 3;
int buttonPin = 2;

int zasahCounter = 0;
bool blikaniZacalo = false;
int mod = 0; // 0 = neaktivní, 1 = bez pohybu, 2 = servo pohyb

unsigned long posledniStisk = 0;
bool posledniStavTlacitka = HIGH;
int pocetStisku = 0;

void setup() {
  digitalWrite(led, HIGH);
  myservo.attach(9);
  randomSeed(analogRead(0));
  Serial.begin(9600);

  myservo.write(90);
  delay(1000);

  pinMode(led, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  int starthodnota = analogRead(fotoDioda);
  prahHodnota = starthodnota + 150; // přidání prahu nad okolní světlo
  Serial.print("Kalibrovaná prahová hodnota: ");
  Serial.println(prahHodnota);
  delay(5000);
  digitalWrite(led, LOW);
  
}

void loop() {
  // Čtení tlačítka
  bool stavTlacitka = digitalRead(buttonPin);
  
  // Výpis při změně stavu
  //if (stavTlacitka = posledniStavTlacitka) {
  if (stavTlacitka == LOW) {
    Serial.println("Tlačítko STISKNUTO");
  } else {
    Serial.println("Tlačítko PUŠTĚNO");
  }
  //}

  if (stavTlacitka == LOW && posledniStavTlacitka == HIGH && millis() - posledniStisk > 200) {
    posledniStisk = millis();

    if (blikaniZacalo) {
      // Reset po blikání
      zasahCounter = 0;
      blikaniZacalo = false;
      mod = 0;
      pocetStisku = 0;
      Serial.println("Systém restartován.");
    } else {
      pocetStisku++;
      if (pocetStisku == 1) {
        mod = 1; // Mód bez pohybu
        Serial.println("Mód 1: Bez pohybu, měření zásahů.");
      } else if (pocetStisku == 2) {
        mod = 2; // Mód s pohybem
        Serial.println("Mód 2: Servo se pohybuje.");
      }
    }
  }
  posledniStavTlacitka = stavTlacitka;

  if (mod > 0 && !blikaniZacalo) {
    if (mod == 2) {
      int nahodnyUhel = random(0, 181);
      myservo.write(nahodnyUhel);
    }

    hodnota = analogRead(fotoDioda);
    Serial.print("Hodnota fotodiody: ");
    Serial.println(hodnota);

    if (hodnota > prahHodnota) {
      zasahCounter++;
      Serial.print("Zásah #: ");
      Serial.println(zasahCounter);
      digitalWrite(led, HIGH);
      delay(200);
      digitalWrite(led, LOW);
    }

    if (zasahCounter >= 5) {
      blikaniZacalo = true;
      Serial.println("Dosaženo 5 zásahů – blikání LED.");
    }

    delay(1500);
  }

  if (blikaniZacalo) {
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
  }
}