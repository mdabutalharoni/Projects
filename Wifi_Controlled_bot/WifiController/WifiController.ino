#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

//SSID and Password to your ESP Access Point
const char* ssid = "ESP8266";


#define ENA   14          
#define ENB   12          
#define IN_1  4          
#define IN_2  5          
#define IN_3  0           
#define IN_4  2           
 
String command; 
int speedCar = 150;
int speed_low = 60;

ESP8266WebServer server(80);

void setup() {
 
 Serial.begin(115200);

 pinMode(ENA, OUTPUT); 
 pinMode(ENB, OUTPUT);
 pinMode(IN_1, OUTPUT);
 pinMode(IN_2, OUTPUT);
 pinMode(IN_3, OUTPUT);
 pinMode(IN_4, OUTPUT);
  
  
// Connecting WiFi

  WiFi.mode(WIFI_AP);           //Only Access point
  WiFi.softAP(ssid);  //Start HOTspot removing password will disable security

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
 // Starting WEB-server 
     server.on ( "/", HTTP_handleRoot );
     server.onNotFound ( HTTP_handleRoot );
     server.begin();    
}

void loop() {
    server.handleClient();
    
      command = server.arg("State");
      if (command == "F") Forword();
      else if (command == "B") Backward();
      else if (command == "L") Left();
      else if (command == "R") Right();
      else if (command == "FR") ForwordRight();
      else if (command == "FL") ForwordLeft();
      else if (command == "BR") BackRight();
      else if (command == "BL") BackLeft();
      else if (command == "0") speedCar = 100;
      else if (command == "1") speedCar = 120;
      else if (command == "2") speedCar = 140;
      else if (command == "3") speedCar = 160;
      else if (command == "4") speedCar = 180;
      else if (command == "5") speedCar = 200;
      else if (command == "6") speedCar = 215;
      else if (command == "7") speedCar = 230;
      else if (command == "8") speedCar = 240;
      else if (command == "9") speedCar = 255;
      else if (command == "S") Stop();
}

void HTTP_handleRoot(void) {

if( server.hasArg("State") ){
       Serial.println(server.arg("State"));
  }
  server.send ( 200, "text/html", "" );
  delay(1);
}

void Forword(){ 

      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
  }

void Backward(){ 

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar);
  }

void Right(){ 

      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar);
  }

void Left(){

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
  }

void ForwordRight(){
      
      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar-speed_low);
 
      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar);
   }

void ForwordLeft(){
      
      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar-speed_low);
  }

void BackRight(){ 

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar-speed_low);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
  }

void BackLeft(){ 

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar-speed_low);
  }

void Stop(){  

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
 }

