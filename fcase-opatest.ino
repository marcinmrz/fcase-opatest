/* M.Rz. 06.2025
   Prototyp testera przezroczystości czarnej osłonki
*/
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define sensorPin A0     // wejście analogowe z fototranzystora
#define RefValuePin A1   // wejście analogowe z potencjometru
#define ledRED 10
#define ledGREEN 11   
#define ledBLUE 12   
#define sensorLight 5    // pin na LEDa oświetlającego materiał
#define SerieLenght 100  // długość serii pomiarowej do uśrednienia odczytu
#define RSTAB 10        // czas [msec] stabilizacji prądu fototranzystora (dobrać eksperymentalnie)

int opacityValue = 0;    // wynik pomiaru
int ReferenceValue = 80; // wartość graniczna
int calibLo = 0;
int calibHi = 1024;
LiquidCrystal_I2C lcd(0x27,  16, 2);

void setup() {
  pinMode(RefValuePin, INPUT); pinMode(sensorPin, INPUT);
  pinMode(ledRED, OUTPUT); pinMode(ledGREEN, OUTPUT); pinMode(ledBLUE, OUTPUT);
  digitalWrite(ledRED, LOW); digitalWrite(ledGREEN, LOW); digitalWrite(ledBLUE, HIGH);
  pinMode(sensorLight, OUTPUT); digitalWrite(sensorLight, HIGH);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0); lcd.print("Autokalibracja...");
  sensorCalibrate(SerieLenght);
  Serial.begin(14400);
  delay(2000); 
  digitalWrite(ledBLUE, LOW);
  digitalWrite(ledGREEN, HIGH);
}

int sensorCalibrate(int serie) {
 int summary = 0;
 int average = 0;
 
 digitalWrite(sensorLight, LOW);
 delay (RSTAB);
 for (int i=0; i<serie; i++) {
   summary = summary + analogRead(sensorPin);
   delay(50);
 }
 calibHi = int ( summary/serie );
 
 digitalWrite(sensorLight, HIGH);
 delay (RSTAB);
 for (int i=0; i<serie; i++) {
   summary = summary + analogRead(sensorPin); 
   delay(50);
 }
 calibLo = int ( summary/serie );
 
 Serial.println("Wartości kalibracyjne:");
 Serial.print("Górny (100% światła): "); Serial.println(calibHi);
 Serial.print("Dolny  (źródło OFF):  "); Serial.println(calibLo);
}

int CheckOpacity(int serie) {
 int summary, average;
 summary = 0;
 average = 0;
 digitalWrite(sensorLight, LOW);
 delay (RSTAB);
 for (int i=0; i<serie; i++) {
  summary = summary + analogRead(sensorPin); 
 }
 average = int ( summary/serie );
 digitalWrite(sensorLight, HIGH);
 return average;
}

void loop() {
  int val1;
  val1 = CheckOpacity(100);
  ReferenceValue = analogRead(RefValuePin);
  if (val1 > ReferenceValue) { digitalWrite(ledRED, LOW); digitalWrite(ledGREEN, HIGH); }
  else { digitalWrite(ledGREEN, LOW); digitalWrite(ledRED, HIGH);}
  //delay (10);
  Serial.print(val1); Serial.print(" "); Serial.print(ReferenceValue);
  Serial.println();
  lcd.setCursor(0,0); lcd.print("Wart.progu: "); lcd.print(ReferenceValue); lcd.print("  ");
  lcd.setCursor(0,1); lcd.print("    Odczyt: "); lcd.print(val1); lcd.print("  ");
}
