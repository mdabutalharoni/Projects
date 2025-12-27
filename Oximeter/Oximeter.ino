#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Constants
#define REPORTING_PERIOD_MS 1000
#define DHTPIN D4           // Pin connected to DHT sensor
#define DHTTYPE DHT11       // Type of DHT sensor (DHT11)

// Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL6cYL87qOJ"
#define BLYNK_TEMPLATE_NAME "Smart Health Monitoring"
#define BLYNK_AUTH_TOKEN "vwrjHZpOtIh_W4LX_s3Ck58XohbP5yQ9"

// Objects
PulseOximeter pox;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change address if needed (0x27 or 0x3F)
DHT dht(DHTPIN, DHTTYPE);           // Initialize DHT sensor

uint32_t tsLastReport = 0;

// WiFi Credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Your_SSID";           // Replace with your WiFi SSID
char pass[] = "Your_PASSWORD";       // Replace with your WiFi password

// Callback for heartbeat detection
void onBeatDetected() {
  Serial.println("💓 Beat Detected!");
}

void setup() {
  Serial.begin(115200);
  Wire.begin();  // Initialize I2C

  // LCD Setup
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(1500);
  lcd.clear();

  // MAX30100 Setup
  if (!pox.begin()) {
    Serial.println("❌ Failed to initialize MAX30100");
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!");
    while (1);
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_4_4MA);  // Set LED current
  pox.setOnBeatDetectedCallback(onBeatDetected);

  // DHT Sensor Setup
  dht.begin();

  // Connect to WiFi and Blynk
  Blynk.begin(auth, ssid, pass);
  Serial.println("✅ Sensors Ready");
}

void loop() {
  pox.update();  // Update pulse oximeter data

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    // Read Pulse Oximeter values
    float bpm = pox.getHeartRate();
    float spo2 = pox.getSpO2();

    // Read DHT sensor values
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Display readings on Serial Monitor
    Serial.print("BPM: ");
    Serial.print(bpm);
    Serial.print(" | SpO2: ");
    Serial.print(spo2);
    Serial.print(" | Temp: ");
    Serial.print(temperature);
    Serial.print(" | Humidity: ");
    Serial.println(humidity);

    // Display readings on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BPM: ");
    if (bpm > 30 && bpm < 200)
      lcd.print(int(bpm));
    else
      lcd.print("--");

    lcd.setCursor(9, 0);
    lcd.print("T: ");
    lcd.print(temperature);
    lcd.write(223);  // Degree symbol
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("SpO2: ");
    if (spo2 > 70 && spo2 <= 100)
      lcd.print(int(spo2));
    else
      lcd.print("--");
    lcd.print(" %");

    lcd.setCursor(9, 1);
    lcd.print("H: ");
    lcd.print(humidity);
    lcd.print(" %");

    // Send data to Blynk
    Blynk.virtualWrite(V1, bpm);        // Virtual Pin 1 for BPM
    Blynk.virtualWrite(V2, spo2);       // Virtual Pin 2 for SpO2
    Blynk.virtualWrite(V3, temperature); // Virtual Pin 3 for Temperature
    Blynk.virtualWrite(V4, humidity);   // Virtual Pin 4 for Humidity

    tsLastReport = millis();
  }

  Blynk.run();  // Run Blynk process
}
