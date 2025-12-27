#include <TFT_eSPI.h>
#include <SPI.h>
#include <math.h>  // For sine wave test pattern

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define ADC_PIN 33
#define BUTTON_VOLT 14  // Voltage scale button
#define BUTTON_TIME 27  // Time scale button

TFT_eSPI tft = TFT_eSPI();

// Buffer for samples (double buffer for smoother display)
const int NUM_POINTS = SCREEN_WIDTH;
uint16_t samples[NUM_POINTS];
uint16_t displayBuffer[NUM_POINTS];

// Voltage scales (in volts per division)
const float voltDivOptions[] = {0.1, 0.2, 0.5, 1.0, 2.0, 5.0};
int voltDivIndex = 2;  // Default 0.5V/div

// Timebase settings (microseconds per sample)
const int timeDelays[] = {10, 20, 50, 100, 200, 500};  // In microseconds
int timeDivIndex = 2;  // Default 50μs

// Display settings
const int GRID_SIZE = 20;  // Pixels per division
const int GRID_ORIGIN_X = SCREEN_WIDTH / 2;
const int GRID_ORIGIN_Y = SCREEN_HEIGHT / 2;
const float VREF = 3.3;    // ESP32 ADC reference voltage
const int ADC_RESOLUTION = 4095;

// Button debounce
unsigned long lastDebounce = 0;
const int debounceDelay = 200;

// For frequency calculation
unsigned long lastSampleTime = 0;
float measuredFreq = 0;

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);

  pinMode(BUTTON_VOLT, INPUT_PULLUP);
  pinMode(BUTTON_TIME, INPUT_PULLUP);

  // Initialize with test pattern
  generateTestPattern();
  drawGrid();
}

void loop() {
  handleButtons();
  acquireSamples();
  processSamples();
  updateDisplay();
  delay(50);  // Reduce refresh rate for stability
}

void handleButtons() {
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
}

void acquireSamples() {
  unsigned long sampleInterval = timeDelays[timeDivIndex];
  unsigned long sampleTime = micros();
  
  for (int i = 0; i < NUM_POINTS; i++) {
    // Multi-sample averaging for noise reduction
    uint32_t sum = 0;
    for (int j = 0; j < 4; j++) {
      sum += analogRead(ADC_PIN);
    }
    samples[i] = sum / 4;
    
    // Accurate timing between samples
    while (micros() - sampleTime < sampleInterval) {}
    sampleTime += sampleInterval;
  }
  
  // Simple frequency measurement (time for 10 zero crossings)
  int zeroCrossings = 0;
  int lastSign = (samples[0] > 2048) ? 1 : -1;
  int firstCross = -1, lastCross = -1;
  
  for (int i = 1; i < NUM_POINTS; i++) {
    int currentSign = (samples[i] > 2048) ? 1 : -1;
    if (currentSign != lastSign) {
      if (firstCross == -1) firstCross = i;
      lastCross = i;
      zeroCrossings++;
      lastSign = currentSign;
    }
  }
  
  if (zeroCrossings >= 10) {
    float waveDuration = (lastCross - firstCross) * timeDelays[timeDivIndex] / 1e6;
    measuredFreq = (zeroCrossings / 2) / waveDuration;
  }
}

void processSamples() {
  // Copy to display buffer (could add filtering here)
  memcpy(displayBuffer, samples, sizeof(samples));
}

void updateDisplay() {
  // Clear only the graph area (preserve info area)
  tft.fillRect(0, 30, SCREEN_WIDTH, SCREEN_HEIGHT - 30, TFT_BLACK);
  drawGrid();
  
  // Draw the waveform
  float voltsPerDiv = voltDivOptions[voltDivIndex];
  float pixelsPerVolt = GRID_SIZE / voltsPerDiv;
  
  int prevX = 0;
  int prevY = GRID_ORIGIN_Y - ((displayBuffer[0] / float(ADC_RESOLUTION) * VREF - VREF/2) * pixelsPerVolt);
  prevY = constrain(prevY, 0, SCREEN_HEIGHT - 1);
  
  for (int i = 1; i < NUM_POINTS; i++) {
    int x = i;
    int y = GRID_ORIGIN_Y - ((displayBuffer[i] / float(ADC_RESOLUTION) * VREF - VREF/2) * pixelsPerVolt);
    y = constrain(y, 0, SCREEN_HEIGHT - 1);
    
    tft.drawLine(prevX, prevY, x, y, TFT_GREEN);
    prevX = x;
    prevY = y;
  }
  
  // Display info
  tft.setCursor(5, 5);
  tft.printf("V/div: %.2fV", voltDivOptions[voltDivIndex]);
  tft.setCursor(120, 5);
  tft.printf("Time: %dμs", timeDelays[timeDivIndex]);
  tft.setCursor(220, 5);
  tft.printf("Freq: %.1fHz", measuredFreq);
}

void drawGrid() {
  // Vertical lines (time)
  for (int x = GRID_ORIGIN_X % GRID_SIZE; x < SCREEN_WIDTH; x += GRID_SIZE) {
    tft.drawFastVLine(x, 30, SCREEN_HEIGHT - 30, TFT_DARKGREY);
  }
  
  // Horizontal lines (voltage)
  for (int y = GRID_ORIGIN_Y % GRID_SIZE; y < SCREEN_HEIGHT; y += GRID_SIZE) {
    tft.drawFastHLine(0, y, SCREEN_WIDTH, TFT_DARKGREY);
  }
  
  // Center axes
  tft.drawFastHLine(0, GRID_ORIGIN_Y, SCREEN_WIDTH, TFT_WHITE);
  tft.drawFastVLine(GRID_ORIGIN_X, 30, SCREEN_HEIGHT - 30, TFT_WHITE);
}

void generateTestPattern() {
  // Generate a test sine wave pattern
  for (int i = 0; i < NUM_POINTS; i++) {
    samples[i] = 2048 + 1500 * sin(i * 2 * PI / 50.0);
  }
}