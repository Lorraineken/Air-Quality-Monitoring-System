/**
 * pms5003Raw.ino: Reads the PMS5003 protocol and prints its 32 raw
 * byte values
 *
 * Copyright (c) U. Raich, January 2024
 * This program is part of the IoT4AQ workshop at the 
 * university Alioune Diop, Bambey, Sénégal, 2024
 * It is released under the MIT license
 */

#include <PMS5003.h>
PMS5003 pms5003 = PMS5003(); // initialize the PMS5003 class

void setup() {
  Serial.begin(115200);
  if (!Serial)
    delay(10);
}

void loop() {
  uint8_t *rawData;
  rawData = pms5003.readRaw();
  pms5003.printRaw(rawData);
}
  
