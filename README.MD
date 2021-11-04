# Twomes Boiler Monitor Module Firmware
Firmware for the Twomes Boiler Monitor Module. 

## Table of contents
* [General info](#general-info)
* [Deploying](#deploying)
* [Developing](#developing) 
* [Features](#features)
* [Pairing](#pairing)
* [Status](#status)
* [License](#license)
* [Credits](#credits)

## General info
This firmware is designed to run on the ESP32 of the Twomes Temperature Monitor hardware. This device has two DS18b20(Z) temperature sensors clamped to the water pipes leading into and coming out of the boiler for the central heating system.
The ESP32 reads the temperatures measured by these devices every ten seconds, and stores them in the RTC memory section, where they are preserved while the ESP32 enters sleep mode.
Once a series of 20 measurements have been taken the ESP32 will send the measurements to the Twomes P1-Gateway device using the Espressif ESP-NOW protocol.

For the associated hardware design files for the Twomes Temperature Monitor PCB and enclosure and tips and instructions how to produce and assemble the hardware, please see the [twomes-temp-monitor-hardware](https://github.com/energietransitie/twomes-temp-monitor-hardware) repository. 

## Deploying
### Prerequisites
*	a USB to Serial programmer (FTDI, CP210x, etc..);
*	a PC with a USB port;
*	[Python v3.8 or above](https://www.python.org/downloads/) installed, and make sure to select `Add Python <version number> to PATH` so you can use the Python commands we document below from a command prompt;
*	[Esptool](https://github.com/espressif/esptool) installed, the Espressif SoC serial bootloader utility;
*	[PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/), a serial monitor utility. (If your you're also developing, you may use the serial monitor utility in your IDE, instead).

### How to upload firmware
* The boiler monitor device is not powered via the 6 pin programming connector, so it is needed to place the special 3.6V battery to power the board.
* Find the 6 holes programming connector J1 in the corner next to the license text. Connect a 3.3V USB<->Serial converter to the 6 holes in the right top corner, the pinout is described on the top of the PCB: GND, .. , 3V3, RX, TX, .. 

### On Windows:
* Unzip the release binaries archive in a folder.  
* On the board, press and hold the GPIO0 (SW2) button, and press the Reset button shortly, make sure you keep holding the GPIO0 button for a second longer. The green led is now on, the device is now waiting for the upload of the firmware. 
* Run UPLOAD.bat
* When the upload is finished, view the serial output with a serial monitor tool like Putty or the utility of your IDE (115200 baud). Press reset and make sure the firmware boots: 
* When temperature sensors are connected, you will see measurements with an interval of 10 seconds. Remove the battery when you do not have plans to pair the boiler monitor with a P1 gateway rightaway: Otherwise, the boiler monitor ESP_NOW messages to the gateway will fail and the retransmissions will cause more drain of the battery, also when the temperature sensors are not connected, there is a higher power consumption. 

## Developing 
Install [Visual Studio Code](https://code.visualstudio.com/) and the [PlatformIO](https://platformio.org/platformio-ide) plugin

Download the sourcecode, unzip it, and open the folder in Visual Studio Code.
Modify the sourcecode to fit your needs. Connect your board, and press the arrow on the blue bar on the bottom left to compile and upload your code

## Features
List of features ready and TODOs for future development. 

Ready:
* Measure temperatures using two DS18b20 sensors
* Sleep between conversions for ultra-low power consumption
* Send data to a gateway device using ESP-NOW
* Channel Provisioning via Gateway and save via NVS

To-do:
* Implement more status- and error indicators using LEDs

## Pairing
The boiler monitor will send the measurements to the Twomes P1-Gateway device using the Espressif ESP-NOW protocol. The P1-Gateway listens on a specific 2.4 GHz WiFi channel, that is chosen after the provisioning of the P1-Gateway. To let the boiler monitor know on which channel the P1-Gateway expects the measurements, use the following procedure (provisioning of the ESP_NOW channel):
* Place the battery in the boiler monitor, make sure it is near the P1-Gateway to be able to see whether the provisioning is successful.
* On the boiler monitor, press and hold the GPIO15 (SW2) "K" button and Reset (SW1) "R" button, and release the Reset button first, and after that the "K" button. The green led will blink for 20 seconds to indicate that the boiler monitor listens (on WiFi channel 0) for the ESP_NOW channel information from the P1-Gateway.
* On the P1-Gateway, within these 20 seconds, press the GPIO12 (SW2) "K" button. The P1-Gateway now transmits on channel 0 the information about which WiFi channel it expects the ESP_NOW messages, the green led blinks shortly during this transmission.
* On the boiler monitor, when the channel provisioning is successful, the green led will blink and after that the red led.
* This procedure can be repeated if needed, e.g. after renewed provisioning of the P1-Gateway.

## Status
Project is: in Progress.

## License
This software is available under the [Apache 2.0 license](./LICENSE.md), Copyright 2021 [Research group Energy Transition, Windesheim University of Applied Sciences](https://windesheim.nl/energietransitie) 

## Credits
This software is a collaborative effort of:
* Sjors Smit ·  [@Shorts1999](https://github.com/Shorts1999)
* Fredrik-Otto Lautenbag ·  [@Fredrik1997](https://github.com/Fredrik1997)
* Gerwin Buma ·  [@GerwinBuma](https://github.com/GerwinBuma) 
* Werner Heetebrij ·  [@Werner-Heetebrij](https://github.com/Werner-Heetebrij)

Product owner:
* Marco Winkelman · [@MarcoW71](https://github.com/MarcoW71)

We use and gratefully aknowlegde the efforts of the makers of the following source code and libraries:

* [arduino-esp32](https://github.com/espressif/arduino-esp32), by Espressif, licensed under [GNU LGPL-2.1](https://github.com/espressif/arduino-esp32/blob/master/LICENSE.md)
* [OneWire.h](https://github.com/PaulStoffregen/OneWire), by Paul Stoffregen and others, licensed under [unknown](license2 URL)
* [DallasTemperature.h](https://github.com/milesburton/Arduino-Temperature-Control-Library), by Miles Burton, licensed under [GNU LGPL-2.1](https://github.com/milesburton/Arduino-Temperature-Control-Library#license)
