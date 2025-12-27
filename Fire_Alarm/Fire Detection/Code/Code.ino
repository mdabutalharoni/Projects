// RUET Fire Alarm Dashboard — ESP32 (Arduino core 3.3.0) + LittleFS + Async SSE
// WDT-safe: work is done in a dedicated FreeRTOS task with proper delays.

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

// ------------ EDIT YOUR WIFI ------------
const char* WIFI_SSID = "3 Idiots";
const char* WIFI_PASS = "qwerty1234";
// ---------------------------------------

AsyncWebServer server(80);
AsyncEventSource events("/events");

// Publisher task handle
TaskHandle_t publisherHandle = nullptr;

// Helper: send one unit's JSON as a named SSE event
static void sendUnitEvent(const char* eventName, float gasPPM, bool gasDetected, float tempC, float battV) {
  StaticJsonDocument<256> doc;
  doc["gasPPM"]       = gasPPM;
  doc["gasDetected"]  = gasDetected;
  doc["temperatureC"] = tempC;
  doc["batteryV"]     = battV;
  String payload;
  serializeJson(doc, payload);
  events.send(payload.c_str(), eventName, millis());
}

// FreeRTOS publisher task (runs on core 1, sleeps between sends)
void publisherTask(void* pv) {
  uint32_t lastBeat = millis();

  for (;;) {
    // ---- every ~2s: push readings ----
    {
      // Unit 1 (Zone A)
      float gas1  = 40.0 + (random(-50, 50) * 0.5f);
      float temp1 = 24.0 + (random(-15, 15) * 0.1f);
      float batt1 = 12.3 + (random(-2, 2) * 0.05f);
      bool alert1 = gas1 > 120.0f;

      // Unit 2 (Zone B)
      float gas2  = 35.0 + (random(-50, 50) * 0.6f);
      float temp2 = 22.0 + (random(-15, 15) * 0.1f);
      float batt2 = 11.9 + (random(-2, 2) * 0.05f);
      bool alert2 = gas2 > 100.0f;

      sendUnitEvent("unit1_readings", gas1, alert1, temp1, batt1);
      sendUnitEvent("unit2_readings", gas2, alert2, temp2, batt2);
    }

    // ---- every ~5s: heartbeat ----
    if (millis() - lastBeat > 5000) {
      lastBeat = millis();
      events.send("tick", "heartbeat", millis());
    }

    // Yield CPU to AsyncTCP/WiFi by sleeping
    vTaskDelay(pdMS_TO_TICKS(2000));  // 2s cadence
  }
}

void setup() {
  Serial.begin(115200);
  delay(50);

  // LittleFS
  if (!LittleFS.begin(true)) {
    Serial.println("[LittleFS] Mount failed");
  } else {
    Serial.println("[LittleFS] Mounted");
  }

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);  // IMPORTANT: prevent power-save stalls with async I/O
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("[WiFi] Connecting to %s", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println();
  Serial.print("[WiFi] IP: "); Serial.println(WiFi.localIP());

  // Static files
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  // SSE
  events.onConnect([](AsyncEventSourceClient *client){
    Serial.println("[SSE] Client connected");
    // Keep this callback light; no loops or heavy work here
    client->send("hello", NULL, millis(), 1000);
  });
  server.addHandler(&events);

  // Optional: simulate endpoints for quick tests
  server.on("/simulate/1", HTTP_GET, [](AsyncWebServerRequest* req){
    sendUnitEvent("unit1_readings", 165.0f, true, 28.0f, 12.0f);
    req->send(200, "text/plain", "Simulated Unit 1 alert");
  });
  server.on("/simulate/2", HTTP_GET, [](AsyncWebServerRequest* req){
    sendUnitEvent("unit2_readings", 140.0f, true, 27.2f, 11.7f);
    req->send(200, "text/plain", "Simulated Unit 2 alert");
  });

  server.begin();
  Serial.println("[HTTP] Server started");

  // Create publisher task on core 1 with adequate stack
  xTaskCreatePinnedToCore(
    publisherTask,        // task function
    "publisherTask",      // name
    4096,                 // stack size (bytes)
    nullptr,              // parameter
    1,                    // priority (low is fine)
    &publisherHandle,     // handle
    1                     // core 1 (Arduino loop also runs on 1; this is lightweight)
  );
}

void loop() {
  // Keep loop empty/light. No blocking here.
  // If you must do something, add small delays to yield:
  vTaskDelay(pdMS_TO_TICKS(50));
}
