// Include Libraries
#include <esp_now.h>
#include <WiFi.h>

unsigned long timer1 = 0;
unsigned long timer2 = 0;


// MAC Address of responder (Change it according to your MAC address)
uint8_t broadcastAddress[] = { 0xC8, 0xF0, 0x9E, 0x7B, 0x06, 0xE4 }; //C8:F0:9E:7B:06:E4


// Define a data structure. It should be same as responder.
struct sending_data {
  char deviceName[32];
  float gas;
  float temp;
  float batt;
};

// Create data object
sending_data myData;

// Peer info
esp_now_peer_info_t peerInfo;

// ESP-NOW send callback (ESP32-S3 compatible)
void OnDataSent(const esp_now_send_info_t *info, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


void esp_now_setup() {
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void setup() {
  Serial.begin(115200);
  esp_now_setup();
}

void loop() {
  // Format structured data that need to be sent
  strcpy(myData.deviceName, "I am Sensor(Initiator)");
  myData.gas = random(500, 1500)*0.1f;
  myData.temp = random(200, 500)*0.1f;
  myData.batt = 8 + random(0, 100)*0.1f;

  // Send every 500 ms
  if ((millis() - timer2) > 500) {
    esp_err_t result = esp_now_send(broadcastAddress,
                                    (uint8_t *)&myData,
                                    sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Sending confirmed");
    } else {
      Serial.println("Sending error");
    }
    timer2 = millis();
  }
}
