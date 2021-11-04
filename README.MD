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
Once a series of 20 measurements have been taken the ESP32 will send the measurements to the [Twomes P1 Gateway measurement device](https://github.com/energietransitie/twomes-p1-gateway-firmware) using the ESP-NOW protocol.

For the associated hardware design files for the Twomes Temperature Monitor module hardware and enclosure and tips and instructions how to produce and assemble the hardware, please see the [twomes-temp-monitor-hardware](https://github.com/energietransitie/twomes-temp-monitor-hardware) repository. 

## Deploying
This section describes how you can deploy binary releases of the firmware, i.e. without changing the source code, without a development environment and without needing to compile the source code.

### Prerequisites
To deploy the firmware, in addition to the [generic prerequisites for deploying Twomes firmware](https://github.com/energietransitie/twomes-generic-esp-firmware#prerequisites), you need:
* a 3.3V TTL-USB Serial Port Adapter (e.g. [FT232RL](https://www.tinytronics.nl/shop/en/communication-and-signals/usb/ft232rl-3.3v-5v-ttl-usb-serial-port-adapter), CP210x, etc..), including the cable to connect ths adapter to a free USB port on your computer (a USB to miniUSB cable in the case of a [FT232RL](https://www.tinytronics.nl/shop/en/communication-and-signals/usb/ft232rl-3.3v-5v-ttl-usb-serial-port-adapter));
* You need to place a 3.6V Lithium AA-battery (e.g. SAFT LS14500) in its holder to power the board, as the Twomes Boiler Monitor Module is not powered via the 6 pin programming connector.
* Find a row of 6 holes holes (J1 in the corner of the PCB of the Boiler Monitor Module), find the `GND` pin (see  bottom of the PCB), alighn the 6 pins of the serial port adapter such that `GND` and other pins match; then connect the serial port adapter to your computer and connect the 6 pins of the serial port adapter to the 6 holes on the PCB.

### Uploading firmware

* Download the [binary release for your device](https://github.com/energietransitie/twomes-p1-boiler-monitor-firmware/releases) and extract it to a directory of your choice.
* If you used the device before, you shoud first [erase all persistenly stored data](#erasing-all-persistenly-stored-data).
* Open a comand prompt in that directory, change the directory to the binaries subfolder and enter (**N.B. this command differs from the deployment command of the generic firmware**):
	```shell
	py -m esptool --chip esp32 --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x9000 partitions.bin 0xe000 ota_data_initial.bin 0x10000 firmware.bin  
	```
* This should automatically detect the USB port that the device is connected to.
* If not, then open the Device Manager (in Windows press the `Windows + X` key combination, then select Device Manager), go to View and click Show Hidden Devices. Then unfold `Ports (COM & LPT)`. You should find the device there, named `USB-Serial Port`. If there is no numbered COM port indicated, it's usually the next in sequence.  
* If the COM port is not automatically detected, then enter (while replacing `?` with the digit found in the previous step) (**N.B. this command differs from the deployment command of the generic firmware**): 
	```shell
	py -m esptool --chip esp32 --port "COM?" --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x9000 partitions.bin 0xe000 ota_data_initial.bin 0x10000 firmware.bin```
* When you see the beginning of the sequence `conecting ......_____......`, press and hold the button labeled `GPIO0 (SW2)` on the PCB, then briefly press the button labeled `RESET (SW1)` shortly. 
* You should see an indication that the firmware is being written to the device.
* When the upload is finished, view the serial output with a serial monitor tool like PuTTY or the utility of your IDE (115200 baud). Press reset and make sure the firmware boots: 
* When temperature sensors are connected, you will see measurements with an interval of 10 seconds. Remove the battery when you do not have plans to pair the boiler monitor with a P1 gateway rightaway. Otherwise, the boiler monitor ESP-NOW messages to the P1 Gateway will fail and the retransmissions will cause more drain of the battery, also when the temperature sensors are not connected, there is a higher power consumption. 

## Developing 
This section describes how you can change the source code using a development environment and compile the source code into a binary release of the firmware that can be depoyed, either via the development environment, or via the method described in the section [Deploying](#deploying).

Please see the [developing section of the generig Twomes firmware](https://github.com/energietransitie/twomes-generic-esp-firmware#developing) first. Reember to presse buttons to upload the firmware: 
* When you see the beginning of the sequence `conecting ......_____......`, press and hold the button labeled `GPIO0 (SW2)` on the PCB, then briefly press the button labeled `RESET (SW1)` shortly. 
* You should see an indication that the firmware is being written to the device.

## Features
List of features ready and TODOs for future development. 

Ready:
* Pairing with [Twomes P1 Gateway measurement device](https://github.com/energietransitie/twomes-p1-gateway-firmware)
* Measure temperatures using two DS18b20 sensors
* Sleep between conversions for ultra-low power consumption
* Send measurement data to [Twomes P1 Gateway measurement device](https://github.com/energietransitie/twomes-p1-gateway-firmware) using ESP-NOW

To-do:
* Implement more status- and error indicators using LEDs

## Pairing
The boiler monitor will send the measurements to a [Twomes P1 Gateway measurement device](https://github.com/energietransitie/twomes-p1-gateway-firmware) using the ESP-NOW protocol. The P1 Gateway listens on a specific 2.4 GHz channel, that is chosen after the provisioning of the P1 Gateway. To let the boiler monitor know on which channel the P1 Gateway expects the measurements, use the following procedure (provisioning of the ESP_NOW channel):
* Place the battery in the boiler monitor, make sure it is near the P1 Gateway to be able to see whether the provisioning is successful.
* On the boiler monitor, press and hold the `GPIO15 (SW2)` button (labeled `K` on the enclosure, which stands for the Dutch word "Koppelen") and then briefly press the `RESET (SW1)` button (labeled `R` on the enclosure), `GPIO15 (SW2) for about second, until the green LED turns on; then release the button. The green led will blink for 20 seconds to indicate that the boiler monitor listens (on 2.4 GHz channel 0) for the ESP_NOW channel information from the P1 Gateway.
* On the P1 Gateway, within these 20 seconds, press the `GPIO12 (SW2)` (labeled `K` on the enclosure, which stands for the Dutch word "Koppelen"). The P1 Gateway now transmits on channel 0 the information about which 2.4 GHz channel it expects the ESP-NOW messages, the green led blinks shortly during this transmission.
* On the boiler monitor, when the channel provisioning is successful, the green led will blink and after that the red led.
* This procedure can be repeated if needed, e.g. after renewed provisioning of the P1 Gateway.

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
