/**
 * PMS5003.h
 * Header file for the PMS5005 library, providing functionalities to interface 
 * with
 * the PMS5003 dust sensor
 * Author: Uli Raich
 * Version: 0.1
 * License: MIT
 */

#ifndef PMS5003_h
#define PMS5003_h

#include "Arduino.h"

#define RX 16
#define TX 17
#define PMS5003_MSG_LENGTH 32
/**
 * PMS5003 Class
 * Provides methods to read the Planower PMS5003 dust sensor
 */

typedef struct pms5003Data {
  char header[2];
  uint16_t framelength;
  uint16_t pm1_0;       // data 1
  uint16_t pm2_5;
  uint16_t pm10;
  uint16_t pm1_0_atm;   // data 4
  uint16_t pm2_5_atm;
  uint16_t pm10_atm;
  uint16_t nb_0_3;      // data 7
  uint16_t nb_0_5;
  uint16_t nb_1;;
  uint16_t nb_2_5;
  uint16_t nb_5;
  uint16_t nb_10;       // data 12
  uint16_t reserved;
  uint16_t checksum;
};

class PMS5003
{
public:
  uint8_t  pms5003Msg[32];
  uint8_t *pms5003MsgPtr;
  pms5003Data pms5003Result;
  /**
   * Constructor
   * Initializes the ESP32 UART2 for communication with the PMS5003
   *
   */
  PMS5003(int =RX, int =TX);
  
  /**
   * Reads raw data from the PMS5003 protocol
   * The PMS5003 sends a packet of measurement data every second
   * This packet consists of 32 uint8_t values
   *
   * @return: Returns a pointer to the raw data 
   */
  uint8_t      *readRaw();
  void         printRaw(uint8_t *);
  void         printMsg(pms5003Data *);
  pms5003Data  *evaluate(uint8_t *);
  int          calcChecksum(uint8_t *);
  int          readChecksum(uint8_t *);
  bool         verifyChecksum(uint8_t *);
  pms5003Data  readMeas();
  
private:
  uint8_t firstChar='B';
  uint8_t secondChar='M';
  uint8_t inchar;
  int     count;
  int     rx,tx;
};

#endif
