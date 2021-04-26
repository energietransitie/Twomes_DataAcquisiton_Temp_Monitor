#include <Arduino.h>
#include <OneWire.h>           //for DS18B20 support
#include <DallasTemperature.h> //for DS18B20 support
#include <WiFi.h>               //For setting WiFi mode REVIEW could this be done using the esp_wifi.h as well?
#include <esp_now.h>            //For sending and receiving ESP-NOW messages
#include <espnow_settings.h>    //Contains addresses and message limits
#include <esp_wifi.h>           //For setting channel

//DEBUG DEFINES:
#define DEBUG 1              //Global debug define enable

#define DEBUG_BOOT 1            //Debug startup (Able to boot, check for temp-sensors, CPU frequency)
#define DEBUG_TEMPERATURE 1     //Debug Temperature measurements (Print temperature values)
#define DEBUG_TIME 1            //Debug the time adjustment value (Print time since last boot in us)
#define DEBUG_ESPNOW_SEND 1     //Debug for when sending ESP-Now messages (Print payload and destination MAC address)
#define DEBUG_CALLBACK 1        //Debug for ESP-Now callback function (de-initalising and check for ACK)
#define DEBUG_ERRORS 1          //Debug error-messages (init-failures)

/** -------------------------------------------------/
 * @todo:
 * - Test sensor readings [V]
 * - set configurable settings using GPIO or Provisioning through Gateway []
 * - Change temp reading in float to temp reading in uint16_t [V]
 * - Put Debug/Serial operation in #if to disable for real tests [V]
 * - Improve code readability []
 * - implement support for roomTemp version (si7051 sensor) [] //REVIEW should this be entirely seperate code?
 * - automate deep- or light sleep depending on interval time [] (light sleep for <=10 seconds, deep sleep for >10 seconds)
----------------------------------------------------*/

#define ONEWIRE_BUS_PIN 25 //pin for OneWire bus

#define uS_TO_S_FACTOR 1000000ULL                    /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 10                             /* Time between measurements */
#define INTERVAL_US (TIME_TO_SLEEP * uS_TO_S_FACTOR) /* desired interval between measurements in us */
#define TIME_TO_CONVERSION 1                         /* Time ESP32 will go to sleep for conversion(in seconds) */
#define RETRY_INTERVAL 5                             /* Amount of measurements before a new ESP-Now attempt after a Fail To Send */

#define MAX_SAMPLES_MEMORY 60    //maximum memory allocated for samples, derived from maximum amount of samples that fit into a single message
#define ESPNOW_SEND_MINIMUM 20       //Minimum amount of measurements taken before ESP-Now transmissions are started

#if (MAX_SAMPLES_MEMORY != MAX_SAMPLES_ESPNOW)
#error This is not supported yet
#endif

//systemStates
enum systemStates {
    UNKNOWN,           //boot after power loss (RTC data cleared) or first boot
    INIT_MEASUREMENT,  //state where measurement is initialised
    READ_MEASUREMENT   //state where measurement is read
}; /* systemstates*/
RTC_DATA_ATTR systemStates systemState = UNKNOWN;           //variable in RTC memory where current system state is saved

RTC_DATA_ATTR uint8_t currentMeasurement = 0;       //variable in RTC memory where number of current measurement is stored
RTC_DATA_ATTR uint64_t previousTime = 0;            //REVIEW
RTC_DATA_ATTR uint64_t time_correction = 200000;    //REVIEW, initial time correction
RTC_DATA_ATTR uint16_t burstNumber = 0;             //Store the amount of databursts that have been done with ESP-Now, for syncing with gateway

//Register peer
esp_now_peer_info_t peerInfo; //place to save ESPNOW peer info

//prototype functions
void sendESPNOWmessage(); //function to send ESPNOW message
void onDataSent(const uint8_t *macAddress, esp_now_send_status_t status); //ESP-Now callback function

uint8_t gatewayAddress[6] = GATEWAY_MAC_ADDRESS;    //Temporary declarations, will be read from NVS later

OneWire oneWireBus(ONEWIRE_BUS_PIN); //init software OneWire bus

DallasTemperature tempSensors(&oneWireBus); //init temperature bus


RTC_DATA_ATTR int16_t temp1[MAX_SAMPLES_MEMORY];
RTC_DATA_ATTR int16_t temp2[MAX_SAMPLES_MEMORY];
RTC_DATA_ATTR bool waitForCallback = false;

