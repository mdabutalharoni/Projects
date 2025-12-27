bool sonarf_read(bool mode) {
  if (tf - millis() > 50 || mode) {
    tf = millis();
    sf = sonarf.ping_cm();
    if (sf <= 2) sl = 50;
  }
  if (sf < obstacle_distance) return 1;
  else return 0;
}

bool wallf_read(bool mode) {
  if (tf - millis() > 50 || mode) {
    tf = millis();
    sf = sonarf.ping_cm();
    if (sf <= 2) sf = 50;
  }
  if (sf < wall_distance) return 1;
  else return 0;
}

bool sonarl_read(bool mode) {
  if (tl - millis() > 50 || mode) {
    tl = millis();
    sl = sonarl.ping_cm();
    if (sl <= 2) sl = 50;
  }
  if (sl < wall_limit) return 1;
  else return 0;
}

bool sonarr_read(bool mode) {
  if (tr - millis() > 50 || mode) {
    tr = millis();
    sr = sonarr.ping_cm();
    if (sr <= 2) sr = 50;
  }
  if (sr < wall_limit) return 1;
  else return 0;
}

void wall_follow() {
  while (!sum) {
    reading();
    while (sonarr_read(1)) {
      int sonar_error = (wall_mid - sr) * wallp;
      int lm = 50 - sonar_error;
      int rm = 50 + sonar_error;
      motor(lm, rm);
      // if (abs(sonar_error) < 2) motor(lm, rm);
      // else motor(50, 50);
    }
    while (sonarl_read(1)) {
      int sonar_error = (wall_mid - sl) * wallp;
      int lm = 50 + sonar_error;
      int rm = 50 - sonar_error;
      motor(lm, rm);
      // if (abs(sonar_error) < 2) motor(lm, rm);
      // else motor(50, 50);
    }
    motor(60, 60);
  }
  
  m1 = millis();
}

void obstacle(char s){
  brake();
  digitalWrite(led, HIGH);
  turn_90(s);
  reading();
  (s == 'l') ? motor(baseSpeed , baseSpeed/2) : motor(baseSpeed/2 , baseSpeed);
  while (!sum) reading();
  while (sum) reading();
  delay(node_delay);
  motor(0, 0);
  delay(100);
  turn_90(s);
  digitalWrite(led, LOW);
  reading();
  m1 = millis();
}
