#include <IBusBM.h>
IBusBM ibus;

// BTS7960 Motor Driver Pins
#define LF 5   // Left Motor Forward
#define LB 6   // Left Motor Backward
#define RF 9  // Right Motor Forward
#define RB 10   // Right Motor Backward

void setup() {
  Serial.begin(115200);
  ibus.begin(Serial);

  pinMode(LF, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(RB, OUTPUT);
}

// Function to read mapped iBus channel
int readChannel(byte ch, int minVal, int maxVal, int defaultVal) {
  uint16_t val = ibus.readChannel(ch);
  if (val < 100) return defaultVal;
  return map(val, 1000, 2000, minVal, maxVal);
}

// Function to read switch channels as boolean
bool readSwitch(byte ch, bool defaultVal) {
  int val = readChannel(ch, 0, 100, defaultVal ? 100 : 0);
  return val > 50;
}

void loop() {
  // Read channel 6 for mode switch
  bool mode = readSwitch(5, false);  // CH6 = index 5

  int forwardBackward = readChannel(1, -255, 255, 0);  // CH2
  int leftRight;

  if (!mode) {
    leftRight = readChannel(0, -200, 200, 0);  // CH1 when CH6 == 0
  } else {
    leftRight = readChannel(3, -200, 200, 0);  // CH4 when CH6 == 1
  }

  // Software Elevon Mixing
  int leftMotor = forwardBackward + leftRight;
  int rightMotor = forwardBackward - leftRight;

  // Clamp speed to valid PWM range
  leftMotor = constrain(leftMotor, -255, 255);
  rightMotor = constrain(rightMotor, -255, 255);

  if (abs(leftMotor) < 10) leftMotor = 0;
  if (abs(rightMotor) < 10) rightMotor = 0;


  controlMotors(leftMotor, rightMotor);
  delay(20);  // Smooth update
}

void controlMotors(int leftMotor, int rightMotor) {
  leftMotor = constrain(leftMotor, -255, 255);
  rightMotor = constrain(rightMotor, -255, 255);

  // LEFT Motor Control
  if (leftMotor > 0) {
    analogWrite(LF, leftMotor);  // Forward
    analogWrite(LB, 0);
  } else if (leftMotor < 0) {
    analogWrite(LF, 0);
    analogWrite(LB, -leftMotor);  // Backward
  } else {
    analogWrite(LF, 0);
    analogWrite(LB, 0);  // Stop
  }

  // RIGHT Motor Control
  if (rightMotor > 0) {
    analogWrite(RF, rightMotor);  // Forward
    analogWrite(RB, 0);
  } else if (rightMotor < 0) {
    analogWrite(RF, 0);
    analogWrite(RB, -rightMotor);  // Backward
  } else {
    analogWrite(RF, 0);
    analogWrite(RB, 0);  // Stop
  }
}
