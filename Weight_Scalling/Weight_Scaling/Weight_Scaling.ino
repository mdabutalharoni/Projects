#define BLYNK_TEMPLATE_ID "TMPL6XJCsqeeW"
#define BLYNK_TEMPLATE_NAME "Weight scaling"
#define BLYNK_AUTH_TOKEN "W4nM2V4EAeMRtWw2q3MatKv41R3WfKwW"

#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "HX711.h"
#include <LiquidCrystal_I2C.h>

#define DOUT  16
#define CLK   4
HX711 scale;  // Initialize the HX711 object without arguments

// LCD Setup (I2C address, columns, rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 0x27 is the typical I2C address for 16x2 LCD

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "TP-Link_9A5A";
char pass[] = "45571107";

float weight; 
float calibration_factor = 211000; // for me this value works just perfect 211000  

void setup() {
  // Set up serial monitor
  Serial.begin(115200);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");
  
  // Initialize HX711 with DOUT and CLK pins
  scale.begin(DOUT, CLK);
  
  scale.set_scale();  // Set scale with no factor yet
  scale.tare();       // Reset the scale to 0
  long zero_factor = scale.read_average();  // Get a baseline reading
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);
  
  // Connect to Blynk
  Blynk.begin(auth, ssid, pass);
  
  // Initialize the LCD
  Wire.begin(21, 22);
  lcd.begin(16, 2);
  lcd.backlight();  // Turn on the LCD backlight
  lcd.setCursor(0, 0);
  lcd.print("Weight Scaling");  // Welcome message
  delay(2000);  // Display the message for 2 seconds
  lcd.clear();
}

void loop() {
  Blynk.run();
  measureweight();
}

void measureweight(){
 scale.set_scale(calibration_factor); // Adjust to this calibration factor
  Serial.print("Reading: ");
  weight = scale.get_units(5); 
  if(weight < 0) {
    weight = 0.00;
  }
  
  Serial.print("Kilogram: ");
  Serial.print(weight); 
  Serial.print(" Kg");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
  
  // Send weight to Blynk
  Blynk.virtualWrite(V0, weight);
  Blynk.virtualWrite(V1, weight);

  // Display the weight on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);  // Set the cursor to the first column, first row
  lcd.print("Weight:");  // Label
  lcd.setCursor(8, 0);   // Set the cursor to the 9th column, first row
  lcd.print(weight, 2);  // Print weight with 2 decimal places
  lcd.print(" kg");

  lcd.setCursor(0, 1);   // Second row
  lcd.print("Calib:");
  lcd.print(calibration_factor);  // Display calibration factor for reference

  // Delay before repeating measurement
  delay(1000);  // Update every second
}
