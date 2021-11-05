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
This firmware is designed to run on the ESP32 of the [Twomes Temperature Monitor hardware](https://github.com/energietransitie/twomes-temp-monitor-hardware), in combination with two DS18b20(Z) temperature sensors in clamps that can be connected the supply water piple (from the boiler to the hydronic radiators) and return water pipes returning from the hydronic radiators to the boiler. We do not expect the end user of the device to be able to distinguish between supply and return water pipes and leave the determination of supply and return to the analysis phase of the data.

The ESP32 reads the temperatures from both sensors and stores them in [RTC fast memory](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/memory-types.html#rtc-fast-memory), to preserve them during the deep sleep interval between brief moments when the ESP32 needs to be awake for data acquisition. After a series of 20 measurements at fixed time intervals, the ESP32 will send the series of measurements to the [Twomes P1 Gateway measurement device](https://github.com/energietransitie/twomes-p1-gateway-firmware) using the ESP-NOW protocol.

Via the [Twomes P1 Gateway measurement device](https://github.com/energietransitie/twomes-p1-gateway-firmware), the Twomes Boiler Monitor Module sends the following properties to the Twomes server:
| Sensor  | Property           | Unit | Data type  | Format | Interval [h:mm:ss]| Description                            |
|---------|--------------------|------|------------|--------|-------------------|----------------------------------------|
| DS18b20 | `boilerTemp1`      | °C   | float      | f.1    | 0:00:10           | pipe temperature                       |
| DS18b20 | `boilerTemp2`      | °C   | float      | f.1    | 0:00:10           | pipe temperature                       |



## Pairing
After [deploying](#deploying) and installation in the home, the Twomes Boiler Monitor Module should be [paired as a satellite to the Twomes P1 Gateway measurement device](https://github.com/energietransitie/twomes-p1-gateway-firmware/blob/main/README.md#pairing-satellites).

## Deploying
This section describes how you can deploy binary releases of the firmware, i.e. without changing the source code, without a development environment and without needing to compile the source code.

### Prerequisites
To deploy the firmware, in addition to the [generic prerequisites for deploying Twomes firmware](https://github.com/energietransitie/twomes-generic-esp-firmware#prerequisites), you need:
* a 3.3V TTL-USB Serial Port Adapter (e.g. [FT232RL](https://www.tinytronics.nl/shop/en/communication-and-signals/usb/ft232rl-3.3v-5v-ttl-usb-serial-port-adapter), CP210x, etc..), including the cable to connect ths adapter to a free USB port on your computer (a USB to miniUSB cable in the case of a [FT232RL](https://www.tinytronics.nl/shop/en/communication-and-signals/usb/ft232rl-3.3v-5v-ttl-usb-serial-port-adapter));
* You need to place a 3.6V Lithium AA-battery (e.g. SAFT LS14500) in its holder to power the board, as the Twomes Boiler Monitor Module is not powered via the 6 pin programming connector.
* Find a row of 6 holes holes (J1 in the corner of the PCB of the Boiler Monitor Module), find the `GND` pin (see  bottom of the PCB), alighn the 6 pins of the serial port adapter such that `GND` and other pins match; then connect the serial port adapter to your computer and connect the 6 pins of the serial port adapter to the 6 holes on the PCB.

### Uploading firmware
* Download the [binary release for your device](https://github.com/energietransitie/twomes-boiler-monitor-firmware/releases) and extract it to a directory of your choice.
* If you used the device before, you shoud first [erase all persistenly stored data](#erasing-all-persistenly-stored-data).
* Open a comand prompt in that directory, change the directory to the binaries subfolder and enter (**N.B. this command differs from the deployment command of the generic firmware**):
	```shell
	py -m esptool --chip esp32 --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x9000 partitions.bin 0xe000 boot_app0.bin 0x10000 firmware.bin  
	```
* This should automatically detect the USB port that the device is connected to.
* If not, then open the Device Manager (in Windows press the `Windows + X` key combination, then select Device Manager), go to View and click Show Hidden Devices. Then unfold `Ports (COM & LPT)`. You should find the device there, named `USB-Serial Port`. If there is no numbered COM port indicated, it's usually the next in sequence.  
* If the COM port is not automatically detected, then enter (while replacing `?` with the digit found in the previous step) (**N.B. this command differs from the deployment command of the generic firmware**): 
	```shell
	py -m esptool --chip esp32 --port "COM?" --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x9000 partitions.bin 0xe000 boot_app0.bin 0x10000 firmware.bin```
* When you see the beginning of the sequence `conecting ......_____......`, press and hold the button labeled `GPIO0 (SW2)` on the PCB, then briefly press the button labeled `RESET (SW1)` shortly. 
* You should see an indication that the firmware is being written to the device.
* When the upload is finished, view the serial output with a serial monitor tool like PuTTY or the utility of your IDE (115200 baud). Press reset and make sure the firmware boots. 
* Remove the battery or insert a battery insulating pull tab when you do not plan to use the boiler monitor module immediately, to prevent unnecessary battery drain. In particular, when the temperature sensors are not connected, power consumption is higher. 

## Developing 
This section describes how you can change the source code using a development environment and compile the source code into a binary release of the firmware that can be depoyed, either via the development environment, or via the method described in the section [Deploying](#deploying).

Please see the [developing section of the generig Twomes firmware](https://github.com/energietransitie/twomes-generic-esp-firmware#developing) first. Reember to presse buttons to upload the firmware: 
* When you see the beginning of the sequence `conecting ......_____......`, press and hold the button labeled `GPIO0 (SW2)` on the PCB, then briefly press the button labeled `RESET (SW1)` shortly. 
* You should see an indication that the firmware is being written to the device.

## Features
List of features ready and TODOs for future development. 

Ready:
* Pairing with a [Twomes P1 Gateway measurement device](https://github.com/energietransitie/twomes-p1-gateway-firmware)
* Measure temperatures using two DS18b20 sensors
* Deep sleep between data acquisition for ultra-low power consumption
* Send measurement data to [Twomes P1 Gateway measurement device](https://github.com/energietransitie/twomes-p1-gateway-firmware) using ESP-NOW

To-do:
* Implement more status- and error indicators using LEDs

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
