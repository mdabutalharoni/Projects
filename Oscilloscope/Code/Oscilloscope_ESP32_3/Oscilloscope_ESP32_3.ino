#include <TFT_eSPI.h>
#include <SPI.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define ADC_PIN     33
#define BTN_VOLT    14
#define BTN_TIME    27
#define BTN_HOLD    26
#define BTN_RESET   25

TFT_eSPI tft = TFT_eSPI();

const int NUM_POINTS = 320;
uint16_t values[NUM_POINTS];

float voltDivOptions[] = {0.2, 0.5, 1.0, 2.0, 5.0};
int voltDivIndex = 2;  // Start at 1.0V/div

int timeDelays[] = {20, 50, 100, 200, 500};  // in microseconds
int timeDivIndex = 2;

bool isPaused = false;

unsigned long lastDebounce = 0;
const int debounceDelay = 200;

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);  // Landscape
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);

  pinMode(BTN_VOLT, INPUT_PULLUP);
  pinMode(BTN_TIME, INPUT_PULLUP);
  pinMode(BTN_HOLD, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);

  drawAxes();
}

void loop() {
  handleButtons();

  if (!isPaused) {
    sampleSignal();
    drawWaveform();
  }

  delay(100);
}

void sampleSignal() {
  for (int i = 0; i < NUM_POINTS; i++) {
    values[i] = analogRead(ADC_PIN);
    delayMicroseconds(timeDelays[timeDivIndex]);
  }
}

void drawWaveform() {
  float vRef = 3.3;
  float voltsPerDiv = voltDivOptions[voltDivIndex];
  float pixelsPerVolt = 40.0 / voltsPerDiv;

  tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_BLACK);
  drawAxes();

  for (int i = 1; i < NUM_POINTS; i++) {
    float v1 = (values[i - 1] / 4095.0) * vRef;
    float v2 = (values[i] / 4095.0) * vRef;

    int y1 = SCREEN_HEIGHT / 2 - (v1 - vRef / 2) * pixelsPerVolt;
    int y2 = SCREEN_HEIGHT / 2 - (v2 - vRef / 2) * pixelsPerVolt;

    y1 = constrain(y1, 0, SCREEN_HEIGHT - 1);
    y2 = constrain(y2, 0, SCREEN_HEIGHT - 1);

    int x1 = SCREEN_WIDTH - (i - 1);
    int x2 = SCREEN_WIDTH - i;

    tft.drawLine(x1, y1, x2, y2, TFT_GREEN);
  }

  // Labels
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(10, 10);
  tft.printf("Volt/div: %.1f V", voltDivOptions[voltDivIndex]);
  tft.setCursor(10, 25);
  tft.printf("Time/div: %d us", timeDelays[timeDivIndex]);
  if (isPaused) {
    tft.setCursor(SCREEN_WIDTH - 60, 10);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.print("PAUSED");
  }
}

void drawAxes() {
  int xCenter = SCREEN_WIDTH / 2;
  int yCenter = SCREEN_HEIGHT / 2;

  tft.drawLine(0, yCenter, SCREEN_WIDTH, yCenter, TFT_WHITE);
  tft.drawLine(xCenter, 0, xCenter, SCREEN_HEIGHT, TFT_WHITE);

  for (int i = -5; i <= 5; i++) {
    int y = yCenter - i * 40;
    if (y >= 0 && y < SCREEN_HEIGHT) {
      tft.drawLine(xCenter - 4, y, xCenter + 4, y, TFT_WHITE);
    }
  }

  for (int i = -7; i <= 7; i++) {
    int x = xCenter + i * 20;
    if (x >= 0 && x < SCREEN_WIDTH) {
      tft.drawLine(x, yCenter - 4, x, yCenter + 4, TFT_WHITE);
    }
  }
}

void handleButtons() {
  if (millis() - lastDebounce < debounceDelay) return;

  if (!digitalRead(BTN_VOLT)) {
    voltDivIndex = (voltDivIndex + 1) % (sizeof(voltDivOptions) / sizeof(float));
    lastDebounce = millis();
  }

  if (!digitalRead(BTN_TIME)) {
    timeDivIndex = (timeDivIndex + 1) % (sizeof(timeDelays) / sizeof(int));
    lastDebounce = millis();
  }

  if (!digitalRead(BTN_HOLD)) {
    isPaused = !isPaused;
    Serial.println(isPaused ? "Paused" : "Running");
    lastDebounce = millis();
  }

  if (!digitalRead(BTN_RESET)) {
    voltDivIndex = 2;
    timeDivIndex = 2;
    isPaused = false;
    Serial.println("Reset settings");
    lastDebounce = millis();
  }
}
