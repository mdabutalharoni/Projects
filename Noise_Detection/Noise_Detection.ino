#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Library for LCD

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD address and size

const int sampleWindow = 100;         // Increased sampling window for better stability
unsigned int sample;

#define SENSOR_PIN A0
#define GREEN_LED 2    // Quiet
#define YELLOW_LED 3   // Moderate
#define RED_LED 4      // High

void setup () {
  pinMode(SENSOR_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Noise Monitor");
  delay(1500);
  lcd.clear();
}

void loop () {
  unsigned long startMillis = millis(); 
  float peakToPeak = 0;
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(SENSOR_PIN);
    if (sample < 1024) {
      if (sample > signalMax) {
        signalMax = sample;
      }
      else if (sample < signalMin) {
        signalMin = sample;
      }
    }
  }

  peakToPeak = signalMax - signalMin;

  // Debugging values
  Serial.print("SignalMax: "); Serial.print(signalMax);
  Serial.print(" | SignalMin: "); Serial.print(signalMin);
  Serial.print(" | Peak-to-Peak: "); Serial.print(peakToPeak);

  // Map the value to a dB scale (adjusted for common sound sensors)
  int dB = map(peakToPeak, 10, 300, 40, 90);  // Adjust based on actual range
  dB = constrain(dB, 30, 100); // Clamp to valid range
  Serial.print(" | dB: "); Serial.println(dB);

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("Loudness: ");
  lcd.print(dB);
  lcd.print("dB   ");  // Padding to clear old values

  // LED & Level Logic
  lcd.setCursor(0, 1);
  if (dB <= 60) {
    lcd.print("Level: Quiet   ");
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
  }
  else if (dB <= 75) {
    lcd.print("Level: Moderate");
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  }
  else {
    lcd.print("Level: High    ");
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
  }

  delay(500);  // More stable visual output
  //lcd.clear();
}
