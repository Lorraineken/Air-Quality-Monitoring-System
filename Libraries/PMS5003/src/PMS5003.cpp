/**
 * PMS5003.cpp
 * Library for reading the PlanTower PMS5003 dust sensor
 *
 * Author: Uli Raich
 * Version: 1.0
 * License: MIT
 */

#include "PMS5003.h"

/**
 * Constructor for the PMS5003 class.
 * Initializes ESP32 UART2, connected to Rx: GPIO16, Tx: GPIO17
 *
 */
PMS5003::PMS5003(int pms_rx, int pms_tx)
{
  pms5003Msg[0] = firstChar;
  pms5003Msg[1] = secondChar;
  rx = pms_rx;
  tx = pms_tx;
  
}

  /**
   * Reads raw data from the PMS5003 protocol
   * The PMS5003 sends a packet of measurement data every second
   * This packet consists of 32 uint8_t values
   *
   * @return: Returns a pointer to the raw data 
   */

uint8_t *PMS5003::readRaw()
{
  Serial2.begin(9600,SERIAL_8N1,rx,tx);
  uint8_t inChar=0;

  pms5003MsgPtr = &pms5003Msg[2];
  
  // synchronize by making sure that the first two characters are "BM"
  while (true) 
    if (Serial2.available()) {
      inChar = (uint8_t)Serial2.read();
      if (inChar == firstChar)
        break;
    }

  while (true) 
    if (Serial2.available()) {
      inChar = (uint8_t)Serial2.read();      
      if (inChar == secondChar)
        break;
    }
  
  // now that we have read the correct header, let's read the rest
  
  for (int i=2;i<sizeof(pms5003Msg);i++) {
    while (!Serial2.available()); // wait until we see the next data
    *pms5003MsgPtr++ = (uint8_t) Serial2.read();
  }
  
  /*
  // print the raw data
  pms5003MsgPtr = pms5003Msg;
  for (int i=0;i<sizeof(pms5003Msg);i++) {
    Serial.print("0x");
    Serial.print(*pms5003MsgPtr++,HEX);
    Serial.print(" ");
  }
  Serial.println();
  */
  Serial2.end();
  return PMS5003::pms5003Msg;
}

void PMS5003::printRaw(uint8_t *msgPtr) {
  for (int i=0;i<PMS5003_MSG_LENGTH;i++) {
    Serial.print("0x");
    Serial.print(*msgPtr++,HEX);
    Serial.print(" ");
  }
  Serial.println();
}

pms5003Data *PMS5003::evaluate(uint8_t *msgPtr) {
  pms5003Result.header[0]   = *msgPtr++;
  pms5003Result.header[1]   = *msgPtr++;
  pms5003Result.framelength = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.pm1_0       = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.pm2_5       = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.pm10        = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.pm1_0_atm   = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.pm2_5_atm   = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.pm10_atm    = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.nb_0_3      = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.nb_0_5      = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.nb_1        = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.nb_2_5      = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.nb_5        = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.nb_10       = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.reserved    = (*msgPtr++ << 8) | *msgPtr++;
  pms5003Result.checksum    = (*msgPtr++ << 8) | *msgPtr;
  return &pms5003Result;
}

void PMS5003::printMsg(pms5003Data *resultPtr) {
  Serial.println("**********************************************");
  Serial.println("* PMS5003 message                            *");
  Serial.println("**********************************************");

  Serial.print("Header: ");
  Serial.print(resultPtr->header[0]);
  Serial.println(resultPtr->header[1]);

  // PM values
  Serial.print("pm1.0:     ");
  Serial.println(resultPtr->pm1_0);
  Serial.print("pm2.5:     ");
  Serial.println(resultPtr->pm2_5);
  Serial.print("pm10:      ");  
  Serial.println(resultPtr->pm10);
  Serial.println();

  // PM values under atmospheric pressure
  Serial.println("pm_x under atmospheric pressure");
  Serial.print("pm1.0 atm:  ");
  Serial.println(resultPtr->pm1_0_atm);
  Serial.print("pm2.5 atm:  ");
  Serial.println(resultPtr->pm2_5_atm);
  Serial.print("pm10 atm:   ");
  Serial.println(resultPtr->pm10_atm);
  Serial.println();
  
  // number of dust particles in 1L volume
  Serial.println("Number of dust particles in 1L volume");
  Serial.print("nb above 0.3 um: ");
  Serial.println(resultPtr->nb_0_3);
  Serial.print("nb above 0.5 um: ");
  Serial.println(resultPtr->nb_0_5);
  Serial.print("nb above 1 um:   ");
  Serial.println(resultPtr->nb_1);
  Serial.print("nb above 2.5 um: ");
  Serial.println(resultPtr->nb_2_5);
  Serial.print("nb above 5 um:   ");
  Serial.println(resultPtr->nb_5);
  Serial.print("nb above 10 um:  ");
  Serial.println(resultPtr->nb_10);
  Serial.println();
  Serial.print("reserved: ");
  Serial.println(resultPtr->reserved);
  Serial.print("checksum: 0x");
  Serial.println(resultPtr->checksum,HEX);
  Serial.println();
}

int PMS5003::calcChecksum(uint8_t *msgPtr) {
  int checksum = 0;
  for (int i=0;i<PMS5003_MSG_LENGTH-2;i++)
    checksum += *msgPtr++;
  return checksum;
}

int PMS5003::readChecksum(uint8_t *msgPtr) {
  msgPtr +=30;
  return (*msgPtr++ << 8) | *msgPtr;
}

bool PMS5003::verifyChecksum(uint8_t *msgPtr) {
  return (calcChecksum(msgPtr) == readChecksum(msgPtr));
}

pms5003Data PMS5003::readMeas() {
  // Reads the raw data from the PMS5003 sensor
  // calls verifyChecksum to make sure the message is coherent
  // prints an error message if the checksum check fails
  // In this case the data are invalid and the header
  // in the result structure is set to '\0' '\0'
  
  uint8_t *rawData = readRaw();
  pms5003Data pms5003Result;
  pms5003Data *pms5003ResultPtr;
  
  if (!verifyChecksum(rawData)) {
    Serial.println("Checksum test failed! The data are invalid!");
    Serial.print("Checksum calculated: ");
    Serial.print(calcChecksum(rawData),HEX);
    Serial.print(", checksum read: ");
    Serial.println(readChecksum(rawData),HEX);
    printRaw(rawData);
    // indicate an error
    pms5003Result.header[0] = '\0';
    pms5003Result.header[1] = '\0';
    return pms5003Result;
  }
  else {
    pms5003ResultPtr = evaluate(rawData);
    return *pms5003ResultPtr;
  }    
}
