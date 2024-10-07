#include <Wire.h>
#include "RTClib.h"
#include <PMS5003.h>
#include <DHT11.h>
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP32Time.h>
#include "ThingSpeak.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

const char* ssid     = "Ken";
const char* password = "Ken@Kupa";

File dataFile; // Global file variable

//ThingSpeak setup
WiFiClient  client; //create wifi client to connect to ThingSpeak
unsigned long myChannelNumber = 3;
const char * myWriteAPIKey = "HK6MRXDIS3NYQM16";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// Time offset and NTP server configuration
const long gmtOffset_sec = 3 * 3600; // Offset for UTC+3
const int daylightOffset_sec = 0;    // No daylight savings
const char* ntpServer = "pool.ntp.org";

ESP32Time espRTC; // Create an instance of ESP32Time
RTC_DS3231 ds3231RTC; // Create an instance of RTC_DS3231

PMS5003 pms5003 = PMS5003(17,16); // initialize the PMS5003 class
DHT11 dht11(15);

const int rs = 13, en = 12, d4 = 14, d5 = 2, d6 = 4, d7 = 27;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Wifi setup  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  ThingSpeak.begin(client);  // Initialize ThingSpeak

  // Initialize SD card
  if (!SD.begin()) {
      Serial.println("Card Mount Failed");
      return;
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // Create a directory to store data if it doesn't exist
  createDir(SD, "/IoT4AQ");

  // Open file in append mode if it exists, otherwise create it
  dataFile = SD.open("/IoT4AQ/data.csv", FILE_APPEND);
  if (!dataFile) {
      Serial.println("Failed to open file for writing");
  } else {
      // Print headers to the file if it's new
      if (dataFile.size() == 0) {
          dataFile.println("Timestamp,PM1.0,PM2.5,PM10,Temperature,Humidity");
      }
  }
  dataFile.close(); // Close the file after initializing

   if (WiFi.status() != WL_CONNECTED) {
      Serial.print(".....");
       Serial.println("\n Trying to connect to the WiFi network");
      delay(5000);
  } 
  else {
      Serial.println("\nConnected to the WiFi network");
      Serial.print("Local ESP32 IP: ");
      Serial.println(WiFi.localIP());
  }

 /* Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());*/

  // Set time via NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;

  if (getLocalTime(&timeinfo)) {
      // Set the RTC with the time from NTP
      espRTC.setTimeStruct(timeinfo);
      Serial.println("ESP32 RTC set successfully via NTP!");
  } else {
      Serial.println("Failed to obtain time from NTP, using DS3231 RTC instead.");
      if (!ds3231RTC.begin()) {
          Serial.println("Couldn't find DS3231 RTC");
          while (1);
      }

      if (ds3231RTC.lostPower()) {
          Serial.println("DS3231 RTC lost power, please set the time manually.");
      } else {
          DateTime now = ds3231RTC.now();
          espRTC.setTime(now.unixtime() - gmtOffset_sec);
          Serial.println("ESP32 RTC set successfully via DS3231 RTC!");
      }
  }

  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  if (!Serial){
      delay(10);
  }
}

void loop() {
  pms5003Data result_pm;
  result_pm = pms5003.readMeas();

  int temperature = 0;
  int humidity = 0;

  // Attempt to read the temperature and humidity values from the DHT11 sensor.
  int result_dht = dht11.readTemperatureHumidity(temperature, humidity);

  // Set ThingSpeak Fields with Values
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, result_pm.pm1_0);
  ThingSpeak.setField(4, result_pm.pm2_5);
  ThingSpeak.setField(5, result_pm.pm10);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if(x == 200){
      Serial.println("ThingSpeak channel updated successful.");
  }
  else{
      Serial.println("Problem updating thingspeak channel. HTTP error code " + String(x));
  }

  // Create a string with the PM values  
  String displayString = "pm 1.0: " + String(result_pm.pm1_0) + " pm 2.5: " + String(result_pm.pm2_5) + " pm 10: " + String(result_pm.pm10);  
  String dht_string = "Temp: " + String(temperature) + "deg/c  Hum: " + String(humidity) + "%";
  
  // Print the string to the LCD  
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print(displayString); 
  lcd.setCursor(0,1);
  lcd.print(dht_string); 

  // Scroll the display to the left  
  for (int i = 0; i < displayString.length() + 16; i++) {  
      lcd.scrollDisplayLeft();  
      delay(300); // Adjust the speed of scrolling here  
  }

  // Prepare data string to be stored in CSV format
  String timestamp = espRTC.getTime("%Y-%m-%d %H:%M:%S");
  String dataString = timestamp + "," + String(result_pm.pm1_0) + "," + String(result_pm.pm2_5) + "," + String(result_pm.pm10) + "," + String(temperature) + "," + String(humidity) + "\n";

  // Open file in append mode and write data
  dataFile = SD.open("/IoT4AQ/data.csv", FILE_APPEND);
  if (dataFile) {
      if (dataFile.print(dataString)) {
          Serial.println("Data written to file");
      } else {
          Serial.println("Write failed");
      }
      dataFile.close(); // Close file after writing
  } else {
      Serial.println("Failed to open file for appending");
  }

  // Print time and date from ESP32 RTC
  Serial.println(espRTC.getTime("%A, %B %d %Y %H:%M:%S"));

  delay(1000); // Delay 1 second before printing again
}

// Function to create a directory on the SD card
void createDir(fs::FS &fs, const char *path) {
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path)) {
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}
