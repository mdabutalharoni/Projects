#define LF 5  // Left Motor Forward
#define LB 6  // Left Motor Backward
#define RF 9  // Right Motor Forward
#define RB 10  // Right Motor Backward



void setup() {
  // put your setup code here, to run once:
  pinMode(LF, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(RB, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(LF, 100);
  analogWrite(LB, 0);
  analogWrite(RF, 100);
  analogWrite(RB, 0);

  delay(2000);

  analogWrite(LF, 0);
  analogWrite(LB, 0);
  analogWrite(RF, 0);
  analogWrite(RB, 0);

  delay(2000);

  analogWrite(LF, 0);
  analogWrite(LB, 100);
  analogWrite(RF, 0);
  analogWrite(RB, 100);

  delay(2000);

  analogWrite(LF, 0);
  analogWrite(LB, 0);
  analogWrite(RF, 0);
  analogWrite(RB, 0);

  delay(2000);

  // analogWrite(LF, 100);
  // analogWrite(LB, 0);
  // analogWrite(RF, 100);
  // analogWrite(RB, 0);
  // delay(2000);
}
