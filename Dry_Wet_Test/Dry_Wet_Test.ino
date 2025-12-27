#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <math.h>

// SPI pins for STM32 (Blue Pill)
#define TFT_CS   PB1
#define TFT_DC   PB10
#define TFT_RST  PB11

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define MID_Y         (SCREEN_HEIGHT / 2)
#define LABEL_HEIGHT  20

// ========= USER SETTINGS =========
float AMPLITUDE = 100;         // Wave height in pixels
float FREQUENCY = 2.0;         // Wave cycles per screen
const char* PULSE_TYPE = "sine"; // Options: "sine", "square", "triangle"
// ================================

void drawAxes() {
  // X-axis
  tft.drawLine(0, MID_Y, SCREEN_WIDTH, MID_Y, ILI9341_DARKGREY);

  // Y-axis
  tft.drawLine(10, 0, 10, SCREEN_HEIGHT - LABEL_HEIGHT, ILI9341_DARKGREY);

  // Voltage Labels
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.print("+V");
  tft.setCursor(0, MID_Y);
  tft.print("0V");
  tft.setCursor(0, SCREEN_HEIGHT - LABEL_HEIGHT - 8);
  tft.print("-V");

  // Bottom Info Label
  tft.fillRect(0, SCREEN_HEIGHT - LABEL_HEIGHT, SCREEN_WIDTH, LABEL_HEIGHT, ILI9341_BLACK);
  tft.setCursor(5, SCREEN_HEIGHT - LABEL_HEIGHT + 4);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.print("Amp:");
  tft.print(AMPLITUDE, 0);
  tft.print(" Freq:");
  tft.print(FREQUENCY, 1);
  tft.print(" Type:");
  tft.print(PULSE_TYPE);
}

void setup() {
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(ILI9341_NAVY);
  drawAxes();
}

void loop() {
  // Clear waveform area only (not label)
  tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - LABEL_HEIGHT, ILI9341_NAVY);
  drawAxes();

  int lastX = 0;
  int lastY = MID_Y;

  for (int x = 0; x < SCREEN_WIDTH; x++) {
    float t = (float)x / SCREEN_WIDTH;
    float angle = 2 * PI * FREQUENCY * t;
    float value = 0;

    // Select waveform type
    if (strcmp(PULSE_TYPE, "sine") == 0) {
      value = sin(angle);
    } else if (strcmp(PULSE_TYPE, "square") == 0) {
      value = sin(angle) >= 0 ? 1.0 : -1.0;
    } else if (strcmp(PULSE_TYPE, "triangle") == 0) {
      value = 2 * abs(2 * (t * FREQUENCY - floor(t * FREQUENCY + 0.5))) - 1;
    }

    // Convert to screen Y coordinate
    int y = MID_Y - (int)(value * AMPLITUDE);
    y = constrain(y, 0, SCREEN_HEIGHT - LABEL_HEIGHT - 1);

    tft.drawLine(lastX, lastY, x, y, ILI9341_WHITE);
    lastX = x;
    lastY = y;
  }

  delay(300);
}
