# PMS5003 Arduino Library

## Table of Contents

- [Introduction](#introduction)
- [How It Works](#how-it-works)
  - [Internal Protocol Handling](#internal-protocol-handling)
- [Features](#features)
- [Installation](#installation)
    - [Arduino IDE Library Manager](#arduino-ide-library-manager)
    - [Installing Manually from GitHub](#installing-manually-from-github)
- [Usage](#usage)
  - [Basic Usage](#basic-usage)
  - [New Methods in v2.0.0](#new-methods-in-v200)
  - [Wiring Details](#wiring-details)
- [Examples](#examples)
- [Error Handling](#error-handling)
- [Troubleshooting](#troubleshooting)
- [FAQ](#faq)
- [Compatibility](#compatibility)
  - [Contribute by Testing](#contribute-by-testing)
- [Contributing](#contributing)
- [Code of Conduct](#code-of-conduct)
- [License](#license)
- [External References](#external-references)

## Introduction

This Arduino library is designed to read the PlanTower PMS5003 dust sensor. It uses UART2 on
the ESP32 (UART1 is used for the serial monitor UART2 is used for PSRAM access on certain
CPU board. UART2 is free.
The library has been developped for the IoT4AQ (IoT for air quality measurements) held in
2024 at the University Alioune Diop, Bambey, Sénégal

**Author:** [Uli Raich](https://github.com/uraich)

**Version:** 1.0

**License:** [MIT](/LICENSE)

## How It Works

The PMS5003 dust sensor uses messages on a serial line at 9600 baud. It uses a 32 byte protocol with the
following layout:

| Byte number  | Meaning                                   | Value                           |
|--------------|:-----------------------------------------:|--------------------------------:|
| 0 .. 1       | Header                      		   | "BM"                            |
| 2 .. 3       | Frame Length                		   | length = 2*13+2 (data+checksum) |
| 4 .. 5       | PM1.0 concentration [ug/m3] 		   |                                 |
| 6 .. 7       | PM2.5 concentration [ug/m3] 		   |                                 |
| 8 .. 9       | PM10  concentration [ug/m3] 		   |                                 |
| 10 .. 11     | PM1.0 under atmospheric     		   |                                 |
| 12 .. 13     | PM2.5 pressure              		   |                                 |
| 14 .. 15     | PM10                        		   |                                 |
| 16 .. 17     | no of particles above 0.3 um in 1L volume |                                 |
| 18 .. 19     | no of particles above 0.5 um in 1L volume |                                 |
| 20 .. 21     | no of particles above 1.0 um in 1L volume |                                 |
| 22 .. 23     | no of particles above 2.5 um in 1L volume |                                 |
| 24 .. 25     | no of particles above 5.0 um in 1L volume |                                 |
| 26 .. 27     | no of particles above  10 um in 1L volume |                                 |
| 28 .. 29     | reserved                                  |                                 |
| 30 .. 31     | checksum                                  |                                 |

### Internal Protocol Handling

The library waits for a header "BM" to arrive on SERIAL2, the third UART on the ESP32 (UART2) which uses the pins GPIO 16
Tx and GPIO 17 for Rx. After that it reads 30 bytes of raw data from the serial line. 

![](/assets/communication.png)
**Source & Credit :** [PMS5003 Datasheet](https://www.digikey.com/en/htmldatasheets/production/2903006/0/0/1/pms5003-series-manual)

In addition to reading raw data, the library extracts physical data, combining 2 data bytes into 16 bit data words. It verifies the checksum
and returns a data structure containing all the information extracted from the protocol 

## Features

- **Easy to Use**: Easy to use interface for reading temperature and humidity from the DHT11 sensor.
- **No External Dependencies**: The library is standalone and doesn't require any external libraries.
- **Example Sketches**: The library package includes example Arduino sketches to get you started quickly.

## Installation

### Installing Manually from GitHub
1. Download the latest release of the library from this GitHub repository as a ZIP file.
2. Open the Arduino IDE.
3. Go to `Sketch` > `Include Library` > `Add .ZIP Library...`.
4. Navigate to the downloaded ZIP file and select it to install.

## Usage

### Basic Usage

- Include the `PMS5003.h` header file.
- Create an instance of the PMS5003 class. The constructor takes no arguments.
- The following methods are available:

    uint8_t      *readRaw();
    void         printRaw(uint8_t *);
    void         printMsg(pms5003Data *);
    pms5003Data  *evaluate(uint8_t *);
    int          calcChecksum(uint8_t *);
    int          readChecksum(uint8_t *);
    bool         verifyChecksum(uint8_t *);
    pms5003Data  readMeas();
  

readRaw returns a pointer to an array of 32 bytes. These are the data read from the PMS5003 protocol without any treatment
The pointer to the raw data can be put into the evaluate method, which returns the following data structure, directly mapped onto the
PMS5003 protocol:

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

It is possible to get at the physical data by calling just a single function:

    PMS5003 pms5003 = PMS5003();             // create a PMS5003 object
    pms5003Data result = pms5003.readMeas(); // verifies the checksum and returns the above data structure correctly filled
    
### Wiring Details

The connections are made as follows:
|PMS5003 pin number | PMS5003 pin significance | ESP32 GPIO pin number |
|:-----------------:| :-----------------------:| :--------------------:|
| Pin 1             | Vcc                      | 5V                    |
| Pin 2             | GND                      | GND                   |
| Pin 3             | Set (suspend mode)       | not used              |
| Pin 4             | Rx                       | GPIO 17               |
| Pin 5             | Tx                       | GPIO 16               |
| Pin 6             | Reset                    | not used              |
| Pin 7             | NC (not connected)       |                       |

## Examples

All examples provided use Arduino UNO's digital pin 2 as the default connection to the DHT11 sensor's data pin. Modify this in the code if using a different pin.

- **[Read Raw Data](examples/pms5003Raw/pms5003Raw.ino)**  
  This example demonstrates how to read the raw data from the PMS5003 and print them

- **[Read a Measurment](examples/pms5003Simple/pms5002Simple.ino)**  
  Reads a measurement into the pms5003Data structure. The individual fields pm1.0, pm2.5 ... number of particles in 1L volume can the easily be extracted.

- **[Read All](examples/pms5003Test/pms5003Test.ino)**  
  Tests all PMS5003 methods


## Error Handling

The library will print an error message if the checksum does not match. 

## Troubleshooting

- **Sensor Not Responding:**
  Your program may block if the wiring is wrong and the library does not see any data coming in on the serial line

### Contribute by Testing

If you've successfully used this library on a board, please consider contributing by letting me know. This will help the community to have a broader understanding of the library's compatibility.

1. Fork the [repository](https://github.com/dhrubasaha08/DHT11).
2. Update the README with the board you've tested.
3. Create a pull request with your changes.

Your contribution will be greatly appreciated, and it will benefit the entire Arduino community.

## Contributing

For guidelines on contributing to this project, please see the [`CONTRIBUTING.md`](CONTRIBUTING.md) file.

## License

This library is licensed under the `MIT` License. See the [`LICENSE`](LICENSE) file for more details.

## External References

- [`PMS5003 Datasheet`](https://www.digikey.jp/htmldatasheets/production/2903006/0/0/1/pms5003-series-manual.html)
