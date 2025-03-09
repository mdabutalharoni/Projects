#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

RTC_DS3231 rtc;

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

#define BUZZER_PIN 11 // Define the pin for the buzzer

// Define a structure to hold alarm data
struct AlarmData {
  uint8_t hour;
  uint8_t minute;
  //uint8_t second;
};

// Address in EEPROM where alarm data will be stored
#define EEPROM_ADDR_ALARM 0

void setup() {
  Serial.begin(9600);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  // display.display();
  // delay(2000); // Pause for 2 seconds

  // Set up buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Make sure buzzer is initially off

  // Load alarm from EEPROM
  loadAlarmFromEEPROM();
  setAlarm(21,25);
}

void loop() {
  // Get the current time
  DateTime now = rtc.now();

  // Display the current alarm on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Time: ");
  display.print(now.hour(), DEC);
  display.print(':');
  display.print(now.minute(), DEC);
  display.print(':');
  display.println(now.second(), DEC);
  display.print("Current alarm: ");
  printAlarmOnDisplay();
  display.display();

  // Check if the current time matches the alarm time
  AlarmData alarm;
  EEPROM.get(EEPROM_ADDR_ALARM, alarm);
  if (now.hour() == alarm.hour && now.minute() == alarm.minute) {
    // Trigger the buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    // delay(1000); // Buzzer on for 1 second
    // digitalWrite(BUZZER_PIN, LOW);
    // delay(1000); // Delay for 1 second before re-checking the alarm
  }
  else{
    digitalWrite(BUZZER_PIN, LOW)7;;
  }

  delay(1000); // Update every second
}

// Function to print the current alarm on OLED
void printAlarmOnDisplay() {
  AlarmData alarm;
  EEPROM.get(EEPROM_ADDR_ALARM, alarm);
  display.print(alarm.hour);
  display.print(":");
  display.println(alarm.minute);
}

// Function to save alarm data to EEPROM
void saveAlarmToEEPROM(AlarmData alarm) {
  EEPROM.put(EEPROM_ADDR_ALARM, alarm);
}

// Function to load alarm data from EEPROM
void loadAlarmFromEEPROM() {
  AlarmData alarm;
  EEPROM.get(EEPROM_ADDR_ALARM, alarm);
}

// Function to set the alarm and save it to EEPROM
void setAlarm(uint8_t hour, uint8_t minute) {
  AlarmData alarm = {hour, minute};
  saveAlarmToEEPROM(alarm);
}
