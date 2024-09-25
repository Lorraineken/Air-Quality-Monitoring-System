/**
 * pms5003Test.ino:Exercises all the methods of the PMS5003 class 
 *
 * Copyright (c) U. Raich, January 2024
 * This program is part of the IoT4AQ workshop at the 
 * university Alioune Diop, Bambey, Sénégal, 2024
 * It is released under the MIT license
 */

#include <PMS5003.h>
PMS5003 pms5003 = PMS5003(34,33); // initialize the PMS5003 class

void setup() {
  Serial.begin(115200);
  if (!Serial)
    delay(10);
}

void loop() {
  uint8_t *rawData;
  pms5003Data results;
  pms5003Data *resultsPtr;
  // read the raw data
  rawData = pms5003.readRaw();
  // print them
  Serial.println("Result of pms5003.printMsg:");
  pms5003.printRaw(rawData);
  // extract the physical values from the raw data
  resultsPtr = pms5003.evaluate(rawData);
  // print the results
  Serial.println("Result from mps5003.printMsg:"); 
  pms5003.printMsg(resultsPtr);
  // print the calculated and the read checksums
  Serial.print("Calculated checksum: 0x");
  Serial.print(pms5003.calcChecksum(rawData),HEX);
  Serial.print(", checksum read from message: 0x");
  Serial.println(pms5003.readChecksum(rawData),HEX);
  // verify checksum
  if (pms5003.verifyChecksum(rawData))
    Serial.println("Checksum is correct");
  else
    Serial.println("Checksum error");
  // get the filled pms5003Data structure and print
  // the first 3 dust concentration values: pm1.0, pm2.5, pm10
  results = pms5003.readMeas();
  Serial.print("pm 1.0: ");
  Serial.println(results.pm1_0);
  Serial.print("pm 2.5: ");
  Serial.println(results.pm2_5);
  Serial.print("pm 10: ");
  Serial.println(results.pm10);
  Serial.println();
  delay(2000);
}
  
