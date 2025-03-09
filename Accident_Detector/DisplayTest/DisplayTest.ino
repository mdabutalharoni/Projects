#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin not used
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Initialize the OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is the I2C address for most 0.96" OLEDs
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();  // Clear the buffer
  display.setTextSize(1);  // Set text size to 1
  display.setTextColor(SSD1306_WHITE);  // Set color to white

  // Display static text on the OLED
  display.setCursor(0, 10);  // Set position for the text
  display.println(F("Eto Slow Keno")); // Text to display

  display.display(); // Display the text on the OLED
}

void loop() {
  // Nothing to do here, static text is shown
}
