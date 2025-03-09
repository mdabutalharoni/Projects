#include <Wire.h>
#include <MPU6050_light.h>
#include <LiquidCrystal_I2C.h>

MPU6050 mpu(Wire);   
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long timer = 0;

// Reference points for calibration
float xref = 0, yref = 0, zref = 0;

// Variables to hold filtered angles
float angleZ, angleX, angleY;
const int numReadings = 5; 
float angleZReadings[numReadings]; 
float angleXReadings[numReadings]; 
float angleYReadings[numReadings]; 
int readIndex = 0;
float totalZ = 0, totalX = 0, totalY = 0;

// Define a threshold to ignore small fluctuations
const float threshold = 1.0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2); 
  lcd.backlight();    

  Wire.begin();
  mpu.begin();
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcGyroOffsets();
  Serial.println("Done!\n");

  // Initialize readings arrays to 0
  for (int i = 0; i < numReadings; i++) {
    angleZReadings[i] = 0;
    angleXReadings[i] = 0;
    angleYReadings[i] = 0;
  }
}

// Function to set the current angles as the reference point
void setReference() {
  xref = angleX;
  yref = angleY;
  zref = angleZ;
  Serial.println("Reference points set.");
}

void loop() {
  mpu.update();

  // Update total by subtracting the last reading
  totalZ -= angleZReadings[readIndex];
  totalX -= angleXReadings[readIndex];
  totalY -= angleYReadings[readIndex];

  // Read current angles and apply reference offsets
  angleZReadings[readIndex] = mpu.getAngleZ() - zref;
  angleXReadings[readIndex] = mpu.getAngleX() - xref;
  angleYReadings[readIndex] = mpu.getAngleY() - yref;

  // Add new readings to the total
  totalZ += angleZReadings[readIndex];
  totalX += angleXReadings[readIndex];
  totalY += angleYReadings[readIndex];

  readIndex = (readIndex + 1) % numReadings;

  // Calculate the average
  angleZ = totalZ / numReadings;
  angleX = totalX / numReadings;
  angleY = totalY / numReadings;

  // Apply threshold to ignore small fluctuations
  if (abs(angleX) < threshold) angleX = 0;
  if (abs(angleY) < threshold) angleY = 0;
  if (abs(angleZ) < threshold) angleZ = 0;

  if ((millis() - timer) > 100) {
    timer = millis();

    lcd.setCursor(0, 0);
    lcd.print("X: ");
    lcd.print(int(angleX));
    lcd.print("   "); // Clear any remaining characters

    lcd.print(" Y: ");
    lcd.print(int(angleY));
    lcd.print("   "); // Clear any remaining characters

    lcd.setCursor(0, 1);
    lcd.print("Z: ");
    lcd.print(int(angleZ));
    lcd.print("   "); // Clear any remaining characters

    // Print to Serial Monitor
    Serial.print("Angle X: "); Serial.print(angleX);
    Serial.print(" | Angle Y: "); Serial.print(angleY);
    Serial.print(" | Angle Z: "); Serial.println(angleZ);
  }

  // Example to set reference point if Serial input 'r' is received
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'r') {
      setReference();
    }
  }
}
