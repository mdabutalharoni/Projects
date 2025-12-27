String menu_list(byte a) {
  if (a == 1)       return "Line Follow          ";
  else if (a == 2)  return "Adjust               ";
  else if (a == 3)  return "Calibration          ";
  else if (a == 4)  return "Analog Reading       ";
  else if (a == 5)  return "Digital Reading      ";
  else if (a == 6)  return "Motor Test           ";
  else if (a == 7)  return "Rotation Test        ";
  else if (a == 8)  return "Empty                ";
  else if (a == 9)  return "Empty                ";
  else if (a == 10)  return "Empty               ";
  else if (a == 11)  return "Empty               ";
  else return "";
}

byte menu() {
  
  byte base = 1, peak = 8, temp = 0, limit = 11;
  int ind = 1;
  bool u_flag = 0, d_flag = 0;
  while (1)
  {
    if (temp != ind)
    {
      temp = ind;
      display.clearDisplay();
      for (byte i = 0; i < 8 ; i++)   //change the printing limit here also
      {
        (i == ind - base) ? display.setTextColor(0, 1) : display.setTextColor(1);
        text(menu_list(i + base), 0, i * 8);
        //          if(i == ind - base)
        //          {
        //            text(menu_list(i + base), 0, i * 8);
        //            display.display();
        //          }
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
    if (r)
    {
      if (r == 1) return ind;
      else return 0;
    }
  }
}