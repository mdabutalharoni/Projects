// Pin assignment
const int alcoholPin = 33;       // Alcohol sensor connected to GPIO 33
const int vibrationPin = 27;     // Vibration sensor connected to GPIO 27
const int irSensorPin = 14;      // IR sensor connected to GPIO 14

// Variables to store sensor readings
int alcoholValue = 0;
int vibrationValue = 0;
int irSensorValue = 0;

void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);

  // Initialize pins as input
  pinMode(alcoholPin, INPUT);
  pinMode(vibrationPin, INPUT);
  pinMode(irSensorPin, INPUT);
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

  // Send sensor values over Serial to receiver
  Serial.print("A:");
  Serial.print(alcoholValue);
  Serial.print(" V:");
  Serial.print(vibrationValue);
  Serial.print(" IR:");
  Serial.println(irSensorValue);

  // Wait before the next reading
  delay(500);
}
