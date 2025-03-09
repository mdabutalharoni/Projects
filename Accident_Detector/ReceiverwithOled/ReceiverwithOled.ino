#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "A particle";
const char* password = "Talhaspot";

// Pin assignment for the relay
const int relayPin = 33;

// Telegram bot information
const char* telegramBotToken = "7544634417:AAEK-gPZOx-8TcmM0zDkLQ5R5GBcsYEuL4k"; // Replace with your Telegram Bot Token
const char* chatID = "2005149959"; // Replace with your Telegram Chat ID

// Server URL for sending messages to Telegram
String telegramApiUrl = "https://api.telegram.org/bot" + String(telegramBotToken) + "/sendMessage";

// Create a WebServer object on port 80
WebServer server(80);

// Variables to store received sensor readings
int alcoholValue = 0;
int vibrationValue = 0;
int irValue = 0;
bool relayState = false;  // To track relay state and send message only when it changes to ON

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize relay pin as output
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Set relay off initially

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Define the server route for receiving data from the transmitter
  server.on("/data", handleSensorData);

  // Start the server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}

void handleSensorData() {
  // Get sensor values from query parameters
  if (server.hasArg("alcohol")) {
    alcoholValue = server.arg("alcohol").toInt();
  }
  if (server.hasArg("vibration")) {
    vibrationValue = server.arg("vibration").toInt();
  }
  if (server.hasArg("ir")) {
    irValue = server.arg("ir").toInt();
  }

  // Print received values to Serial Monitor
  Serial.print("Alcohol: ");
  Serial.println(alcoholValue);
  Serial.print("Vibration: ");
  Serial.println(vibrationValue);
  Serial.print("IR: ");
  Serial.println(irValue);

  // Control relay based on sensor readings
  if (alcoholValue > 200 || vibrationValue == 1 || irValue == 1) {
    if (!relayState) {
      digitalWrite(relayPin, HIGH);  // Turn ON the relay
      relayState = true;             // Update relay state
      Serial.println("Relay ON");
      
      // Send message to Telegram when relay is turned ON
      sendTelegramMessage("Relay turned ON due to sensor trigger!");
    }
  } else {
    if (relayState) {
      digitalWrite(relayPin, LOW);  // Turn OFF the relay
      relayState = false;           // Update relay state
      Serial.println("Relay OFF");
    }
  }

  // Send response to the transmitter
  server.send(200, "text/plain", "Data received and relay updated");
}

void sendTelegramMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = telegramApiUrl + "?chat_id=" + String(chatID) + "&text=" + message;
    
    // Send the request to Telegram
    http.begin(url);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Telegram message sent: " + response);
    } else {
      Serial.println("Error sending message to Telegram");
    }
    
    http.end();  // Close connection
  }
}