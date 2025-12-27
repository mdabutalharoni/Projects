#include <TFT_eSPI.h>
#include <SPI.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define ADC_PIN 33
#define BUTTON_VOLT 14  // Voltage scale button
#define BUTTON_TIME 27  // Time scale button

TFT_eSPI tft = TFT_eSPI();

const int NUM_POINTS = 320;
uint16_t values[NUM_POINTS];

// Voltage scales (in volts per division)
float voltDivOptions[] = { 0.5, 1.0, 2.0, 5.0 };
int voltDivIndex = 1;  // Default 1V/div

// Time delay per sample (affects horizontal time scale)
int timeDelays[] = { 50, 100, 200, 500 };  // In microseconds
int timeDivIndex = 1;

unsigned long lastDebounce = 0;
const int debounceDelay = 200;

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);

  pinMode(BUTTON_VOLT, INPUT_PULLUP);
  pinMode(BUTTON_TIME, INPUT_PULLUP);

  drawAxes();  // Draw only once
}

void loop() {
  // Handle button presses with debouncing
  if (millis() - lastDebounce > debounceDelay) {
    if (digitalRead(BUTTON_VOLT) == LOW) {
      voltDivIndex = (voltDivIndex + 1) % (sizeof(voltDivOptions) / sizeof(voltDivOptions[0]));
      lastDebounce = millis();
    }
    if (digitalRead(BUTTON_TIME) == LOW) {
      timeDivIndex = (timeDivIndex + 1) % (sizeof(timeDelays) / sizeof(timeDelays[0]));
      lastDebounce = millis();
    }
  }

  // Sample ADC values
  for (int i = 0; i < NUM_POINTS; i++) {
    values[i] = analogRead(ADC_PIN);
    delayMicroseconds(timeDelays[timeDivIndex]);
  }

  // Clear graph area only
  tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_BLACK);
  drawAxes();

  // Draw waveform
  // Draw waveform with voltage scaling
  float voltsPerDiv = voltDivOptions[voltDivIndex];
  float pixelsPerVolt = 40.0 / voltsPerDiv;  // 1 div = 40 pixels
  float vRef = 3.3;

  for (int i = 1; i < NUM_POINTS; i++) {
    float voltage1 = (values[i - 1] / 4095.0) * vRef;
    float voltage2 = (values[i] / 4095.0) * vRef;

    int y1 = SCREEN_HEIGHT / 2 - (voltage1 - vRef / 2) * pixelsPerVolt;
    int y2 = SCREEN_HEIGHT / 2 - (voltage2 - vRef / 2) * pixelsPerVolt;

    // Clamp to display bounds
    y1 = constrain(y1, 0, SCREEN_HEIGHT - 1);
    y2 = constrain(y2, 0, SCREEN_HEIGHT - 1);

    int x1 = SCREEN_WIDTH - (i - 1);
    int x2 = SCREEN_WIDTH - i;

    tft.drawLine(x1, y1, x2, y2, TFT_GREEN);
  }


  // Draw scale info
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(10, 10);
  tft.printf("Volt/div: %.1f V", voltDivOptions[voltDivIndex]);

  tft.setCursor(10, 25);
  tft.printf("Time/div: %d us", timeDelays[timeDivIndex]);

  delay(100);
}

void drawAxes() {
  int xCenter = SCREEN_WIDTH / 2;
  int yCenter = SCREEN_HEIGHT / 2;

  // Draw X and Y axes
  tft.drawLine(0, yCenter, SCREEN_WIDTH, yCenter, TFT_WHITE);   // X-axis
  tft.drawLine(xCenter, 0, xCenter, SCREEN_HEIGHT, TFT_WHITE);  // Y-axis

  // Tick marks and labels (Y-axis, Voltage)
  for (int i = -5; i <= 5; i++) {
    int y = yCenter - i * 20;
    if (y >= 0 && y <= SCREEN_HEIGHT) {
      tft.drawLine(xCenter - 5, y, xCenter + 5, y, TFT_WHITE);
      tft.setCursor(xCenter + 8, y - 4);
      tft.setTextColor(TFT_WHITE);
      tft.setTextSize(1);
      tft.printf("%d", i);
    }
  }

  // Tick marks and labels (X-axis, Time)
  for (int i = -7; i <= 7; i++) {
    int x = xCenter + i * 20;
    if (x >= 0 && x <= SCREEN_WIDTH) {
      tft.drawLine(x, yCenter - 5, x, yCenter + 5, TFT_WHITE);
      tft.setCursor(x - 5, yCenter + 8);
      tft.setTextColor(TFT_WHITE);
      tft.setTextSize(1);
      tft.printf("%d", i);
    }
  }

  // Draw (0,0) label
  tft.setCursor(xCenter + 5, yCenter + 5);
  tft.setTextColor(TFT_YELLOW);
  tft.print("(0,0)");
}