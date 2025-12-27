void calibration() {
  for (int i = 0; i < numOfSensor; i++) {
    //setting starting value of max, min array before calibration
    maximum[i] = 0;
    minimum[i] = 1023;
  }
  digitalWrite(led, 1);
  for (int i = 0; i < 20000; i++) {
    for (int i = 0; i < numOfSensor; i++) {
      Mux_Addr(i); 
      delayMicroseconds(10);       
      rawSensor[i] = analogRead(SIG);
      maximum[i] = max(maximum[i], rawSensor[i]);
      minimum[i] = min(minimum[i], rawSensor[i]);
    }
    motor(80, -80);
  }
    motor(0,0);

  for (int i = 0; i < numOfSensor; i++) {
    threshold[i] = (maximum[i] + minimum[i]) / 2;
  }
  digitalWrite(led,0);
  saveCalibration();
  return;
  
}

void saveCalibration() {
  for (int i = 0; i < numOfSensor; i++) {
    EEPROM.update(51 + i, threshold[i] / 4);
  }
  for (int i = 0; i < numOfSensor; i++) {
    EEPROM.update(71 + i, minimum[i] / 4);
  }
  for (int i = 0; i < numOfSensor; i++) {
    EEPROM.update(91 + i, maximum[i] / 4);
  }
  EEPROM.commit(); // Save changes to flash memory
}

// void readCalibration() {
//   for (int i = 0; i < numOfSensor; i++) {
//     threshold[i] = EEPROM.read(51 + i) * 4;
//     Serial.print(threshold[i]);
//     Serial.print(" ");
//   }
//   Serial.println();
//   for (int i = 0; i < numOfSensor; i++) {
//     minimum[i] = EEPROM.read(71 + i) * 4;
//     Serial.print(minimum[i]);
//     Serial.print(" ");
//   }
//   Serial.println();
//   for (int i = 0; i < numOfSensor; i++) {
//     maximum[i] = EEPROM.read(91 + i) * 4;
//     Serial.print(maximum[i]);
//     Serial.print(" ");
//   }
//   Serial.println();
// }

void calibration_display() {
  display.clearDisplay(); // Clear the display at the start
  text(" STARTING ", 40, 2);
  display.display(); // Update the display to show "STARTING"

  // Display loading animation
  for (byte i = 0; i < 38; i++) {
    text(">", 10 + i * 3, 12); // Draw dots with appropriate spacing
    display.display(); // Update the display to show the dots
    delay(25); // Delay for smooth animation
    if (i >= 37) calibration(); // Call calibration when animation is done
    if (push(sw)) break; // Exit loop if button is pressed
  }

  display.clearDisplay(); // Clear the display for showing sensor values

  // Spacing variables
  const int colSpacing = 21; // Horizontal spacing between columns
  const int rowSpacing = 8;  // Vertical spacing between rows

  // Starting positions
  const int startX = 4;  // Starting X position for the first column
  const int startY = 0;  // Starting Y position for the first row

  // Display sensor values in the format:
  // Columns 0-1: MIN, Columns 2-3: MAX, Columns 4-5: MID
  for (byte i = 0; i < 7; i++) {
    // MIN values (Columns 0-1)
    text(String(minimum[i + 7]), startX, startY + i * rowSpacing);  // Row 7 to 1
    text(String(minimum[6 - i]), startX + colSpacing, startY + i * rowSpacing); // Row 7 to 1

    // MAX values (Columns 2-3)
    text(String(maximum[i + 7]), startX + 2 * colSpacing, startY + i * rowSpacing);  // Row 7 to 1
    text(String(maximum[6 - i]), startX + 3 * colSpacing, startY + i * rowSpacing); // Row 7 to 1

    // MID values (Columns 4-5)
    text(String(threshold[i + 7]), startX + 4 * colSpacing, startY + i * rowSpacing);  // Row 7 to 1
    text(String(threshold[6 - i]), startX + 5 * colSpacing, startY + i * rowSpacing); // Row 7 to 1
  }

  display.display(); // Update the display to show all values

  while (!push(sw)); // Wait for button press to exit
}







