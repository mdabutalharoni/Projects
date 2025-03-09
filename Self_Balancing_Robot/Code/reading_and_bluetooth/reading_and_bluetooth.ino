#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "BluetoothSerial.h"  // Include BluetoothSerial library for Bluetooth communication

// Create an MPU6050 object
MPU6050 mpu;
BluetoothSerial SerialBT;     // Create Bluetooth serial object

// Variables to store sensor data
float pitch = 0;
float gyroRate = 0;
unsigned long prevTime = 0;
float prevPitch = 0;

// Variables to store Bluetooth commands
char command = 'S';  // Default command is Stop

void setup() {
  // Initialize serial communication for debugging and Bluetooth
  Serial.begin(115200);
  SerialBT.begin("ESP32_Balancing_Robot"); // Set Bluetooth device name

  // Initialize MPU6050 sensor
  Wire.begin();
  mpu.initialize();

  // Check if the MPU6050 is connected properly
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful!");
  } else {
    Serial.println("MPU6050 connection failed!");
    while(1);  // Stop the program if there's no connection
  }
}

void loop() {
  // Get the current time for calculations
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - prevTime) / 1000.0; // Time difference in seconds

  // Get raw accelerometer and gyroscope data
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Convert gyroscope data to angular velocity in degrees per second
  gyroRate = gx / 131.0; // MPU6050 gives raw values, divide by 131 to get degrees per second

  // Calculate pitch angle using accelerometer data (in degrees)
  pitch = atan2(ay, az) * 180 / PI;

  // Calculate the filtered pitch using complementary filter
  float alpha = 0.98;  // Complementary filter constant
  pitch = alpha * (prevPitch + gyroRate * deltaTime) + (1.0 - alpha) * pitch;

  // Update previous values
  prevTime = currentTime;
  prevPitch = pitch;

  // Output the pitch angle and gyro rate for debugging
  Serial.print("Pitch Angle: ");
  Serial.print(pitch);
  Serial.print("  Gyro Rate: ");
  Serial.println(gyroRate);

  // Check for incoming Bluetooth command
  if (SerialBT.available()) {
    command = SerialBT.read();
    Serial.print("Received Command: ");
    Serial.println(command);
  }

  // Control logic based on Bluetooth commands
  switch (command) {
    case 'F':  // Forward
      Serial.println("Moving Forward");
      // Add your motor control logic here
      break;
    case 'B':  // Backward
      Serial.println("Moving Backward");
      // Add your motor control logic here
      break;
    case 'L':  // Left
      Serial.println("Turning Left");
      // Add your motor control logic here
      break;
    case 'R':  // Right
      Serial.println("Turning Right");
      // Add your motor control logic here
      break;
    default:   // Stop
      Serial.println("Stopping");
      // Add your motor control logic here for stop
      break;
  }

  // Wait for a short time before repeating
  delay(50);
}
