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
You can download and locally install the lastest installable version(s) via <link to the latest binary release(s) you published and describe to how people can install and run thise binaries; if needed describe this for different platforms>.

## Developing with the source code 
Describe how to use / adapt/ compile the souce code. 

## Features
List of features ready and TODOs for future development. Ready:

* Awesome feature 1
* Awesome feature 2
* Awesome feature 3

To-do:

* Wow improvement to be done 1
* Wow improvement to be done 2

## Status
Project is: _in progress_, _finished_, _no longer continue_ and why?

## License
This software is available under the [Apache 2.0 license](./LICENSE.md), Copyright 2021 [Research group Energy Transition, Windesheim University of Applied Sciences](https://windesheim.nl/energietransitie) 

## Credits
This software is a collaborative effort the following students and researchers:
* <contributor name 1> ·  [@Github_handle_1](https://github.com/<github_handle_1>) ·  Twitter [@Twitter_handle_1](https://twitter.com/<twitter_handle_1>)
* <contributor name 2> ·  [@Github_handle_2](https://github.com/<github_handle_2>) ·  Twitter [@Twitter_handle_2](https://twitter.com/<twitter_handle_2>)
* <contributor name 3> ·  [@Github_handle_3](https://github.com/<github_handle_3>) ·  Twitter [@Twitter_handle_3](https://twitter.com/<twitter_handle_3>)
* etc. 


We use and gratefully aknowlegde the efforts of the makers of the following source code and libraries:

* [arduino-esp32](https://github.com/espressif/arduino-esp32), by <Espressif>, licensed under [GNU LGPL-2.1](https://github.com/espressif/arduino-esp32/blob/master/LICENSE.md)
* [OneWire.h](https://github.com/PaulStoffregen/OneWire), by <Paul Stoffregen and others>, licensed under [unknown](license2 URL)
* [DallasTemperature.h](https://github.com/milesburton/Arduino-Temperature-Control-Library), by <Miles Burton>, licensed under [GNU LGPL-2.1](https://github.com/milesburton/Arduino-Temperature-Control-Library#license)
* [](library 3 URL), by <copyright holder name 3>, licensed under [license 3 name](license3 URL)
* [library name 3 and version](library 3 URL), by <copyright holder name 3>, licensed under [license 3 name](license3 URL)
* [library name 3 and version](library 3 URL), by <copyright holder name 3>, licensed under [license 3 name](license3 URL)
* [library name 3 and version](library 3 URL), by <copyright holder name 3>, licensed under [license 3 name](license3 URL)

## Contact
<not yet determined; which contact info to include here?>