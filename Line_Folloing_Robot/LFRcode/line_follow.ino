void line_follow() {
  while (!push(sw)) {
a:
    reading();
    if (sum == 0) {
      if (turn != 's') {
        delay(30);
        brake();
        //Serial.println("Left/Right Turn Executign");
        (turn == 'l') ? motor(-turn_speed, turn_speed) : motor(turn_speed, -turn_speed);
        while (sum) reading();
        while (rawSensor[5] == 0 && rawSensor[6] == 0 && rawSensor[7] == 0 && rawSensor[8] == 0) reading();
        motor(0, 0);
        delay(30);
        //(turn == 'r') ? motor(-turn_speed, turn_speed) : motor(turn_speed, -turn_speed);
        // while (rawSensor[6] == 0 && rawSensor[7] == 0) reading();
        // (turn == 'r') ? motor(-turn_speed, turn_speed) : motor(turn_speed, -turn_speed);
        //delay(turn_brake);
        turn = 's';
        cross = 's';
      } else{
        m2 = millis();
        while (sum == 0) {
          reading();
          if (millis() - m2 > u_turn_delay) {
            //Serial.println("U Turn");
            motor(0, 0);
            delay(100);
            (side == 'l') ? motor(-turn_speed, turn_speed) : motor(turn_speed, -turn_speed);
            while (rawSensor[6] == 0 && rawSensor[7] == 0) reading();
            (side == 'r') ? motor(-turn_speed, turn_speed) : motor(turn_speed, -turn_speed);
            delay(turn_brake);
            break;
          }
        }
      }
    }

    else if (sum == 1 || sum == 2 || sum == 3) {
      if (sonarf.ping_cm() < obstacle_distance && sonarf.ping_cm() > 5) obstacle('r');
      if (cross != 's') {
        brake();
        (cross == 'l') ? motor(-turn_speed, turn_speed) : motor(turn_speed, -turn_speed);
        while (sum) reading();
        while (rawSensor[5] == 0 && rawSensor[6] == 0 && rawSensor[7] == 0 && rawSensor[8] == 0) reading();
        motor(0, 0);
        delay(30);
        // while (sum > 1) reading();
        // while (rawSensor[6] == 0 && rawSensor[7] == 0) reading();
        // (cross == 'r') ? motor(-turn_speed, turn_speed) : motor(turn_speed, -turn_speed);
        // delay(turn_brake);
        cross = 's';
        turn = 's';
      } else {
        reading();
        //Serial.println("Straight");
        // Serial.println(avg);
        Ap = Kpmax - Kpmin;
        Ad = Kdmax - Kdmin;
        error = 6.5 - avg;

        Kp = Kpmin + Ap * abs(error) / 6.5;
        Kd = Kdmin + Ad * abs(error) / 6.5;
        PID = error * Kp + (error - last_error) * Kd + (error + last_error) * Ki;
        last_error = error;
        lmotor = baseSpeed + PID;
        rmotor = baseSpeed - PID;
        int maxSpeed = 1.5 * baseSpeed;
        if (maxSpeed > 255) maxSpeed = 255;

        lmotor = constrain(lmotor, -30, maxSpeed);
        rmotor = constrain(rmotor, -30, maxSpeed);
        // Serial.print(lmotor);
        // Serial.print(" ");
        // Serial.println(rmotor);
        motor(lmotor, rmotor);
      }
    } else if (sum >= 4 && sum <= 9) {
      if (rawSensor[0] == 0 && rawSensor[12] == 1 && rawSensor[13] == 1 && rawSensor[6] + rawSensor[7] > 0) {
        //Serial.println("Left Turn");
        turn = 'l';
        // else{
        // turn = 'l';
        while (rawSensor[0] == 0 && rawSensor[12] == 1 && rawSensor[13] == 1 && rawSensor[6] + rawSensor[7] > 0) reading();
        if (rawSensor[0] == 0 && rawSensor[6] + rawSensor[7] > 0) {
          delay(node_delay);
          reading();
          if (sum != 0) {
            //Serial.println("Left Hand");
            if (side == 'l') cross = 'l';
          }
        }
        // }
      } else if (rawSensor[13] == 0 && rawSensor[0] == 1 && rawSensor[1] == 1 && rawSensor[6] + rawSensor[7] > 0) {
        //Serial.println("Right Turn");
        turn = 'r';

        // else {
        //   turn = 'r';
        while (rawSensor[13] == 0 && rawSensor[0] == 1 && rawSensor[1] == 1 && rawSensor[6] + rawSensor[7] > 0) reading();
        if (rawSensor[13] == 0 && rawSensor[6] + rawSensor[7] > 0) {
          delay(node_delay);
          reading();
          if (sum != 0) {
            //Serial.println("Right Hand");
            if (side == 'r') cross = 'r';
          }
        }
        // }
      }
      else if((rawSensor[4] || rawSensor[5]) && (rawSensor[8] || rawSensor[9]) && (!rawSensor[6] || !rawSensor[7])){
        turn = side;
        //Serial.println(turn);
        delay(capacitor_delay);
        if(side == 'l')
        reading();
      }
      m1 = millis();
    } else if (sum > 9) {
      turn = side;
      m2 = millis();
      if (rawSensor[0] && rawSensor[13] && (!rawSensor[6] || !rawSensor[7])) {  //inverse line detection
        delay(30);
        reading();
        if (rawSensor[0] && rawSensor[13] && (!rawSensor[6] || !rawSensor[7])) {
          i_mode = !i_mode;
          digitalWrite(led, i_mode);
        }
        //turn = side;  //specific guided turn for Y section
      }

      
      else if (rawSensor[0] && rawSensor[13] && sum > 13 && i_mode == 0) {
        while (sum > 9) {
          reading();
          if (millis() - m2 > stop_time) {
            //Serial.println("Stop");
            motor(0, 0);
            while (sum > 9) {
              reading();
              if (push(sw)) return;
            }
            goto a;
          }
        }
      }
      delay(node_delay);
      reading();
      if (sum != 0) {
        //Serial.println("Cross");
        if (!rawSensor[0] && !rawSensor[13] && (rawSensor[6] || rawSensor[7])) cross = 's';
      } else {
        //Serial.println("T");
        // delay(capacitor_delay);
        // reading();
        // if(rawSensor[0] == 1 || rawSensor[13] == 1){
        //   turn = 's';
        //   delay(node_delay);
        // }
        turn = 'r';  //put this line only in else loop if no capacitor given
      }
      m1 = millis();
    }
    if (millis() - m1 > 500) turn = 's';
  }
}