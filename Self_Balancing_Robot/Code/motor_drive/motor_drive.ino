#define lms 32
#define lmf 33
#define lmb 25

#define rmf 26
#define rmb 27
#define rms 14

void setup(){
  Serial.begin(115200);

  pinMode(lmf, OUTPUT);
  pinMode(lmb, OUTPUT);
  pinMode(rmf, OUTPUT);
  pinMode(rmb, OUTPUT);
   //run all the motor forward at full speed
   
  ledcSetup(1, 1000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 1000, 8);

  ledcAttachPin(lms, 1); // assign RGB led pins to channels
  ledcAttachPin(rms, 2);
}

void loop(){
  motor(-200, 200);
}
