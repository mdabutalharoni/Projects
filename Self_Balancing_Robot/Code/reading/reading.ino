#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Define motor driver pins for L293N
#define MOTOR_ENA 14  // Motor A PWM pin
#define MOTOR_IN1 26  // Motor A control pin
#define MOTOR_IN2 27  // Motor A control pin

#define MOTOR_ENB 32  // Motor B PWM pin
#define MOTOR_IN3 33  // Motor B control pin
#define MOTOR_IN4 25  // Motor B control pin

// Define MPU6050 interrupt pin
#define MPU_INT_PIN 2

Adafruit_MPU6050 mpu;

// PID constants
float kp = 40;  // Proportional gain
float ki = 0;   // Integral gain
float kd = 0.1;   // Derivative gain

float setPoint = -2.5;   // Target angle (upright position)
float lastError = 0;
float errorSum = 0;
float lastTime = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }
  
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  pinMode(MPU_INT_PIN, INPUT);

  // Motor setup
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);
  
  // Configure PWM for motors
  ledcSetup(0, 1000, 8);  // Channel 0 for MOTOR_ENA
  ledcAttachPin(MOTOR_ENA, 0);  // Attach MOTOR_ENA to channel 0

  ledcSetup(1, 1000, 8);  // Channel 1 for MOTOR_ENB
  ledcAttachPin(MOTOR_ENB, 1);  // Attach MOTOR_ENB to channel 1

  Serial.println("MPU6050 and motor setup completed.");
}

void loop() {
  // Get MPU6050 sensor data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Calculate pitch from accelerometer data
  float pitch = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;

  // Calculate PID control values
  float error = setPoint - pitch;
  float currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 1000.0;  // Convert to seconds
  
  errorSum += error * deltaTime;  // Integral term
  float rateOfChange = (error - lastError) / deltaTime;  // Derivative term
  
  // PID output
  float pidOutput = (kp * error) + (ki * errorSum) + (kd * rateOfChange);
  
  // Constrain PID output to motor speed range (0 to 255)
  pidOutput = constrain(pidOutput, -255, 255);

  // Control motors based on PID output
  controlMotors(pidOutput);

  // Update last error and time
  lastError = error;
  lastTime = currentTime;

  // Display data for debugging
  Serial.print("Pitch: ");
  Serial.print(pitch);
  Serial.print("°\tPID Output: ");
  Serial.println(pidOutput);

  delay(100);  // Short delay for smoother operation
}

void controlMotors(float speed) {
  int leftMotorSpeed, rightMotorSpeed;

  // Split PID output for both motors
  if (speed > 0) {
    leftMotorSpeed = speed;
    rightMotorSpeed = speed - (speed * 0.2);  // Slightly reduce right motor speed for balancing
  } else {
    leftMotorSpeed = speed + (speed * 0.2);  // Slightly increase left motor speed for balancing
    rightMotorSpeed = speed;
  }

  // Control left motor (Motor A)
  if (leftMotorSpeed > 0) {
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
    ledcWrite(0, leftMotorSpeed);  // Channel 0 controls MOTOR_ENA
    Serial.print("Left Motor: Speed = ");
    Serial.print(leftMotorSpeed);
    Serial.println(", Direction = FORWARD");
  } else {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
    ledcWrite(0, -leftMotorSpeed);  // Reverse motor speed
    Serial.print("Left Motor: Speed = ");
    Serial.print(-leftMotorSpeed);
    Serial.println(", Direction = BACKWARD");
  }

  // Control right motor (Motor B)
  if (rightMotorSpeed > 0) {
    digitalWrite(MOTOR_IN3, HIGH);
    digitalWrite(MOTOR_IN4, LOW);
    ledcWrite(1, rightMotorSpeed);  // Channel 1 controls MOTOR_ENB
    Serial.print("Right Motor: Speed = ");
    Serial.print(rightMotorSpeed);
    Serial.println(", Direction = FORWARD");
  } else {
    digitalWrite(MOTOR_IN3, LOW);
    digitalWrite(MOTOR_IN4, HIGH);
    ledcWrite(1, -rightMotorSpeed);  // Reverse motor speed
    Serial.print("Right Motor: Speed = ");
    Serial.print(-rightMotorSpeed);
    Serial.println(", Direction = BACKWARD");
  }
}