//NEW int16_t version to increase amount of measurements in single burst
typedef struct ESP_message {
    uint8_t numberofMeasurements;               //number of measurements
    uint16_t index;                             //Number identifying the message, only increments on receiving an ACK from Gateway
    uint8_t intervalTime = TIME_TO_SLEEP;       //Interval between measurements, for timestamping in gateway
    int16_t pipeTemps1[MAX_SAMPLES_ESPNOW];     //measurements of the first temperature sensor
    int16_t pipeTemps2[MAX_SAMPLES_ESPNOW];     //measurements of the second temperature sensor
} ESP_message;
ESP_message prepareMessage; //allocate space for this struct //Should/can this be done dynamically?


void setup() {
#if defined(DEBUG) //Serial only has to be started when debugging:
    Serial.begin(115200);
#endif
    //Needs loop for lightsleep, since program counter is saved
    while (1) {
        //this is a state machine
        switch (systemState) {
            //First boot or power loss, re-initialise:
            case systemStates::UNKNOWN: //first run/sensors not connected?, this could be detection for hardware variant
#if defined(DEBUG) & defined(DEBUG_BOOT)
                Serial.println("Device has started");
                Serial.println("booted for the first time");
#endif
                tempSensors.begin();
                delay(50); //give time for initing
                //Check that both sensors are connected:
                if (tempSensors.getDeviceCount() != 2) {
#if defined(DEBUG) & defined(DEBUG_BOOT)
                    Serial.println("Missing temperature sensors!");
#endif
                    delay(500);
                } //if getDeviceCount != 2
                else {
#if defined(DEBUG) & defined(DEBUG_BOOT)
                    Serial.println("Ready to start measuring!");
#endif
                    systemState = systemStates::INIT_MEASUREMENT;
                } //if get DeviceCount == 2
                break; //end of systemStates::UNKNOWN
            //init measurement
            case systemStates::INIT_MEASUREMENT:
                tempSensors.begin();                     //initialize sensor
                tempSensors.setWaitForConversion(false); //disable the wait for conversion
                tempSensors.requestTemperatures();       //start conversion

                esp_sleep_enable_timer_wakeup(TIME_TO_CONVERSION * uS_TO_S_FACTOR); //sleep for 1 second during conversion

                systemState = systemStates::READ_MEASUREMENT; //set conversion is started bit
#if (TIME_TO_SLEEP>10)
                esp_deep_sleep_start();
#else
                esp_light_sleep_start(); //go to sleep
#endif
                break; //end of systemStates::INIT_MEASUREMENT
            case systemStates::READ_MEASUREMENT: //read measurements
            {
#if defined(DEBUG) & defined(DEBUG_BOOT)
                Serial.println("Device has started");
#endif
                tempSensors.begin(); //initialize again to find the device adress of the sensor
                //Theoretically speaking, currentMeasurement should in this case always be equal to MAX_SAMPLES_MEMORY, and never exceed it.
                if (currentMeasurement >= MAX_SAMPLES_MEMORY) {
                    //move all samples "RETRY_INTERVAL" amount of spaces in memory, overwriting "RETRY_INTERVAL" amount of old measurements and making room for "RETRY_INTERVAL" amount new ones.
                    memmove(temp1, &temp1[(currentMeasurement - MAX_SAMPLES_MEMORY) + RETRY_INTERVAL], (MAX_SAMPLES_MEMORY - RETRY_INTERVAL) * sizeof(temp1[0]));
                    memmove(temp2, &temp2[(currentMeasurement - MAX_SAMPLES_MEMORY) + RETRY_INTERVAL], (MAX_SAMPLES_MEMORY - RETRY_INTERVAL) * sizeof(temp2[0]));
                    currentMeasurement = MAX_SAMPLES_MEMORY - RETRY_INTERVAL;
                }

                DeviceAddress address1;
                DeviceAddress address2;
                tempSensors.getAddress(address1, 0);
                tempSensors.getAddress(address2, 1); //Using same bus
                temp1[currentMeasurement] = tempSensors.getTemp(address1);
                temp2[currentMeasurement] = tempSensors.getTemp(address2); //Using same bus
#if defined(DEBUG) & defined(DEBUG_TEMPERATURE)
                Serial.print("Temp1: ");
                Serial.println(temp1[currentMeasurement], HEX);
                Serial.print("Temp2: ");
                Serial.println(temp2[currentMeasurement], HEX);
                Serial.print("Celsius values: ");
                Serial.print("Temp 1: ");
                Serial.print(temp1[currentMeasurement] * 0.0078125f); //Conversion to Celsius from DallasTemperature library to check if HEX value was read correctly
                Serial.print("Temp 2: ");
                Serial.println(temp2[currentMeasurement] * 0.0078125f); //Conversion to Celsius from DallasTemperature library to check if HEX value was read correctly
#endif
                if ((temp1[currentMeasurement] == DEVICE_DISCONNECTED_RAW) || (temp2[currentMeasurement] == DEVICE_DISCONNECTED_RAW))
                {
#if defined(DEBUG) & defined(DEBUG_TEMPERATURE)
                    Serial.println("\n\n\n\nNow everything should be deleted, and starting again"); //temp error message
#endif
                    currentMeasurement = 0;            //set to zero to reinit all.
                    systemState = systemStates::UNKNOWN; //go back to previous step
                    break;                             // init everything again
                }
                currentMeasurement++; //Add to currentMeasurement AFTER measuring, but BEFORE sending! (0 indexing for writing to memory, but not for comparing to ready_to_send)
#if defined(DEBUG) & defined(DEBUG_TEMPERATURE)
                Serial.printf("\n\n\ncurrentMeasurement: %u\n", currentMeasurement);
#endif
                struct timeval tv;
                gettimeofday(&tv, NULL);
                int64_t time_us = (int64_t)tv.tv_sec * 1000000L + (int64_t)tv.tv_usec; //code needed to get the time in microseconds
                int32_t time_delta = time_us - previousTime;                          //calculate the time that is elapsed since the last measurement
                previousTime = time_us;                                               // write current time to memory for next cycle
#if defined(DEBUG) & defined(DEBUG_TIME)
                Serial.println(time_delta);
#endif
                time_correction += (time_delta - INTERVAL_US);                                   // finetune time correction
                esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR - time_correction); //sleep until next temperature reading
                //if there are not enough samples yet
                if (currentMeasurement < ESPNOW_SEND_MINIMUM) {
#if defined(DEBUG) & defined(DEBUG_TEMPERATURE)
                    Serial.printf("Collecting new measurement\n");
#endif
                }
                else if (currentMeasurement == ESPNOW_SEND_MINIMUM || (currentMeasurement > ESPNOW_SEND_MINIMUM && !((currentMeasurement - ESPNOW_SEND_MINIMUM) % RETRY_INTERVAL))) { //if there are enough samples to send
                    sendESPNOWmessage();
                    waitForCallback = true;
                    while (waitForCallback) {
#if defined(DEBUG) && defined(DEBUG_CALLBACK)
                        Serial.println("waiting for callback");
#endif
                        delay(500);
                    } //Wait for ESP-Now Callback function
                }

                systemState = systemStates::INIT_MEASUREMENT;
#ifdef DEBUG
                Serial.println("Entering Sleep mode");
#endif
#if (TIME_TO_SLEEP>10)
                esp_deep_sleep_start();
#else
                esp_light_sleep_start(); //go to sleep
#endif
            }
            break;
        }
    } // switch systemState
}

