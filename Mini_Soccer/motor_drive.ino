void controlMotors(int leftMotor, int rightMotor) {
  leftMotor = constrain(leftMotor, -255, 255);
  rightMotor = constrain(rightMotor, -255, 255);

  int pwmLeft = abs(leftMotor);
  int pwmRight = abs(rightMotor);

  analogWrite(ENA, pwmLeft);
  analogWrite(ENB, pwmRight);

  // Left Motor Control
  if (leftMotor > 5) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (leftMotor < -5) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }

  // Right Motor Control
  if (rightMotor > 5) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (rightMotor < -5) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
}