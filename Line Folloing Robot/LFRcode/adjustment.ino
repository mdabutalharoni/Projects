String adjustment_list(byte a) {
  if (a == 1)       return "baseSpeed      ";
  else if (a == 2)  return "turn_speed     ";
  else if (a == 3)  return "Kpmin          ";
  else if (a == 4)  return "Kpmax          ";
  else if (a == 5)  return "Kdmin          ";
  else if (a == 6)  return "Kdmax          ";
  else if (a == 7)  return "Ki             ";
  else if (a == 8)  return "turn_brake     ";
  else if (a == 9)  return "node_delay     ";
  else if (a == 10)  return "u_turn_delay   ";
  else if (a == 11)  return "brake_time     ";
  else if (a == 12)  return "stop_time      ";
  else if (a == 13)  return "turn90_delay   ";
  else if (a == 14)  return "i_timer        ";
  else if (a == 15)  return "capac_delay    ";
  else if (a == 16)  return "wall_mid       ";
  else if (a == 17)  return "wallp          ";
  else if (a == 18)  return "wall_limit     ";
  else if (a == 19)  return "wall_distance  ";
  else if (a == 20)  return "obstacle_dis   ";
  else return "";
}

void adjustment() {
  
  byte base = 1, peak = 8, temp = 0, limit = 20;
  int ind = 1;
  bool u_flag = 0, d_flag = 0;
  int value;
  while (1)
  {
    if (temp != ind)
    {
      temp = ind;
      display.clearDisplay();
      for (byte i = 0; i < 8 ; i++)   //change the printing limit here also
      {
        (i == ind - base) ? display.setTextColor(0, 1) : display.setTextColor(1);
        if (i + base >= 5 && i + base <= 6) value = EEPROM.read(i + base + 19) * 100;
        else if (i + base >= 7 && i + base <= 15) value = EEPROM.read(i + base + 19) * 10;
        else value = EEPROM.read(i + base + 19);
        text(adjustment_list(i + base) + String(value) + " ", 0, i*8);
      }
      display.display();
    }

    if (!digitalRead(up))
    {
      u_flag = cont_push(up, u_flag);
      ind--;
      if (ind < 1)
      {
        ind = limit;
        peak = ind;
        base = peak - 7;
      }
      else if (ind < base)
      {
        base = ind;
        peak = base + 7;
      }
    }
    else u_flag = 0;

    if (!digitalRead(down))
    {
      d_flag = cont_push(down, d_flag);
      ind++;
      if (ind > limit)
      {
        ind = 1;
        base = ind;
        peak = base + 7;
      }
      else if (ind > peak)
      {
        peak = ind;
        base = peak - 7;
      }
    }
    else d_flag = 0;

    //this portion is for select button, to enter a menu item..
    //returns the indicated menu number.which is used above to enter the menu item
    byte r = push(sw);
    if (r) {
      if (r == 1)
      {
        set_adjust(ind);
        temp = 0;
      }
      else
      {
        eeprom_refresh();   //this is a must
        return;
      }
      temp = 0;
    }
  }
}

void set_adjust(int indicator){
  display.setTextColor(1);
  display.clearDisplay();
  int value, limit;
  byte temp = value + 1;
  
  if(indicator>=3 && indicator<=4){
    value = EEPROM.read(indicator + 19);
    limit = 25;
  }
  else if(indicator>=5 && indicator<=6) {
    value = EEPROM.read(indicator + 19)*100;
    limit = 10000;
  }
  else if(indicator>=7 && indicator<=15) {
    value = EEPROM.read(indicator + 19)*10;
    limit = 2500;
  }
  else {
    value = EEPROM.read(indicator + 19);
    limit = 255;
  }

  bool u_flag = value, d_flag = 0;
  while (1)
  {
    if (temp != value)
    {
      temp = value;
      display.clearDisplay();
      if(indicator == 1)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 2)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 3)text(adjustment_list(indicator)+ "\n " + String((float)value), 0, 0);
      else if(indicator == 4)text(adjustment_list(indicator)+ "\n " + String((float)value), 0, 0);
      else if(indicator == 5)text(adjustment_list(indicator)+ "\n " + String((float)value), 0, 0);
      else if(indicator == 6)text(adjustment_list(indicator)+ "\n " + String((float)value), 0, 0);
      else if(indicator == 7)text(adjustment_list(indicator)+ "\n " + String((float)value), 0, 0);
      else if(indicator == 8)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 9)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 10)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 11)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 12)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 13)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 14)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 15)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 16)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 17)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 18)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 19)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else if(indicator == 20)text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      else text(adjustment_list(indicator)+ "\n " + String(value), 0, 0);
      display.display();
    }

    if (!digitalRead(up))
    {
      u_flag = cont_push(up, u_flag);
      if(indicator == 1 || indicator == 2) value +=5;
      else if(indicator >= 5 && indicator <= 6) value += 100;
      else if(indicator >= 7 && indicator <= 15) value += 10;
      else value++;
      if (value > limit) value = limit;
      
    }

    if (!digitalRead(down))
    {
      d_flag = cont_push(down, d_flag);
      if(indicator == 1 || indicator == 2) value -=5;
      else if(indicator >= 5 && indicator <= 6) value -= 100;
      else if(indicator >= 7 && indicator <= 15) value -= 10;
      else value--;
      if (value < 0) value = 0;
    }

    byte r = push(sw);
    if (r)
    {
      if (r == 1)
      {
        // Save value to EEPROM
        if(indicator>=1 && indicator<=4) EEPROM.write(19 + indicator, value);
        else if(indicator>=5 && indicator<=6) EEPROM.write(19 + indicator, value/100);
        else if(indicator >= 7 && indicator <= 15) EEPROM.write(19 + indicator, value/10);
        else EEPROM.write(19 + indicator, value);  // Use `write` instead of `update`
        EEPROM.commit();                      // Commit changes to flash
        delay(10);
        //Serial.println("Saved value: " + String(value)); // Debugging
        return;
      }
      else
      {
        EEPROM.commit();  // Ensure commit even on cancel
        return;
      }
    }
  }
}
