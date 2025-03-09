#include <IBusBM.h>

IBusBM ibus;

// Motor driver pins
#define ENA 3
#define IN1 5
#define IN2 4

#define ENB 9
#define IN3 8
#define IN4 7

void setup() {
  Serial.begin(115200);
  ibus.begin(Serial);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultv) {
  uint16_t ch = ibus.readChannel(channelInput);
  if (ch < 100) return defaultv;  // If no signal, return default
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

bool readSwitch(byte channelInput, bool defaultv) {
  int intDefaultv = (defaultv) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultv);
  return (ch > 50);
}

void loop() {
  bool sw = readSwitch(4, false);  // Read switch from Channel 4

  int ch1, ch2, leftMotor, rightMotor;

  if (!sw) {  
    ch1 = readChannel(0, -255, 255, 0);  // Aileron (Left/Right) from Channel 0
    ch2 = readChannel(1, -255, 255, 0);  // Elevator (Forward/Backward) from Channel 1
  } else {  
    ch1 = readChannel(3, -255, 255, 0);  // Aileron (Left/Right) from Channel 3
    ch2 = readChannel(1, -255, 255, 0);  // Elevator (Forward/Backward) from Channel 1
  }

  // Software Elevon Mixing: Compute Left and Right Motor Speeds
  leftMotor = ch2 + ch1;
  rightMotor = ch2 - ch1;

  // Limit values within range
  leftMotor = constrain(leftMotor, -255, 255);
  rightMotor = constrain(rightMotor, -255, 255);

  controlMotors(leftMotor, rightMotor);

  delay(50);
}


