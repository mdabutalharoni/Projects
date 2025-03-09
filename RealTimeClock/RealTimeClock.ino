#include <Wire.h>          // Include Wire library for I2C communication
#include <Adafruit_GFX.h>  // Include Adafruit graphics library
#include <Adafruit_SSD1306.h> // Include Adafruit SSD1306 OLED library
#include <RTClib.h>        // Include RTC library


#define OLED_RESET -1     // Define OLED reset pin (or -1 if not used)
Adafruit_SSD1306 display(OLED_RESET); // Initialize OLED display
RTC_DS3231 rtc;             // Initialize RTC module

void setup() {
  Serial.begin(9600);       // Initialize serial communication
  if(!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Initialize display
  display.clearDisplay();   // Clear the display buffer
  display.setTextColor(SSD1306_WHITE); // Set text color to white
  display.setTextSize(1);   // Set text size
  display.setCursor(0,0);   // Set cursor position
  //display.invertDisplay(bool i);
}

void loop() {
  DateTime now = rtc.now(); // Get current time from RTC
  display.clearDisplay();   // Clear the display buffer
  display.setCursor(0,0);   // Set cursor position
  display.print(now.day(), DEC);
  display.print('/');
  display.print(now.month(), DEC);
  display.print('/');
  display.println(now.year(), DEC);
  
  display.print("Time: ");
  display.print(now.hour(), DEC); // Display hours
  display.print(':');
  display.print(now.minute(), DEC); // Display minutes
  display.print(':');
  display.println(now.second(), DEC); // Display seconds
  display.print("Day: ");
  display.println(getDayOfWeekName(now.dayOfTheWeek()));
  display.display();        // Display on OLED
  delay(1000);             // Delay for one second
}

String getDayOfWeekName(uint8_t day) {
  switch (day) {
    case 1:
      return "Monday";
    case 2:
      return "Tuesday";
    case 3:
      return "Wednesday";
    case 4:
      return "Thursday";
    case 5:
      return "Friday";
    case 6:
      return "Saturday";
    case 7:
      return "Sunday";
    default:
      return "Invalid day";
  }
}
