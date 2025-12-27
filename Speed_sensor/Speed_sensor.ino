#define BLYNK_TEMPLATE_ID "TMPL6iI2wCr7N"
#define BLYNK_DEVICE_NAME "Speed Sensor"
#define BLYNK_AUTH_TOKEN "NpmysmCNQ6GHWRRos-tXAKyG2OsTw2Xa"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials
char ssid[] = "Note9";
char pass[] = "12345679";

// Blynk timer
BlynkTimer timer;

// Sensor pins
const int startPin = 18;
const int endPin   = 19;

// Variables for timing
unsigned long startTime = 0;
unsigned long endTime = 0;
bool startDetected = false;
bool endDetected = false;

// Distance between sensors in meters (15 cm)
const float distance = 0.15;

void setup() {
  Serial.begin(115200);

  // Setup sensors
  pinMode(startPin, INPUT);
  pinMode(endPin, INPUT);

  // Connect to WiFi + Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Connected to Blynk!");
}

void loop() {
  Blynk.run();
  timer.run();

  int startSensor = digitalRead(startPin);
  int endSensor   = digitalRead(endPin);

  // Detect falling edge on start sensor (1 -> 0)
  if (startSensor == 0 && !startDetected) {
    startTime = millis();
    startDetected = true;
    endDetected = false;
    Serial.println("Start sensor triggered!");
  }

  // Detect falling edge on end sensor (1 -> 0)
  if (endSensor == 0 && startDetected && !endDetected) {
    endTime = millis();
    endDetected = true;
    Serial.println("End sensor triggered!");

    // Calculate velocity
    unsigned long deltaT = endTime - startTime; // ms
    if (deltaT > 0) {
      float velocity = distance / (deltaT / 1000.0); // m/s

      Serial.print("Time difference: ");
      Serial.print(deltaT);
      Serial.print(" ms\tVelocity: ");
      Serial.print(velocity);
      Serial.println(" m/s");

      // Send to Blynk app (V0 for velocity, V1 for time difference)
      Blynk.virtualWrite(V0, velocity);
      Blynk.virtualWrite(V1, deltaT);
    }

    // Reset for next measurement
    startDetected = false;
    endDetected = false;
    startTime = 0;
    endTime = 0;
  }
}
