
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
#include "ESPAsyncWebServer.h"

// WiFi credentials
const char* ssid = "";
const char* password = "";

// Sensor and module initializations
PMS5003 pms5003 = PMS5003(17, 16);
DHT11 dht11(15);
ESP32Time espRTC;
RTC_DS3231 ds3231RTC;

const int rs = 13, en = 12, d4 = 14, d5 = 2, d6 = 4, d7 = 27;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// ThingSpeak setup
WiFiClient client;
unsigned long myChannelNumber = 3;
const char* myWriteAPIKey = "";

// File setup
File dataFile;

// Web server setup
AsyncWebServer server(80);

// Timer and NTP variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
const long gmtOffset_sec = 3 * 3600; // Offset for UTC+3
const int daylightOffset_sec = 0;
const char* ntpServer = "pool.ntp.org";

// Data variables
int temperature = 0;
int humidity = 0;
pms5003Data result_pm;

// Function to create a directory on the SD card
void createDir(fs::FS &fs, const char *path) {
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path)) {
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}


String readDHTData() {
    int result = dht11.readTemperatureHumidity(temperature, humidity);
    if (result == 0) {
        return "Temp: " + String(temperature) + " °C | Hum: " + String(humidity) + " %";
    } else {
        return "--";
    }
}

String readPMData() {
    result_pm = pms5003.readMeas();
    return "PM1.0: " + String(result_pm.pm1_0) + " µg/m³ | PM2.5: " + String(result_pm.pm2_5) + " µg/m³ | PM10: " + String(result_pm.pm10) + " µg/m³";
}

// Web page placeholder processor
String processor(const String& var) {
    if (var == "TEMPERATURE") {
        return String(temperature);
    } else if (var == "HUMIDITY") {
        return String(humidity);
    } else if (var == "PM1_0") {
        return String(result_pm.pm1_0);
    } else if (var == "PM2_5") {
        return String(result_pm.pm2_5);
    } else if (var == "PM10") {
        return String(result_pm.pm10);
    }
    return String();
}

// HTML for the web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels {
      font-size: 1.5rem;
      vertical-align: middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>IoT4AQ Sensor Data</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
  </p>
  <p>
    <i class="fas fa-smog" style="color:#e74c3c;"></i> 
    <span class="dht-labels">PM1.0</span>
    <span id="pm1_0">%PM1_0%</span>
  </p>
  <p>
    <i class="fas fa-smog" style="color:#e67e22;"></i> 
    <span class="dht-labels">PM2.5</span>
    <span id="pm2_5">%PM2_5%</span>
  </p>
  <p>
    <i class="fas fa-smog" style="color:#f39c12;"></i> 
    <span class="dht-labels">PM10</span>
    <span id="pm10">%PM10%</span>
  </p>
</body>
<script>
setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var data = this.responseText.split('|');
      document.getElementById("temperature").innerHTML = data[0].trim().replace('Temp: ', '');
      document.getElementById("humidity").innerHTML = data[1].trim().replace('Hum: ', '');
      document.getElementById("pm1_0").innerHTML = data[2].trim().replace('PM1.0: ', '');
      document.getElementById("pm2_5").innerHTML = data[3].trim().replace('PM2.5: ', '');
      document.getElementById("pm10").innerHTML = data[4].trim().replace('PM10: ', '');
    }
  };
  xhttp.open("GET", "/sensordata", true);
  xhttp.send();
}, 10000);
</script>
</html>)rawliteral";

void setup() {
    // Serial port setup
    Serial.begin(9600);

    // WiFi connection
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

    // Web server routes
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send_P(200, "text/html", index_html, processor);
    });

    server.on("/sensordata", HTTP_GET, [](AsyncWebServerRequest* request) {
        String sensorData = readDHTData() + " | " + readPMData();
        request->send_P(200, "text/plain", sensorData.c_str());
    });

    // Start server
    server.begin();
}

void loop() {
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  
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


