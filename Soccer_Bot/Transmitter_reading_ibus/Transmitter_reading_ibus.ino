#include <IBusBM.h>

IBusBM ibus;

int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultv) {
  uint16_t ch = ibus.readChannel(channelInput);
  if (ch < 100) return defaultv;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

// Read the channel and return a boolean v
bool readSwitch(byte channelInput, bool defaultv) {
  int intDefaultv = (defaultv) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultv);
  return (ch > 50);
}
int ch1, ch2, ch3, ch4, ch5, ch6;
bool f = true;
void setup() {
  Serial.begin(115200);
  ibus.begin(Serial);
  digitalWrite(13, 0);
  delay(2000);
}

void loop() {
  ch1 = readChannel(0, -250, 250, 0);    //ch - 1
  ch2 = readChannel(1, 250, -250, 0);  //ch - 2
  ch3 = readChannel(2, 0, 250, 0);     //ch - 3
  ch4 = readChannel(3, -250, 250, 0);  //ch - 4
  ch5 = readSwitch(4, false);     //ch - 5
  ch6 = readSwitch(5, false);

  printSignal();

}
void printSignal() {
  Serial.print("ch1: ");
  Serial.print(ch1);
  Serial.print(" | ");

  Serial.print("ch2: ");
  Serial.print(ch2);
  Serial.print(" | ");

  Serial.print("ch3: ");
  Serial.print(ch3);
  Serial.print(" | ");

  Serial.print("ch4: ");
  Serial.print(ch4);
  Serial.print(" | ");

  Serial.print("ch5: ");
  Serial.print(ch5);
  Serial.print(" | ");

  Serial.print("ch6: ");
  Serial.print(ch6);
  Serial.print(" | ");

  Serial.println();
  delay(300);
}