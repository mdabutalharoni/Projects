void motor(int left, int right) {
  // Constrain the values of left and right to the range -255 to 255
  left = constrain(left, -255, 255);
  right = constrain(right, -255, 255);

  // Handle the left motor
  if (left > 0) {
    analogWrite(LF, left); // Forward
    analogWrite(LB, 0);    // Stop reverse
  } else {
    analogWrite(LF, 0);    // Stop forward
    analogWrite(LB, -left); // Reverse
  }

  // Handle the right motor
  if (right > 0) {
    analogWrite(RF, right); // Forward
    analogWrite(RB, 0);     // Stop reverse
  } else {
    analogWrite(RF, 0);     // Stop forward
    analogWrite(RB, -right); // Reverse
  }
}

void brake() {
  // motor(-100, -100);
  // delay(brake_time);
  motor(0, 0);
  delay(brake_time);
}

void turn_90(char s) {
  (s == 'l') ? motor(-turn_speed, turn_speed) : motor(turn_speed, -turn_speed);
  delay(turn90_delay);  //this should be adjusted
  (s == 'r') ? motor(-turn_speed, turn_speed) : motor(turn_speed, -turn_speed);
  delay(turn_brake);
  motor(0, 0);
  delay(100);
}
