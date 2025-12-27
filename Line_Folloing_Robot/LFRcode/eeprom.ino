void eeprom_refresh(){
  for (int i = 0; i < numOfSensor; i++) {
    threshold[i] = EEPROM.read(51 + i) * 4;
  }
  for (int i = 0; i < numOfSensor; i++) {
    minimum[i] = EEPROM.read(71 + i) * 4;
  }
  for (int i = 0; i < numOfSensor; i++) {
    maximum[i] = EEPROM.read(91 + i) * 4;
  }

  baseSpeed = EEPROM.read(20);
  turn_speed = EEPROM.read(21);
  Kpmin = EEPROM.read(22);
  Kpmax = EEPROM.read(23);
  Kdmin = EEPROM.read(24)*100;
  Kdmax = EEPROM.read(25)*100;
  Ki = EEPROM.read(26);
  turn_brake = EEPROM.read(27)*10;
  node_delay = EEPROM.read(28)*10;
  u_turn_delay = EEPROM.read(29)*10;
  brake_time = EEPROM.read(30)*10;
  stop_time = EEPROM.read(31)*10;
  turn90_delay = EEPROM.read(32)*10;
  i_timer = EEPROM.read(33)*10;
  capacitor_delay = EEPROM.read(34)*10;
  wall_mid = EEPROM.read(35);
  wallp = EEPROM.read(36);
  wall_limit = EEPROM.read(37);
  wall_distance = EEPROM.read(38);
  obstacle_distance = EEPROM.read(39);
  
}