void sendESPNOWmessage() {
    uint8_t channel = 1;
    WiFi.mode(WIFI_STA); //this mode is required for ESP NOW, if you forget this the CPU will panic.
    if (esp_now_init() != ESP_OK) { //if initing wasn't succesful
#if defined(DEBUG) & defined(DEBUG_ERRORS)
        Serial.println("Error initializing ESP-NOW");
#endif
        esp_now_deinit();
        WiFi.mode(WIFI_OFF);
        return;
    }
    esp_now_register_send_cb(onDataSent); //register call back to fetch send reaction

    // Add peer
    memcpy(peerInfo.peer_addr, gatewayAddress, 6);
    //Before sending, retrieve the Wi-Fi channel of the local network:
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
    if (esp_now_add_peer(&peerInfo) != ESP_OK) //add to peer
    {
#if defined(DEBUG) & defined(DEBUG_ERRORS)
        Serial.println("Failed to add peer");
#endif
        esp_now_deinit();
        WiFi.mode(WIFI_OFF);
        //esp_now_del_peer(&peerInfo.peer_addr);
        return;
    }
#if defined(DEBUG) & defined(DEBUG_ESPNOW_SEND)
    Serial.println("Connected");
#endif

    prepareMessage.numberofMeasurements = currentMeasurement;
    prepareMessage.index = burstNumber;
    memcpy(prepareMessage.pipeTemps1, temp1, MAX_SAMPLES_MEMORY * sizeof(temp1[0]));
    memcpy(prepareMessage.pipeTemps2, temp2, MAX_SAMPLES_MEMORY * sizeof(temp2[0]));
#if defined(DEBUG_ESPNOW_SEND) & defined(DEBUG)
    Serial.printf("[ESPNOW]: prepareMessage.numberofMeasurements = %u\n", prepareMessage.numberofMeasurements);
    Serial.printf("[ESPNOW]: prepareMessage.intervalTime = %u\n", prepareMessage.intervalTime);
    Serial.printf("[ESPNOW]: preparemessage.index = %u\n", prepareMessage.index);
    for (uint8_t counter1 = 0; counter1 < prepareMessage.numberofMeasurements; counter1++) {
        // printf("[ESPNOW]: prepareMessage.pipeTemps1[%u] = %f\n", counter1, prepareMessage.pipeTemps1[counter1]); //Old float version
        // printf("[ESPNOW]: prepareMessage.pipeTemps2[%u] = %f\n", counter1, prepareMessage.pipeTemps2[counter1]);
        Serial.printf("[ESPNOW]: prepareMessage.pipeTemps1[%u] = %f\n", counter1, prepareMessage.pipeTemps1[counter1] * 0.0078125f); //New int16_t, print as float for debugging
        Serial.printf("[ESPNOW]: prepareMessage.pipeTemps2[%u] = %f\n", counter1, prepareMessage.pipeTemps2[counter1] * 0.0078125f);
    }
    Serial.printf("[ESPNOW]: This should be the message\n");
#endif

    esp_err_t result = esp_now_send(gatewayAddress, (uint8_t *)&prepareMessage, sizeof(prepareMessage));

    if (result != ESP_OK) {
#if defined(DEBUG) & defined(DEBUG_ESPNOW_SEND)
        Serial.printf("Error sending the data, with return %u\n", result);
#endif
        esp_now_deinit();
        WiFi.mode(WIFI_OFF);
        return;
    }
#if defined(DEBUG) & defined(DEBUG_ESPNOW_SEND)
    else {
        Serial.print("[ESPNOW]: Sent with success to address: ");
        for (uint8_t i = 0; i < 6; i++) {
            Serial.print((peerInfo.peer_addr[i]), HEX);
            Serial.print(":");
        }
        Serial.println();
    }
#else
    else
        return;
#endif
}

