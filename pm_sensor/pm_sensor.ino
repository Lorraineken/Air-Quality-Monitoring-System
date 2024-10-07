#include <PMS5003.h>
PMS5003 pms5003 = PMS5003(17,16); // initialize the PMS5003 class

void setup() {
  Serial.begin(9600);
  if (!Serial)
    delay(10);
}

void loop() {
  pms5003Data results;
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
  