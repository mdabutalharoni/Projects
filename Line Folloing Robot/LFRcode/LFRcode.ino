#include <Wire.h>
#include <NewPing.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire, -1);
//#include <math.h>
#include <EEPROM.h>
#define EEPROM_SIZE 512

NewPing sonarl(16, 16, 40);
NewPing sonarf(17, 17, 40);
NewPing sonarr(22, 22, 40);

#define S0 18
#define S1 19
#define S2 20
#define S3 21
#define SIG 28

#define up 2
#define sw 3
#define down 4

#define LF  12  // Replace with your actual PWM pin
#define LB  14  
#define RF  15  // Replace with your actual PWM pin
#define RB  13

#define led LED_BUILTIN

byte channel_adress_table[14][4] = {
  { 0, 0, 0, 0 },  //0
  { 0, 0, 0, 1 },  //1
  { 0, 0, 1, 0 },  //2
  { 0, 0, 1, 1 },  //3
  { 0, 1, 0, 0 },  //4
  { 0, 1, 0, 1 },  //5
  { 0, 1, 1, 0 },  //6
  { 0, 1, 1, 1 },  //7
  { 1, 0, 0, 0 },  //8
  { 1, 0, 0, 1 },  //9
  { 1, 0, 1, 0 },  //10
  { 1, 0, 1, 1 },  //11
  { 1, 1, 0, 0 },  //12
  { 1, 1, 0, 1 },  //13
};

int turn_speed, turn_brake, u_turn_delay, baseSpeed, node_delay, brake_time, stop_time, turn90_delay, i_timer, capacitor_delay;
int wall_mid, wallp, wall_limit, wall_distance, obstacle_distance;//18,22
int sonar_error;
int sl, sr, sf;
uint32_t tl, tr, tf;
int crossCount = 0;

bool i_mode = 0;


char side = 'l';
char turn = 's';
char cross = 's';

uint32_t m1, m2;


#define numOfSensor 14
int rawSensor[numOfSensor], maximum[numOfSensor], minimum[numOfSensor], threshold[14];
unsigned int base[16] = {
  1, 2, 4, 8, 16, 32, 64, 128, 
  256, 512, 1024, 2048, 4096, 
  (1 << 13)};
int deciValue;
int sum = 0;

int position[14] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13};
int lmotor, rmotor;
float Kp, Ap, Kpmin, Kpmax, Kd, Ad, Kdmin, Kdmax, Ki;
float PID;
int posValue;
float error, last_error;
float avg;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  unsigned long startTime = millis();
  while (!Serial && (millis() - startTime) < 5000) {
    ; // Wait
  }
  delay(100);
  //Serial.println("Starting setup...");
  EEPROM.begin(EEPROM_SIZE); // For Pi Pico, initializes EEPROM emulation
  //Serial.println("EEPROM initialized successfully.");

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(SIG, INPUT);

  pinMode(LF, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(RB, OUTPUT);

  pinMode(up, INPUT_PULLUP);
  pinMode(sw, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(led, OUTPUT);

  Wire.setSDA(0); // Set SDA to GPIO0
  Wire.setSCL(1); // Set SCL to GPIO1
  Wire.begin();
  Wire.setClock(400000L);
  display.begin(0x02, 0x3C);
  display.setTextColor(1);
  display.setRotation(0);
  display.setTextSize(1);

  REF();
  //readCalibration();
  eeprom_refresh();
  
}

void loop() {
  
  byte r = push(sw);
  if(r){
    if(r == 1){
      r = menu();
      if(r){
        if(r == 1) 
        {
          display.clearDisplay();
          text("Running", 43, 32);
          display.display();
          line_follow();
        }
        else if(r == 2) adjustment();
        else if(r == 3) calibration_display();
        else if(r == 4) analog_display();
        else if(r == 5) digital_display();
        else if(r == 6) {
          motor(baseSpeed, baseSpeed);
          while (true){
            if(push(sw)){
              motor(0, 0);
              break;
            }
          }
        }
        else if(r = 7){
          turn_90('r');
        } 
      }
    }
    REF();
  }
  // Serial.print("baseSpeed: ");
  // Serial.println(baseSpeed);
  // Serial.print("turn_speed: ");
  // Serial.println(turn_speed);
  // Serial.print("Kp: ");
  // Serial.println(Kp);
  // Serial.print("Kd: ");
  // Serial.println(Kd);
  // Serial.print("Ki: ");
  // Serial.println(Ki);
  // Serial.print("turn_brake: ");
  // Serial.println(turn_brake);
  // Serial.print("node_delay: ");
  // Serial.println(node_delay);
  // Serial.print("u_turn_delay: ");
  // Serial.println(u_turn_delay);
  // Serial.print("brake_time: ");
  // Serial.println(brake_time);
  // Serial.print("stop_time: ");
  // Serial.println(stop_time);
  // delay(200);
}

