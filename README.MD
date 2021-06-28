# Twomes Boiler Temperature Sensor
Firmware for the Twomes "Digital Twins for the Heat-transition" Boiler temperature Sensor 

## Table of contents
* [General info](#general-info)
* [Using binary releases](#using-binaries-releases)
* [Developing with the source code ](#developing-with-the-source-code) 
* [Features](#features)
* [Status](#status)
* [License](#license)
* [Credits](#credits)

## General info
This firmware is designed to run on the ESP32 of the Twomes boiler Temperature Sensor hardware. This device has two DS18b20(Z) temperature sensors clamped to the water pipes leading into and coming out of the boiler for the central heating system.
The ESP32 reads the temperatures measured by these devices every ten seconds, and stores them in the RTC memory section, where they are preserved while the ESP32 enters sleep mode.
Once a series of 20 measurements have been taken the ESP32 will send the measurements to the Twomes P1-Gateway device using the Espressif ESP-Now protocol.

## Using binary releases
Binary release not (yet) available

## Developing with the source code 
Install [Visual Studio Code](https://code.visualstudio.com/) and the [PlatformIO](https://platformio.org/platformio-ide) plugin

Download the sourcecode, unzip it, and open the folder in Visual Studio Code.
Modify the sourcecode to fit your needs. Connect your board, and press the arrow on the blue bar on the bottom left to compile and upload your code


## Features
List of features ready and TODOs for future development. 

Ready:
* Measure temperatures using two DS18b20 sensors
* Sleep between conversions for ultra-low power consumption
* Send data to a gateway device using ESP-Now

To-do:
* Implement Channel Provisioning via Gateway and save via NVS
* Implement status- and error indicators using LEDs

## Status
Project is: in Progress.
In its current state the software can properly read temperatures and send these via ESP-Now.
There are still some missing features that need to be implemented, see [To-do](#features) for more info

## License
This software is available under the [Apache 2.0 license](./LICENSE.md), Copyright 2021 [Research group Energy Transition, Windesheim University of Applied Sciences](https://windesheim.nl/energietransitie) 

## Credits
This software is a collaborative effort the following students and researchers:
* Fredrik-Otto Lautenbag ·  [@Fredrik1997](https://github.com/Fredrik1997)
* Gerwin Buma ·  [@GerwinBuma](https://github.com/GerwinBuma) 
* Werner Heetebrij ·  [@Werner-Heetebrij] (https://github.com/Werner-Heetebrij)
* Sjors Smit ·  [@Shorts1999](https://github.com/Shorts1999)


We use and gratefully aknowlegde the efforts of the makers of the following source code and libraries:

* [arduino-esp32](https://github.com/espressif/arduino-esp32), by Espressif, licensed under [GNU LGPL-2.1](https://github.com/espressif/arduino-esp32/blob/master/LICENSE.md)
* [OneWire.h](https://github.com/PaulStoffregen/OneWire), by Paul Stoffregen and others, licensed under [unknown](license2 URL)
* [DallasTemperature.h](https://github.com/milesburton/Arduino-Temperature-Control-Library), by Miles Burton, licensed under [GNU LGPL-2.1](https://github.com/milesburton/Arduino-Temperature-Control-Library#license)

## Contact
<not yet determined; which contact info to include here?>
