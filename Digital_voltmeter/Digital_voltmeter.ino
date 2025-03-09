#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x27 for a 16x2 display

// Define analog pin for voltage measurement
const int analogPin = A0;
float voltage = 0;
float analogValue = 0;
const float referenceVoltage = 5.0;  // Reference voltage for the Arduino (5V or 3.3V)

void setup() {
  lcd.begin(16, 2);  // Initialize the LCD with 16 columns and 2 rows
  lcd.backlight();    // Turn on the backlight
  lcd.setCursor(0, 0);    // Set cursor to the first line
  lcd.print("Volt Meter");
  delay(1000);
}

void loop() {
  analogValue = analogRead(analogPin);  // Read the analog value from the input pin
  voltage = (analogValue / 1023.0) * referenceVoltage*(110/10);  // Convert the analog value to voltage

  // Display analog value
  lcd.setCursor(0, 1);  // Set cursor to the second line
  lcd.print("Analog: ");
  lcd.print(analogValue);

  // Display calculated voltage
  lcd.setCursor(0, 0);  // Move to the first line
  lcd.print("Volt: ");
  lcd.print(voltage);
  lcd.print(" V ");

  delay(1000);  // Update every second
}
