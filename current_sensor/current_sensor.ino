//   SDL_Arduino_INA3221 Library Test Code
//   SDL_Arduino_INA3221.cpp Arduino code - runs in continuous mode
//   Version 1.2
//   SwitchDoc Labs   September 2019



#include <Wire.h>
#include "SDL_Arduino_INA3221.h"

SDL_Arduino_INA3221 ina3221;

// the three channels of the INA3221 
#define CHANNEL_1 1
#define CHANNEL_2 2
#define CHANNEL_3 3

void setup(void) 
{
    
  Serial.begin(9600);
  Serial.println("SDA_Arduino_INA3221_Test");
  
  Serial.println("Measuring voltage and current with ina3221 ...");
  ina3221.begin();

  Serial.print("Manufactures ID=0x");
  int MID;
  MID = ina3221.getManufID();
  Serial.println(MID,HEX);
}

void loop(void) 
{
  
  Serial.println("------------------------------");
  float shuntvoltage1 = 0;
  float busvoltage1 = 0;
  float current_mA1 = 0;
  float loadvoltage1 = 0;


  busvoltage1 = ina3221.getBusVoltage_V(CHANNEL_1);
  shuntvoltage1 = ina3221.getShuntVoltage_mV(CHANNEL_1);
  current_mA1 = ina3221.getCurrent_mA(CHANNEL_1);  // minus is to get the "sense" right.   - means the battery is charging, + that it is discharging
  loadvoltage1 = busvoltage1 + (shuntvoltage1 / 1000);
  
  Serial.print("CHANNEL_1 Bus Voltage:   "); Serial.print(busvoltage1); Serial.println(" V");
  Serial.print("CHANNEL_1 Shunt Voltage: "); Serial.print(shuntvoltage1); Serial.println(" mV");
  Serial.print("CHANNEL_1 Load Voltage:  "); Serial.print(loadvoltage1); Serial.println(" V");
  Serial.print("CHANNEL_1 Current 1:       "); Serial.print(current_mA1); Serial.println(" mA");
  Serial.println("");

  float shuntvoltage2 = 0;
  float busvoltage2 = 0;
  float current_mA2 = 0;
  float loadvoltage2 = 0;

  busvoltage2 = ina3221.getBusVoltage_V(CHANNEL_2);
  shuntvoltage2 = ina3221.getShuntVoltage_mV(CHANNEL_2);
  current_mA2 = -ina3221.getCurrent_mA(CHANNEL_2);
  loadvoltage2 = busvoltage2 + (shuntvoltage2 / 1000);
  
  //Serial.print("CHANNEL_2 Bus Voltage 2:   "); Serial.print(busvoltage2); Serial.println(" V");
  //Serial.print("CHANNEL_2 Shunt Voltage 2: "); Serial.print(shuntvoltage2); Serial.println(" mV");
  //Serial.print("CHANNEL_2 Load Voltage 2:  "); Serial.print(loadvoltage2); Serial.println(" V");
  //Serial.print("CHANNEL_2 Current 2:       "); Serial.print(current_mA2); Serial.println(" mA");
  //Serial.println("");

  float shuntvoltage3 = 0;
  float busvoltage3 = 0;
  float current_mA3 = 0;
  float loadvoltage3 = 0;

  busvoltage3 = ina3221.getBusVoltage_V(CHANNEL_3);
  shuntvoltage3 = ina3221.getShuntVoltage_mV(CHANNEL_3);
  current_mA3 = ina3221.getCurrent_mA(CHANNEL_3);
  loadvoltage3 = busvoltage3 + (shuntvoltage3 / 1000);
  
  //Serial.print("CHANNEL_3 Bus Voltage 3:   "); Serial.print(busvoltage3); Serial.println(" V");
  //Serial.print("CHANNEL_3 Shunt Voltage 3: "); Serial.print(shuntvoltage3); Serial.println(" mV");
  //Serial.print("CHANNEL_3 Load Voltage 3:  "); Serial.print(loadvoltage3); Serial.println(" V");
  //Serial.print("CHANNEL_3 Current 3:       "); Serial.print(current_mA3); Serial.println(" mA");
  //Serial.println("");

  delay(30000);
}