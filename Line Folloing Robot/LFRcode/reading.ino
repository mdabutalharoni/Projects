void reading(){
  deciValue = 0;
  posValue = 0;
  sum = 0;
  for (int i = 0; i < numOfSensor; i++)
  {
    Mux_Addr(i);
    delayMicroseconds(30);
    rawSensor[i] = analogRead(SIG);
    rawSensor[i] = map(rawSensor[i], minimum[i], maximum[i], 0, 1400);
    if(rawSensor[i]>1023) rawSensor[i] = 1023;
    if(rawSensor[i]<0) rawSensor[i] = 0;

    if(i_mode) rawSensor[i] = 1023 - rawSensor[i];
    
    (rawSensor[i] > threshold[i])? rawSensor[i] = 1 : rawSensor[i] = 0;
    deciValue = deciValue + rawSensor[i]*base[i]; 
    posValue = posValue + rawSensor[i]*position[i];
    sum = sum + rawSensor[i];
  }
  if(sum) avg = (float)posValue / sum;

}

void Mux_Addr(int ch_Addr) {
  if (channel_adress_table[ch_Addr][0]) {
    digitalWrite(S3, 1);
  } else {
    digitalWrite(S3, 0);
  }

  if (channel_adress_table[ch_Addr][1]) {
    digitalWrite(S2, 1);
  } else {
    digitalWrite(S2, 0);
  }

  if (channel_adress_table[ch_Addr][2]) {
    digitalWrite(S1, 1);
  } else {
    digitalWrite(S1, 0);
  }

  if (channel_adress_table[ch_Addr][3]) {
    digitalWrite(S0, 1);
  } else {
    digitalWrite(S0, 0);
  }
}

void analog_reading() {
  while (1) {
    for (int i = 13 ; i >=0 ; i--) {
      Mux_Addr(i);
      delayMicroseconds(10);
      rawSensor[i] = analogRead(SIG);
      Serial.print(String(rawSensor[i]) + " ");
    }
    Serial.println();
  }
}

void digital_reading() {
  while (1) {
    reading();
    for (int i = 13 ; i >= 0 ; i--)
    Serial.print(String(rawSensor[i]) + " ");
    Serial.println(deciValue);
  }
}

void analog_display() {
  const int centerX = 64;  // Center X (middle of the display width)
  const int centerY = 58;  // Center Y (near the bottom of the display height)
  const int radius = 50;   // Adjusted radius to fit text
  const int leftSensors = 6;  // Sensors on the left-hand side (1 to 6)
  const int rightSensors = 6; // Sensors on the right-hand side (9 to 14)
  const float spacingFactor = 1.2; // Factor to increase row spacing
  const int rowSpacing = (radius / max(leftSensors, rightSensors)) * spacingFactor;

  display.clearDisplay();
  while (!push(sw)) {
    // Read sensor values
    for (int i = 0; i < numOfSensor; i++) {
      Mux_Addr(i);
      delayMicroseconds(50);
      rawSensor[i] = analogRead(SIG);
    }

    display.clearDisplay(); // Clear the display for new rendering

    // Display sensors 7 and 8 at the top center
    text(String(rawSensor[6]), centerX + 8, 0); // Sensor 7 slightly left of center
    text(String(rawSensor[7]), centerX - 16, 0);  // Sensor 8 slightly right of center

    // Calculate positions for sensors 1 to 6 (left-hand side) and mirror for 9 to 14
    for (int i = 0; i < leftSensors; i++) {
      int y = centerY - i * rowSpacing; // Adjust vertical position
      int xLeft = centerX - sqrt(radius * radius - (centerY - y) * (centerY - y)); // Calculate x for left
      int xRight = centerX + (centerX - xLeft); // Mirror x for right

      // Display sensor values
      text(String(rawSensor[13-i]), xLeft - 8, y - 4);         // Left-hand sensor
      text(String(rawSensor[i]), xRight - 8, y - 4);    // Right-hand sensor
    }

    display.display(); // Update the display
    delay(100);
  }
}

// for curve sensor
// void digital_display() {
//   const int centerX = 64;  // Center X (middle of the display width)
//   const int centerY = 58;  // Center Y (near the bottom of the display height)
//   const int radius = 50;   // Radius for the half-circle
//   const int numSensors = numOfSensor; // Total number of sensors

//   display.clearDisplay();
//   while (!push(sw)) {
//     // Read sensor values
//     reading();
//     display.clearDisplay(); // Clear the display for new rendering

//     // Calculate positions along the half-circle
//     for (int i = 0; i < numSensors; i++) {
//       // Compute angle for each sensor
//       float angle = PI - (i * (PI / (numSensors - 1))); // From PI to 0 radians (180° to 0°)
//       int x = centerX + radius * cos(angle);            // X coordinate
//       int y = centerY - radius * sin(angle);            // Y coordinate

//       // Display binary digit at the calculated position
//       //int binaryValue = rawSensor[i] > 512 ? 1 : 0; // Convert analog reading to binary (adjust threshold as needed)
//       text(String(rawSensor[13-i]), x - 3, y - 3);     // Offset text slightly for centering
//     }

//     display.display(); // Update the display
//     delay(100);        // Delay for better visibility

//   }
// }

void digital_display() {
  const int numSensors = numOfSensor; // Total number of sensors
  const int spacing = 128 / numSensors; // Equal spacing between sensor values
  const int textY = 0; // Y-coordinate for the text (1st row)
  display.clearDisplay();

  while (!push(sw)) {
    // Read sensor values
    reading();
    display.clearDisplay();
    for (int i = 0; i < numSensors; i++) {
      int x = 128 - (i + 1) * spacing; // Calculate X position starting from the right
      display.setCursor(x, textY); // Set cursor position
      display.print(rawSensor[i]); // Print binary value
    }
    display.setCursor(8, 32);
    display.print(sonarl.ping_cm());
    display.setCursor(56, 32);
    display.print(sonarf.ping_cm());
    display.setCursor(110, 32);
    display.print(sonarr.ping_cm());

    display.display(); // Update the display
    delay(100); // Delay for better visibility
  }
}













