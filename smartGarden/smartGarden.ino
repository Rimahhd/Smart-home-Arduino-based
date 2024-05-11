#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>
#include <SoftwareSerial.h> // Include the SoftwareSerial library for Bluetooth communication

LiquidCrystal_I2C lcd(0x27, 16, 2);
int soilMoisture = A0;
int LDR = A2;
int led = 2;
int fanPin = 9; // Pin connected to the fan
int scrollCounter = 0;
int DHTPin = 7; // DHT11 sensor pin
int flamePin = 4;
int buzzerPin = 11; 
int bluetoothTX = 1; // Bluetooth module TX pin connected to Arduino RX pin
int bluetoothRX = 0; // Bluetooth module RX pin connected to Arduino TX pin
SimpleDHT11 dhtSensor;
SoftwareSerial bluetooth(bluetoothRX, bluetoothTX); // Create a SoftwareSerial object for Bluetooth communication

void setup() {
  pinMode(soilMoisture, INPUT);
  pinMode(LDR, INPUT);
  pinMode(led, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(flamePin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  bluetooth.begin(9600); // Initialize Bluetooth communication
  lcd.init();
  lcd.backlight();
}

void loop() {
  int moisture = analogRead(soilMoisture);
  int lightLevel = analogRead(LDR);
  int flameState = digitalRead(flamePin);
  
  delay(2000); // Delay before reading from DHT11 sensor
  
  byte temp = 0;
  byte hum = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dhtSensor.read(DHTPin, &temp, &hum, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("DHT11 read failed, err="); Serial.println(err);
    delay(2000);
    return;
  }
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" C, Humidity: ");
  Serial.print(hum);
  Serial.print("%, Moisture: ");
  Serial.print(moisture);
  Serial.print(", Light Level: ");
  Serial.println(lightLevel);
  
  // Check flame sensor state
  if (flameState == LOW) {
    Serial.println("Flame detected!");
    digitalWrite(buzzerPin, HIGH); // Activate buzzer if flame is detected
    bluetooth.println("Flame detected! Check immediately."); // Send Bluetooth notification for flame detection
  } else {
    digitalWrite(buzzerPin, LOW); // Turn off buzzer if no flame is detected
  }

  // Check soil moisture level
  if (moisture > 750) {
    Serial.println("Soil is dry! Water the plants immediately.");
    bluetooth.println("Soil is dry! Water the plants immediately."); // Send Bluetooth notification for dry soil
  }
  
  // Turn on the fan if temperature and humidity are high
  if (temp > 21 && hum > 33) {
    digitalWrite(fanPin, HIGH);
  } else {
    digitalWrite(fanPin, LOW);
  }
  
  scrollCounter++;
  if (scrollCounter > 2) {
    scrollCounter = 0;
  }

  lcd.clear();
  if (scrollCounter == 0) {
    lcd.setCursor(0,0);
    lcd.print("Temp: ");
    lcd.print(temp);
    lcd.print(" C");
    lcd.setCursor(0,1);
    lcd.print("Humidity: ");
    lcd.print(hum);
    lcd.print("%");
  } else if (scrollCounter == 1) {
    lcd.setCursor(0,0);
    lcd.print("Moisture: ");
    lcd.print(moisture);
    lcd.setCursor(0,1);
    lcd.print("Light Level: ");
    lcd.print(lightLevel);
  }
}