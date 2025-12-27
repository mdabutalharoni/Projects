#define CH1 7
#define CH2 4
#define CH3 3
#define CH4 2
#define CH5 8
#define CH6 11

// Must adjust the pin and channel
#define LF 5
#define LB 6
#define RF 9
#define RB 10

int ch1;
int ch2;
int ch3;
int ch4;

bool ch5;
bool ch6;

int lf = 0, rf = 0, lb = 0, rb = 0;

int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue) {
  int ch = pulseIn(channelInput, HIGH, 30000);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

// Read the switch channel and return a boolean value
bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}

void setup() {
  // Set up serial monitor
  Serial.begin(115200);

  // Set all pins as inputs
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH3, INPUT);
  pinMode(CH4, INPUT);
  pinMode(CH5, INPUT);
  pinMode(CH6, INPUT);

  // Set motor pins as outputs
  pinMode(LF, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(RB, OUTPUT);
}

void loop() {
  // Get values for each channel
  ch1 = readChannel(CH1, -200, 200, 0); // For left/right in mode 1
  ch2 = readChannel(CH2, -250, 250, 0); // For forward/backward
  ch3 = readChannel(CH3, 0, 250, 125);  // For speed control in mode 1
  ch4 = readChannel(CH4, -200, 200, 0); // For left/right in mode 0
  ch5 = readSwitch(CH5, false);
  ch6 = readSwitch(CH6, false);

  // Dead zone adjustments
  if (abs(ch1) < 10) ch1 = 0;
  if (abs(ch2) < 10) ch2 = 0;
  if (abs(ch4) < 10) ch4 = 0;

  if (ch6 == 1) {
    // Mode 1: Forward/Backward with ch2, Left/Right with ch1, PWM using ch3
    if (ch2 < 0) {
      lf = rf = abs(ch2);
      lb = rb = 0;
    } else {
      lf = rf = 0;
      lb = rb = abs(ch2);
    }

    lf = lf - ch1;
    lb = lb + ch1;
    rf = rf + ch1;
    rb = rb - ch1;

    // Adjust for PWM control
    lf = constrain(lf, 0, ch3);
    rf = constrain(rf, 0, ch3);
    lb = constrain(lb, 0, ch3);
    rb = constrain(rb, 0, ch3);

  } else {
    // Mode 0: Forward/Backward with ch2, Left/Right with ch4, no PWM control
    if (ch2 > 0) {
      lf = rf = ch2;
      lb = rb = 0;
    } else {
      lf = rf = 0;
      lb = rb = abs(ch2);
    }

    lf = lf + ch4;
    lb = lb - ch4;
    rf = rf - ch4;
    rb = rb + ch4;

    // Constrain outputs within valid range
    lf = constrain(lf, 0, 250);
    rf = constrain(rf, 0, 250);
    lb = constrain(lb, 0, 250);
    rb = constrain(rb, 0, 250);
  }

  // Write outputs to motors
  analogWrite(LF, lf);
  analogWrite(LB, lb);
  analogWrite(RF, rf);
  analogWrite(RB, rb);

  // Print debug information to Serial Monitor
  // Serial.print("LF: ");
  // Serial.print(lf);
  // Serial.print(" | LB: ");
  // Serial.print(lb);
  // Serial.print(" | RF: ");
  // Serial.print(rf);
  // Serial.print(" | RB: ");
  // Serial.println(rb);

  // delay(50); // Reduced delay for smoother operation
}