//call back on ESP message sent
void onDataSent(const uint8_t *macAddress, esp_now_send_status_t status) {
#if defined(DEBUG) & defined(DEBUG_CALLBACK)
    Serial.printf("[ESPNOW]: unregister cb: %d\n", esp_now_unregister_send_cb());
    Serial.printf("[ESPNOW]: delete peer: %d\n", esp_now_del_peer(gatewayAddress));
    Serial.println(String("[ESP-Now] De-init ") + String((esp_now_deinit() ? "failed" : "Succes")));
    Serial.println(String("[WiFi] Turn off WiFi ") + String(WiFi.mode(WIFI_OFF) ? "Succes" : "Failed"));
#else
    esp_now_unregister_send_cb();
    esp_now_del_peer(gatewayAddress);
    esp_now_deinit();
    WiFi.mode(WIFI_OFF);
#endif
#if defined(DEBUG) & defined(DEBUG_CALLBACK)
    Serial.print("\r\nLast Packet Send Status: ");
#endif
    if (status == ESP_NOW_SEND_SUCCESS) {
#if defined(DEBUG) & defined(DEBUG_CALLBACK)
        Serial.printf("Delivery Success, so currentMeasurement =0\n");
#endif
        currentMeasurement = 0; //zero this if measurement succeed
        burstNumber++;          //Add to the total amount of samples sent
    }
#if defined(DEBUG) & defined(DEBUG_CALLBACK)
    else {
        Serial.printf("Delivery Fail\n");
    }
#endif
    //Go to sleep only after ESP_Now message is completely processed

    waitForCallback = false;
    return;
}


void loop() {}