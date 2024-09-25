/**
 * PlanTowerSimple.ino: Reads the PlanTower PMS5003 using the ESP32_PMS5003
 * library
 * It uses the library function readMeas() which returns a data structure
 * containing the results
 *
 * Copyright (c)  Uli Raich, January 2024
 * This program is part of the IoT4AQ workshop
 * held at the university Alioune Diop, Bambey, Sénégal
 * 
 * License: MIT
 */

#include <PMS5003.h>

PMS5003 pms5003 = PMS5003();

void setup() {
  Serial.begin(115200);
  if (!Serial)
    delay(10);
}

void loop() {
  pms5003Data results;
  results = pms5003.readMeas();
  Serial.print("pm 1.0 concentration: ");
  Serial.println(results.pm1_0);
  Serial.print("pm 2.5 concentration: ");
  Serial.println(results.pm2_5);
  Serial.print("pm 10 concentration: ");
  Serial.println(results.pm10);
  Serial.println();
}
