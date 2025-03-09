const int dirPin = 4;    // Direction pin
const int pulPin = 5;    // Pulse pin
const int enPin = 3;     // Enable pin
const int ldrPin1 = A0;  // LDR 1 pin
const int ldrPin2 = A1;  // LDR 2 pin

const int lightThreshold = 5; // Threshold to compare light values
const int stepsToRotate = 600; // Number of steps for rotation
const int pulseDelay = 500;      // Pulse width (adjust for motor speed)

void setup() {
  Serial.begin(9600); // Initialize serial communication
  pinMode(dirPin, OUTPUT); // Set direction pin as output
  pinMode(pulPin, OUTPUT); // Set pulse pin as output
  pinMode(enPin, OUTPUT);  // Set enable pin as output

  // Enable the driver
  digitalWrite(enPin, LOW); // Assuming LOW is to enable the driver

  // Set the direction to forward
  digitalWrite(dirPin, HIGH); // Set to HIGH for forward direction
}

void loop() {
  // Read the LDR values
  int ldrValue1 = analogRead(ldrPin1);
  int ldrValue2 = analogRead(ldrPin2);

  // Determine motor direction based on LDR values
  String direction = "";
  if (abs(ldrValue1 - ldrValue2) > lightThreshold) {
    if (ldrValue1 > ldrValue2) {
      digitalWrite(dirPin, HIGH); // Set direction to forward
      direction = "Forward";
    } else {
      digitalWrite(dirPin, LOW); // Set direction to backward
      direction = "Backward";
    }

    // Rotate the motor for a certain number of steps
    for (int i = 0; i < stepsToRotate; i++) {
      digitalWrite(pulPin, HIGH);
      delayMicroseconds(pulseDelay); // Pulse width (adjust for motor speed)
      digitalWrite(pulPin, LOW);
      //delayMicroseconds(pulseDelay); // Pulse width (adjust for motor speed)
    }
  } else {
    digitalWrite(pulPin, LOW); // No pulse means the motor won't rotate
    direction = "Stopped"; // No movement
  }

  // Print values in one line
  Serial.print("LDR1: ");
  Serial.print(ldrValue1);
  Serial.print(" | LDR2: ");
  Serial.print(ldrValue2);
  Serial.print(" | Direction: ");
  Serial.print(direction);
  Serial.print(" | Steps: ");
  Serial.println(stepsToRotate); // Print the number of steps

  // Optional delay to avoid too rapid switching
  delay(100); // Adjust as needed
}
