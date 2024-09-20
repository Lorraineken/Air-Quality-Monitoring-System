# Air-Quality-Monitoring-System

## Introduction
Monitoring and managing of air quality has become a crucial aspect in cities worldwide due to the increased industrialization and growth of urbanization. Exposure to polluted air has been seen to have grave health implications which include respiratory infections, lung cancer, severe asthma, hospitalization and premature deaths. (University of Nairobi, Faculty of Law, 2023).

Air pollution is also associated with increased rate of miscarriages, a higher likelihood of birth defects occurring and lower birth weight. It has also been seen to cause damage to blood vessels that can occur due to fine particulate matter entering the bloodstream via the lungs which eventually damages the kidneys and liver as well as the reproductive health. The dominant pollutant in Nairobi is PM 2.5 as in 2020 its concentration was 1.5  times the recommended annual PM 2.5 threshold concentration of the World Health Organization (WHO). The reading then was at 14.7 ug/m3 . (IQAir, 2024).

The aim of this project was to develop an innovative air quality monitoring system using Internet of Things (IoT) technologies that can monitor air pollutants such as PM 1.0, PM 2.5 and PM 10. Humidity and Temperature values are also monitored. The design of the solution provides real-time data with an available real time backup system. The data collected can be used not only to inform the public about the quality of air but also inform authorities in policy and decision making regarding environmental and climate change management. 

## Objectives

The key objectives of the air quality monitoring project were :
Develop an IoT system to monitor essential air quality metrics which include PM 1.0, PM 2.5, PM 10, Humidity and Temperature. 
Provide a user interface which includes a cloud platform and lcd screen for real-time monitoring.
Provide a backup data storage system for the data collected with timestamp.

## Methodology
The air quality monitoring system is based on an IoT framework that includes sensors for detecting pollutants and other air quality metrics, external sd storage module, microcontroller and cloud-based platform for continuous data monitoring. The steps below describe the projects methodology : 

1. ***Sensor Selection***

- Particulate Matter Sensor : PMS 5003 for measuring PM 1.0, PM 2.5 and PM 10
- Temperature and Humidity Sensor : DHT 11
- Real Time Clock Module : DS 3231 for keeping track of current time and date
- External Storage Module : MH-SD Card Module
- Display Screen : LCD 16 * 2 display module

2. ***Microcontroller Selection***

ESP32 -WROOM-32 Microcontroller : This microcontroller was selected due to integrated connectivity, high processor power, support for a wide range of interfaces, Low cost, ease of use, large memory, large and active community of developers and security features.
Its wifi enabled feature 

3. ***Cloud Platform***

ThingSpeak : The data collected is sent to ThingSpeak which is a cloud platform that provides graphical real-time visualization and analysis of data.


4. ***Prototype Development***

The development of of the prototype consisted of several modules which where integrated as follows:

  1. **Download and Install Arduino IDE**

Follow the link below to download and install Arduino IDE

        https://www.arduino.cc/en/software

  2. **Integration of DHT11**

The DHT11 is a humidity and temperature sensor using its own proprietary serial communication protocol. To be able to read data from the sensor I first had to initialize the DHT11 class. After which I was able to read the temperature and humidity and print them on the serial monitor. The hardware connections are as follow:
    ![DHT11 PIN CONNECTION](./images/dht11_pinout.png?raw=true "DHT11 PIN CONNECTION")

Below is a figure of the typical application. The 5k resistor is recommended as a pull up resistor but not mandatory.
    ![DHT11 TYPICAL APPLICATION](./images/dht11_typical_application.png?raw=true "DHT11 TYPICAL APPLICATION")

Source: DHT11 datasheet

  3. **Integration of PMS5003 dust sensor**

The PMS5003 measures PM1.0, PM2.5 and PM10. To be able to read data from it there was a need to put it in active mode as it was in sleep mode initially. The circuit below retrieved from the data sheet was used.
    ![PMS5003 TYPICAL APPLICATION](./images/PMS5003_CIRCUIT.png?raw=true "PMS 5003 TYPICAL APPLICATION")

Source: PMS5003 datasheet

Connector Definition:

 ![PMS5003 PIN DEFINITION](./images/PMS5003_pin_configuration.png?raw=true "PMS5003 PIN DEFINITION")

Source: PMS5003 datasheet


The connection to the esp32 is as follows:
    ![PMS5003 PIN CONNECTION](./images/PMS5003_pinout.png?raw=true "PMS5003 PIN CONNECTION")


  4. **LCD Integration**

The LCD being used was one without I2C and it could only display data in 2 rows. To enable the display of data from DHT11 and PMS5003 I set the PMS5003 data to be displayed on the 1st row and the dht11 data to be displayed on the 2nd row. To be able to view all the data I enabled scroll display to the left feature.
Potentiometer connection is added to control the light intensity of the display. The Liquid Crystal library is used for setting up the LCD connection.
The pin connection is as follows:

![PMS5003 PIN CONNECTION](./images/lcd_pinout.png?raw=true "PMS5003 PIN CONNECTION")


  5. **SD card Module Integration**

