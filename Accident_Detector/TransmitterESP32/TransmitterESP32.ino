#include <WiFi.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>

// Blynk Auth Token
char auth[] = "-Ev2Q-ZV4CXqrbpgKcJrjfsq2WZJFq_D";
#define BLYNK_TEMPLATE_ID "TMPL6Sf0AwQcq"
#define BLYNK_TEMPLATE_NAME "AccidentDetector"



// Wi-Fi credentials
const char* ssid = "TP-Link_9A5A";
const char* password = "45571107";

// Server URL (receiver ESP32 IP)
const char* serverUrl = "http://192.168.37.130/data";  // Replace with your receiver's IP

// Pin assignment
const int alcoholPin = 33;       // Alcohol sensor connected to GPIO 33
const int vibrationPin = 27;     // Vibration sensor connected to GPIO 27
const int irSensorPin = 14;      // IR sensor connected to GPIO 14

// Variables to store sensor readings
int alcoholValue = 0;
int vibrationValue = 0;
int irSensorValue = 0;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize pins as input
  pinMode(alcoholPin, INPUT);
  pinMode(vibrationPin, INPUT);
  pinMode(irSensorPin, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected to WiFi");

  // Initialize Blynk connection
  Blynk.begin(auth, ssid, password);
}

void loop() {
  // Read the alcohol sensor value (analog)
  alcoholValue = analogRead(alcoholPin);
  
  // Read the vibration sensor value (digital)
  vibrationValue = digitalRead(vibrationPin);
  
  // Read the IR sensor value (digital)
  irSensorValue = digitalRead(irSensorPin);

  // Print sensor values to Serial Monitor
  Serial.print("Alcohol Sensor Value: ");
  Serial.println(alcoholValue);
  Serial.print("Vibration Sensor Value: ");
  Serial.println(vibrationValue);
  Serial.print("IR Sensor Value: ");
  Serial.println(irSensorValue);

  // Send sensor values to Blynk
  Blynk.virtualWrite(V1, alcoholValue);    // Send alcohol sensor value to V1
  Blynk.virtualWrite(V2, vibrationValue);  // Send vibration sensor value to V2
  Blynk.virtualWrite(V3, irSensorValue);   // Send IR sensor value to V3

  // Send data to the receiver ESP32 (server)
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Prepare URL with sensor data as parameters
    String url = String(serverUrl) + "?alcohol=" + alcoholValue + "&vibration=" + vibrationValue + "&ir=" + irSensorValue;
    
    // Send the request
    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response: " + response);
    } else {
      Serial.println("Error in sending GET request");
    }

    http.end();  // Close connection
  }

  // Run Blynk to keep the connection alive
  Blynk.run();

  // Wait before the next reading
  delay(2000);  // Adjust delay for your application
}
