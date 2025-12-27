#include <esp_now.h>
#include <WiFi.h>

// Define a data structure Must be same as Initiator
struct received_data {
  char deviceName[32];
  float gas;
  float temp;
  float batt;
};

// Create a structured object
received_data myData;

// Callback function executed when data is received
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  Serial.print("Data received: ");
  Serial.println(len);

  // Print sender MAC (src_addr in new ESP-IDF)
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           info->src_addr[0], info->src_addr[1], info->src_addr[2],
           info->src_addr[3], info->src_addr[4], info->src_addr[5]);
  Serial.print("From MAC: ");
  Serial.println(macStr);

  Serial.print("Device Name: ");
  Serial.println(myData.deviceName);
  Serial.printf("Gas: %.2f ppm\n", myData.gas);
  Serial.printf("Temp: %.2f Celcius\n", myData.temp);
  Serial.printf("Battery: %.2f Volt\n", myData.batt);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
}
