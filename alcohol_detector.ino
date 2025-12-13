#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#define buzzer 5
#define fan 6
#define proximity_sensor 7
#define alcohol_sensor A0
Servo myservo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
int pos = 0;
int alcohol_value;
int check = 0;
bool prox;
unsigned long previousMillis = 0;
int screenState = 0;
void setup() {
  Serial.begin(9600);
  myservo.attach(8);
  myservo.write(53);
  pinMode(fan, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(proximity_sensor, INPUT);
  digitalWrite(fan, LOW);
  lcd.init();
  lcd.backlight();
  lcd.clear();
}
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 2500) {
    previousMillis = currentMillis;
    lcd.clear();
    if (screenState == 0) {
      lcd.setCursor(0, 0);
      lcd.print("   REAL TIME   ");
      lcd.setCursor(0, 1);
      lcd.print("DRUNK AND DRIVE");
      screenState = 1;
    } else if (screenState == 1) {
      lcd.setCursor(0, 0);
      lcd.print("  TEST SYSTEM  ");
      lcd.setCursor(0, 1);
      lcd.print("   WITH ENTRY   ");
      screenState = 2;
    } else if (screenState == 2) {
      lcd.setCursor(0, 0);
      lcd.print("  RESTRICTION  ");
      lcd.setCursor(0, 1);
      lcd.print(" USING ARDUINO ");
      screenState = 0;
     } else {
      screenState = 0;
    }
  }
  prox = !digitalRead(proximity_sensor);
  while (prox == true && check <= 50) {
    prox = !digitalRead(proximity_sensor);
    check++;
    delay(10);
  }
  if (check >= 50) {
    check = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VEHICLE DETECTED");
    Serial.println("VEHICLE DETECTED");
    delay(1500);
    lcd.setCursor(0, 1);
    lcd.print("TESTING ALCOHOL");
    Serial.println("TESTING ALCOHOL");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VALUE: ");
    lcd.setCursor(0, 1);
    lcd.print("TIME: ");
    digitalWrite(fan, HIGH);
    Serial.println("Fan turned ON");
    for (int i = 0; i <= 150; i++) {
      if ((!digitalRead(proximity_sensor)) == false) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("  NO VEHICLE");
        lcd.setCursor(0, 1);
        lcd.print("   DETECTED ");
        digitalWrite(fan, LOW);
        delay(2000);
        screenState = 0;
        return;
      }
      alcohol_value = analogRead(alcohol_sensor);
      lcd.setCursor(6, 0);
      lcd.print(alcohol_value);
      if (i % 10 == 0) {
        lcd.setCursor(5, 1);
        lcd.print(i / 10);
        digitalWrite(buzzer, HIGH);
        delay(50);
        digitalWrite(buzzer, LOW);
        delay(50);
      }
      if (alcohol_value > 500) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ALCOHOL DETECTED");
        lcd.setCursor(0, 1);
        lcd.print(" ACCESS DENIED  ");
        digitalWrite(buzzer, HIGH);
        while ((!digitalRead(proximity_sensor))) {
          delay(10);
        }
        digitalWrite(buzzer, LOW);
        digitalWrite(fan, LOW);
        screenState = 0;
        delay(2000);
        return;
      }
      delay(80);
     }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("NO ALCOHOL FOUND");
    lcd.setCursor(0, 1);
    lcd.print("ENTRY OK");
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(500);
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(500);
    Serial.println("Opening gate - Servo moving to 150");
    myservo.write(150);
    delay(500); // Give servo time to move
    while ((!digitalRead(proximity_sensor))) {
      delay(10);
    }
    digitalWrite(buzzer, LOW);
    digitalWrite(fan, LOW);
    delay(1000);
    Serial.println("Closing gate - Servo moving to 53");
    myservo.write(53);
    delay(500); // Give servo time to move
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GATE CLOSED");
    delay(2000);
    screenState = 0;
    return;
  } else {
    check = 0;
  }
  // Debug output
  if (millis() % 2000 < 100) { // Print every ~2 seconds for debugging
    Serial.print("Proximity: ");
    Serial.print(digitalRead(proximity_sensor));
    Serial.print(", Check: ");
    Serial.println(check);
  }
  delay(100);  
}