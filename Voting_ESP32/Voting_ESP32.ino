// ESP32 Real-Time Voting System (Final UI Adjustments: No images, Sky Blue highlight, Bigger Bold Text)
// Features: FreeRTOS task, SSE, TFT feedback, modern web UI

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>

const char* AP_SSID = "Voting-ESP32";
const char* AP_PASS = "12345678";

#define BTN_BOAT   14
#define BTN_DHANER 27
#define BTN_PALLA  26
#define BTN_LANGOL 25
const int BUTTON_PINS[4] = { BTN_BOAT, BTN_DHANER, BTN_PALLA, BTN_LANGOL };

volatile uint32_t votes[4] = {0};
const char* LABELS[4] = { "Nowka", "Dhaner Shish", "Dari Palla", "Langol" };
const char* OPTIONS[4] = { "A", "B", "C", "D" };

TFT_eSPI tft = TFT_eSPI();
AsyncWebServer server(80);
AsyncEventSource events("/events");
TaskHandle_t broadcasterTask;

int lastVotedIndex = -1;

void drawGridBox(int x, int y, int w, int h, const char* top, const char* bottom, bool highlight) {
  uint16_t bg = highlight ? TFT_CYAN : TFT_BLACK;
  uint16_t textColor = highlight ? TFT_BLACK : TFT_WHITE;
  uint16_t borderColor = TFT_WHITE;

  tft.fillRect(x, y, w, h, bg);
  tft.drawRect(x, y, w, h, borderColor);

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(textColor);
  tft.drawString(top, x + w / 2, y + 20, 4);   // Bigger & bold font
  tft.drawString(bottom, x + w / 2, y + h - 40, 4);
}

void updateTFT() {
  int w = tft.width() / 2;
  int h = tft.height() / 2;
  for (int i = 0; i < 4; i++) {
    int col = i % 2;
    int row = i / 2;
    int x = col * w;
    int y = row * h;
    drawGridBox(x, y, w, h, OPTIONS[i], LABELS[i], i == lastVotedIndex);
  }
}

void IRAM_ATTR handleVote(int i) {
  votes[i]++;
  lastVotedIndex = i;
  updateTFT();
}

void setupButtons() {
  for (int i = 0; i < 4; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }
}

void checkButtons() {
  static uint32_t last[4] = {0};
  uint32_t now = millis();
  for (int i = 0; i < 4; i++) {
    if (digitalRead(BUTTON_PINS[i]) == LOW && now - last[i] > 300) {
      last[i] = now;
      handleVote(i);
    }
  }
}

void broadcaster(void* param) {
  while (true) {
    StaticJsonDocument<128> doc;
    JsonArray arr = doc.createNestedArray("counts");
    for (int i = 0; i < 4; i++) arr.add(votes[i]);
    String json;
    serializeJson(doc, json);
    events.send(json.c_str(), NULL, millis());
    delay(500);
  }
}

void setupWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send_P(200, "text/html", R"rawliteral(
<!DOCTYPE html><html lang="en"><head>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 Voting</title>
<style>
  body {
    margin: 0; padding: 0;
    font-family: 'Segoe UI', sans-serif;
    background: linear-gradient(135deg, #0f2027, #203a43, #2c5364);
    color: white;
    text-align: center;
  }
  h2 { margin: 1rem 0; font-weight: 300; }
  .grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
    gap: 1rem;
    padding: 1rem;
    max-width: 800px;
    margin: auto;
  }
  .card {
    background: #1e293b;
    border-radius: 16px;
    padding: 1.2rem;
    box-shadow: 0 6px 18px rgba(0,0,0,0.25);
    transition: transform 0.3s, background 0.6s ease;
  }
  .card.active {
    background: #0284c7 !important;
  }
  .label {
    font-size: 1.3em;
    font-weight: 700;
    color: #a5f3fc;
    margin-bottom: 0.7rem;
  }
  .value {
    font-size: 2.6em;
    color: #fff;
    font-weight: bold;
  }
</style>
</head><body>
<h2>Live Voting Dashboard</h2>
<div class="grid">
  <div id="c0" class="card">
    <div class="label">A - Nowka</div><div id="v0" class="value">0</div>
  </div>
  <div id="c1" class="card">
    <div class="label">B - Dhaner Shish</div><div id="v1" class="value">0</div>
  </div>
  <div id="c2" class="card">
    <div class="label">C - Dari Palla</div><div id="v2" class="value">0</div>
  </div>
  <div id="c3" class="card">
    <div class="label">D - Langol</div><div id="v3" class="value">0</div>
  </div>
</div>
<script>
function update(vals) {
  for (let i = 0; i < 4; i++) {
    const countEl = document.getElementById('v' + i);
    const cardEl = document.getElementById('c' + i);
    if (countEl && cardEl) {
      countEl.textContent = vals[i];
      cardEl.classList.add('active');
      clearTimeout(cardEl.timer);
      cardEl.timer = setTimeout(() => cardEl.classList.remove('active'), 1000);
    }
  }
}
let sse = new EventSource('/events');
sse.onmessage = e => {
  try {
    let d = JSON.parse(e.data);
    if (d.counts) update(d.counts);
  } catch (err) {
    console.error("SSE parse error", err);
  }
};
</script>
</body></html>
)rawliteral");
  });
  server.addHandler(&events);
  server.begin();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.println("AP IP: " + WiFi.softAPIP().toString());
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  updateTFT();
  setupButtons();
  setupWebServer();
  xTaskCreatePinnedToCore(broadcaster, "broadcaster", 4096, NULL, 1, &broadcasterTask, 1);
}

void loop() {
  checkButtons();
}