The sd card which uses Serial Peripheral Interface(SPI) was used to provide backup storage and tracking of data read from the 2 sensors integrated initially. To store the data a folder and file are created if they were not initially created and the data is stored. 
The pin connection is as follows:

   ![SD PIN CONNECTION](./images/sd_pinout.png?raw=true "SD PIN CONNECTION")


The file created is a csv file which stores data in the format shown below.

   ![SD FILE FORMAT](./images/sd_card_data.png?raw=true "SD FILE FORMAT")


  6. **Enabling of Wifi connection**
Enabling wifi in the ESP32 was for setting up of the esp RTC and it was also necessary eventually in one of the improvements of the system in development and integration of a web server. The main feature included in the program is inclusion of the wifi header file and setting up the ssid and password of a currently active network.

  7. **Integration of Real Time Clock**
The DS3231 is a low cost and accurate real time clock that includes a battery input and maintains the correct time when the main power to the device is interrupted. It keeps time in seconds, minutes, hours, day, date, month and year. Address and data are transferred serially through the I2C bidirectional bus.
A program is written to set its time with the actual time and the data stored in the sd includes a time stamp.
The ESP32 RTC and DS3231 RTC are used concurrently such that if the ESP32 does not have access to the internet the date and time are read from the battery backed DS3231 RTC which has to be correctly set initially. The ESP32 RTC can get time and date from Network Time Protocol (NTP) which is set through wifi connection. Once this is done the timestamp is captured by either the ES32 rtc or the DS3231 RTC.
Below is the pin connection:

   ![RTC CONNECTION](./images/rtc_pinout.png?raw=true "RTC CONNECTION")

Below is a sample data set captured with the timestamp.

   ![RTC CAPTURED DATA](./images/data_with_timestamp.png?raw=true "RTC CAPTURED DATA")


  8. **ThingSpeak Integration**

ThingSpeak is a cloud platform that provides visualization and analysis of IoT data.To integrate thingspeak first register as a user on thingspeak. Afterwards I created a channel with fields for temperature, humidity and pm 1.0, pm 2.5 and pm 10. To enable our hardware configuration setup to be able to send data to thingspeak I generated a unique API key which is added to the program to allow for connection to thingspeak.
Below is a sample image of the Thingspeak dashboard of the data captured:

   ![THINGSPEAK DASH](./images/Thingspeak_image.png?raw=true "THINGSPEAK DASH")

# Improvements of The Air Quality Monitoring System

Up until this point the project's main objectives were successfully completed. In this regard there was a need to make improvements to the system to make it more effective and efficient. The suggested improvements included : Integrating a web server to allow multiple users to access the systems data, integration of a current sensor for power monitoring and management and inclusion of an environment sensor which would provide comparative values to the DHT11 for accuracy verification and more environmental data would be captured. These features were partially implemented.

## Methodology

  1. **Sensors Selection**

- Current Sensor : INA 3231 three way current sensor
- Environment Sensor : BME680 - Temperature, Humidity, Pressure and Gas Sensor

  2. **Web Server Integration**

ESP32 allows for setting up of a web server which allows for storage and processing of web pages. To create a web server the esp32 operates in 3 modes which are: station (STA) mode, Soft Access Point (AP) mode and STA and AP mode simultaneously. In this project STA mode is used where the ESP connects to an existing Wifi network then obtains an IP address from the wireless router to which it is connected. It then sets up a web server to the connected devices on the existing Wifi network.
Below is an image of how data is displayed by the webserver:

   ![WEB SERVER](./images/WebServer_aiq.jpeg?raw=true "WEB SERVER")

  3. **Current Sensor Integration**

INA3221 current sensor is a three-channel current and bus voltage monitor with an I2C compatible interface. It is being used to monitor the power consumption of the modules integrated within the project and is intended to ensure power management of the system.

Test Case Pin Connection
   ![CURRENT SENSOR PINOUT](./images/current_sensor_pinout.png?raw=true "CURRENT SENSOR PINOUT")

## Challenges

Network Connectivity : Maintaining reliable data transmission during weak wifi signals is a challenge as we are unable to capture data continuously on the cloud platform.
Integration of Current Sensor: Inaccurate readings for the Load’s current.

## Conclusion and Future Work

The air quality monitoring project showcases the feasibility of using IoT for real-time tracking of air pollutants. The system is effective in detecting various levels of particulate matter, humidity and temperature with accurate time. More can be done in completion of the project's improvement suggestion which is integration of the current sensor and the environment sensor.

Future work would include making the project more affordable through the implementation of components which consume less power such as microcontrollers with LoRa wireless communication technology and incorporation of a different display method as the LCD consumes a lot of power. In the event there are components that require different power sources it would be necessary to consider a current sensor that measures current for loads with different power sources.


## References

University of Nairobi, Faculty of Law. (2023, August 24). Breathing on borrowed time: Nairobi’s silent air crisis. https://law.uonbi.ac.ke/latest-news/breathing-borrowed-time-nairobis-silent-air-crisis

IQAir. (2024, September 16). Air quality in Nairobi: Air quality index (AQI) and PM2.5 air pollution in Nairobi. https://www.iqair.com/kenya/nairobi

# Authors
This project was contributed to by:
- [Lorraine Kupa](https://github.com/Lorraineken)

# License
The project is licensed under MIT.
