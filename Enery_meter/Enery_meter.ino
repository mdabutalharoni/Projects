#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <math.h>

// Blynk Template details
#define BLYNK_TEMPLATE_ID "TMPL6JafcChog"
#define BLYNK_TEMPLATE_NAME "Energy Meter"
#define BLYNK_AUTH_TOKEN "PUaUWPxMMHHaWS73S2ZG6B-1uYYHzgOb"

// Blynk credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "note 9";
char pass[] = "aaaaaaaa";

// Sensor pins
#define CURRENT_SENSOR_PIN 33
#define VOLTAGE_SENSOR_PIN 34

// Calibration values
const int sampleCount = 1000;
const float currentCalibration = 0.0035;;
const float voltageCalibration = 0.35;

float currentOffset = 0;
float voltageOffset = 0;

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Blynk Timer
BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // LCD Initialization
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  // ADC Resolution
  analogReadResolution(12); // 0-4095

  // Sensor Offset Calibration
  currentOffset = calibrateOffset(CURRENT_SENSOR_PIN);
  voltageOffset = calibrateOffset(VOLTAGE_SENSOR_PIN);

  // WiFi Connection
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");

  // Blynk Connection
  Blynk.begin(auth, ssid, pass);

  // Blynk Timer setup
  timer.setInterval(1000L, sendToBlynk); // every 1 sec
}

void loop() {
  Blynk.run();
  timer.run();
}

void sendToBlynk() {
  float rmsCurrent = measureRMS(CURRENT_SENSOR_PIN, currentOffset);
  float rmsVoltage = measureRMS(VOLTAGE_SENSOR_PIN, voltageOffset);
  float power = rmsVoltage * rmsCurrent;

  // Serial Monitor
  Serial.print("I: ");
  Serial.print(rmsCurrent, 2);
  Serial.print(" A  V: ");
  Serial.print(rmsVoltage, 2);
  Serial.print(" V  P: ");
  Serial.print(power, 2);
  Serial.println(" W");

  // Send to Blynk
  Blynk.virtualWrite(V0, rmsVoltage);
  Blynk.virtualWrite(V1, rmsCurrent);
  Blynk.virtualWrite(V2, power);

  // LCD Display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("I:");
  lcd.print(rmsCurrent, 2);
  lcd.print("A V:");
  lcd.print(rmsVoltage, 2);
  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(power, 2);
  lcd.print("W");
}

// --- RMS Calculation ---
float measureRMS(int pin, float offset) {
  long total = 0;
  for (int i = 0; i < sampleCount; i++) {
    float val = analogRead(pin) - offset;
    total += val * val;
    delayMicroseconds(100);
  }
  float rms = sqrt((float)total / sampleCount);
  return (pin == CURRENT_SENSOR_PIN) ? rms * currentCalibration : rms * voltageCalibration;
}

// --- Offset Calibration ---
float calibrateOffset(int pin) {
  long total = 0;
  for (int i = 0; i < 500; i++) {
    total += analogRead(pin);
    delayMicroseconds(100);
  }
  return total / 500.0;
}
