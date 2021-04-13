# Temperature monitor
Firmware for ESP32 to read and parse data from
* Two DS18B20 temperature sensors, this is boilerTemp variant
* One SI7051 temperature sensor, this is roomTemp variant

send this measurements with ESPNOW to the gateway, this should be low power since this hardware is powered from a battery.

## TODO's and considerations
* ~~Read measurements from DS18B20~~
* implement/merge compability with SI7051
* ~~Implement ESPNOW for gateway communication~~
* ~~Buffer measurements if ESPNOW messages are ignored~~
* optimize ESPNOW message building/memory allocation
* Add button functionality
* Sync destination MAC by BLE
* Remove debugging code
* improve/review timing/ system states
* improve low power performance



## Dependencies
```
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <esp_now.h>
#include <espnow_settings.h>
```

## Header files for program settings
The file espnow_setting.h.bak contain program enviroment variables. To use the simply remove .bak from the file name and fill in the correct values
