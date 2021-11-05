#include <Arduino.h>
#include <sensor_IO.h>
#include <OneWire.h>           //for DS18B20 support
#include <DallasTemperature.h> //for DS18B20 support
#include <WiFi.h>               //For setting WiFi mode REVIEW could this be done using the esp_wifi.h as well?
#include <esp_now.h>            //For sending and receiving ESP-NOW messages
#include <espnow_settings.h>    //Contains addresses and message limits
#include <twomes_sensor_pairing.h>
#include <esp_wifi.h>           //For setting channel
#include <nvs.h>                //For storing and reading Gateway MAC and channel
#include <rom/rtc.h>            //Get wakeup reason

//DEBUG DEFINES:
#define DEBUG 1              //Global debug define enable

#define DEBUG_BOOT 1            //Debug startup (Able to boot, check for temp-sensors, CPU frequency)
#define DEBUG_TEMPERATURE 1     //Debug Temperature measurements (Print temperature values)
#define DEBUG_TIME 1            //Debug the time adjustment value (Print time since last boot in us)
#define DEBUG_ESPNOW_SEND 1     //Debug for when sending ESP-Now messages (Print payload and destination MAC address)
#define DEBUG_CALLBACK 1        //Debug for ESP-Now callback function (de-initalising and check for ACK)
#define DEBUG_ERRORS 1          //Debug error-messages (init-failures)
#define DEBUG_PROVISIONING 1    //Debug sensor provisioning

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

#define ONEWIRE_BUS_PIN 25  //pin for OneWire bus
#define SUPERCAP_ENABLE 27  //Pull low to enable supercap

#define uS_TO_S_FACTOR 1000000ULL                    /* Conversion factor for micro seconds to seconds */
#define mS_TO_S_FACTOR 1000ULL                       /* Conversion factor for milli seconds to seconds */
#define TIME_TO_SLEEP 10                             /* Time between measurements */
#define INTERVAL_US (TIME_TO_SLEEP * uS_TO_S_FACTOR) /* desired interval between measurements in us */
#define TIME_TO_CONVERSION 750                         /* Time ESP32 will go to sleep for conversion(in milliseconds) */
#define RETRY_INTERVAL 5                             /* Amount of measurements before a new ESP-Now attempt after a Fail To Send */

#define PAIRING_TIMEOUT_uS 20*uS_TO_S_FACTOR        /* timeout for pairing mode*/

#define MAX_SAMPLES_MEMORY 60    //maximum memory allocated for samples, derived from maximum amount of samples that fit into a single message
#define ESPNOW_SEND_MINIMUM 20       //Minimum amount of measurements taken before ESP-Now transmissions are started

#if (MAX_SAMPLES_MEMORY != MAX_SAMPLES_ESPNOW)
#error This is not supported yet
#endif

//systemStates
enum systemStates {
    UNKNOWN,           //boot after power loss (RTC data cleared) or first boot
    INIT_MEASUREMENT,  //state where measurement is initialised
    READ_MEASUREMENT,   //state where measurement is read
    PROVISION_SENSOR    //State where sensor gets provisioned with channel and gateway MAC through ESP-Now
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
void onDataSent(const uint8_t *macAddress, esp_now_send_status_t status); //ESP-Now send callback function
void onDataReceive(const uint8_t *macAddress, const uint8_t *payload, int length); //ESP-Now receive callabck function

OneWire oneWireBus(ONEWIRE_BUS_PIN); //software OneWire bus

DallasTemperature tempSensors(&oneWireBus); //temperature object


RTC_DATA_ATTR int16_t temp1[MAX_SAMPLES_MEMORY];
RTC_DATA_ATTR int16_t temp2[MAX_SAMPLES_MEMORY];
RTC_DATA_ATTR bool waitForCallback = false;

//Twomes Measurement type enum:
enum ESPNOWdataTypes {
    BOILERTEMP,
    ROOMTEMP,
    CO2,
};

//NEW int16_t version to increase amount of measurements in single burst
typedef struct ESP_message {
    uint8_t measurementType = BOILERTEMP;  //Type of measurements
    uint8_t numberofMeasurements;                       //number of measurements in burst
    uint16_t index;                                     //Number identifying the message, only increments on receiving an ACK from Gateway. Could be uint8_t since overflows are ignored?
    uint16_t intervalTime = TIME_TO_SLEEP;               //Interval between measurements, for timestamping in gateway
    int16_t pipeTemps1[MAX_SAMPLES_ESPNOW];             //measurements of the first temperature sensor
    int16_t pipeTemps2[MAX_SAMPLES_ESPNOW];             //measurements of the second temperature sensor
} ESP_message;
ESP_message prepareMessage; //allocate space for this struct //Should/can this be done dynamically?

void setup() {
#if defined(DEBUG) //Serial only has to be started when debugging:
    Serial.begin(115200);
#endif
    pinMode(BUTTON_P1, INPUT_PULLUP);
    pinMode(BUTTON_P2, INPUT_PULLUP);
    pinMode(LED_STATUS, OUTPUT);
    pinMode(LED_ERROR, OUTPUT);
    pinMode(SUPERCAP_ENABLE, OUTPUT);
    digitalWrite(SUPERCAP_ENABLE, HIGH); //Pmos = active low

    //Get the wakeup reason:
    RESET_REASON reset_reason = rtc_get_reset_reason(PRO_CPU_NUM);

    //Check for P2 (GPIO15) pressed on boot
    if (reset_reason == POWERON_RESET) {
        systemState = systemStates::PROVISION_SENSOR;
    } //if(!digitalRead(BUTTON_P2))

    //Needs loop for lightsleep, since program counter is saved
    while (1) {
        //this is a state machine
        switch (systemState) {
            //First boot or power loss, re-initialise:
            case systemStates::UNKNOWN: //first run/sensors not connected?, this could be detection for hardware variant
#if defined(DEBUG) & defined(DEBUG_BOOT)
//Blink LED to indicat being in state "UNKNOWN"
                digitalWrite(LED_ERROR, HIGH);
                delay(50);
                digitalWrite(LED_ERROR, LOW);
                delay(50);
                digitalWrite(LED_ERROR, HIGH);
                delay(50);
                digitalWrite(LED_ERROR, LOW);
                Serial.println("Device has started");
                Serial.println("booted for the first time");
#endif
                tempSensors.begin();
                delay(50); //give time for initing
                //Check that both sensors are connected:
                if (tempSensors.getDeviceCount() != 2) {
#if defined(DEBUG) & defined(DEBUG_BOOT)
                    Serial.println("Missing temperature sensors!");
                    Serial.println(tempSensors.getDeviceCount());
#endif
                    delay(500);
                } //if getDeviceCount != 2
                else {
#if defined(DEBUG) & defined(DEBUG_BOOT)
                    Serial.println("Ready to start measuring!");
#endif
                    systemState = systemStates::INIT_MEASUREMENT;
                } //if get DeviceCount == 2
                break; //systemStates::UNKNOWN
            //init measurement
            case systemStates::INIT_MEASUREMENT:
                tempSensors.begin();                     //initialize sensor
                tempSensors.setWaitForConversion(false); //disable the wait for conversion
                tempSensors.requestTemperatures();       //start conversion
                esp_sleep_enable_timer_wakeup(TIME_TO_CONVERSION * mS_TO_S_FACTOR); //sleep for 750 ms during conversion

                systemState = systemStates::READ_MEASUREMENT; //set conversion is started bit

                esp_light_sleep_start(); //go to sleep

                break; //end of systemStates::INIT_MEASUREMENT
            case systemStates::READ_MEASUREMENT: //read measurements
            {
#if defined(DEBUG) & defined(DEBUG_BOOT)
                Serial.println("Device has started");
#endif
                //Theoretically speaking, currentMeasurement should in this case always be equal to MAX_SAMPLES_MEMORY, and never exceed it.
                if (currentMeasurement >= MAX_SAMPLES_MEMORY) {
                    //move all samples "RETRY_INTERVAL" amount of spaces in memory, overwriting "RETRY_INTERVAL" amount of old measurements and making room for "RETRY_INTERVAL" amount new ones.
                    memmove(temp1, &temp1[(currentMeasurement - MAX_SAMPLES_MEMORY) + RETRY_INTERVAL], (MAX_SAMPLES_MEMORY - RETRY_INTERVAL) * sizeof(temp1[0]));
                    memmove(temp2, &temp2[(currentMeasurement - MAX_SAMPLES_MEMORY) + RETRY_INTERVAL], (MAX_SAMPLES_MEMORY - RETRY_INTERVAL) * sizeof(temp2[0]));
                    currentMeasurement = MAX_SAMPLES_MEMORY - RETRY_INTERVAL;
                }//if(currentMeasurement >= MAX_SAMPLES_MEMORY)

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
                if ((temp1[currentMeasurement] == DEVICE_DISCONNECTED_RAW) || (temp2[currentMeasurement] == DEVICE_DISCONNECTED_RAW)) {
#if defined(DEBUG) & defined(DEBUG_TEMPERATURE)
                    Serial.println("\n\n\n\nNow everything should be deleted, and starting again"); //temp error message
#endif
                    currentMeasurement = 0;            //set to zero to reinit all.
                    systemState = systemStates::UNKNOWN; //go back to previous step
                    break;                             // init everything again
                }//if(temp1 || temp2)
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
                } //if(currentMEasurement<ESPNOW_SEND_MINIMUM)
                else if (currentMeasurement == ESPNOW_SEND_MINIMUM || (currentMeasurement > ESPNOW_SEND_MINIMUM && !((currentMeasurement - ESPNOW_SEND_MINIMUM) % RETRY_INTERVAL))) { //if there are enough samples to send
                    sendESPNOWmessage();
                    waitForCallback = true;
                    while (waitForCallback) {
#if defined(DEBUG) && defined(DEBUG_CALLBACK)
                        Serial.println("waiting for callback");
#endif
                        delay(500);
                    } //Wait for ESP-Now Callback function
                }//esle if (enough samples to send)

                systemState = systemStates::INIT_MEASUREMENT;
#ifdef DEBUG
                Serial.println("Entering Sleep mode");
#endif
#if (TIME_TO_SLEEP>5)  //at a delay of more than 10 
                esp_deep_sleep_start();
#else
                esp_light_sleep_start(); //go to sleep
#endif
            }//case READ_MEASUREMENT
            break;

            case systemStates::PROVISION_SENSOR:
            {
                Serial.println("In pairing mode");
                digitalWrite(SUPERCAP_ENABLE, LOW); //Enable supercap
                WiFi.mode(WIFI_STA); //Enter STA mode to enable ESP-Now
                if (esp_now_init() != ESP_OK) { //if initing wasn't succesful
#if defined(DEBUG) & defined(DEBUG_ERRORS)
                    Serial.println("Error initializing ESP-NOW");
#endif
                    esp_now_deinit();
                    WiFi.mode(WIFI_OFF);
                    break;
                }
                //Set the channel to the defined channel for Sensor pairing (Should be the same in the P1-Gateway!!)
                esp_wifi_set_channel(ESPNOW_PAIRING_CHANNEL, WIFI_SECOND_CHAN_NONE);

                esp_now_register_recv_cb(onDataReceive);
                int64_t startTime = esp_timer_get_time();
                //Set 20 second timeout on pairing:
                while ((startTime + PAIRING_TIMEOUT_uS > esp_timer_get_time())) {
                    digitalWrite(LED_STATUS, HIGH);
                    vTaskDelay(500);
                    digitalWrite(LED_STATUS, LOW);
                    vTaskDelay(500);
                }
                esp_now_unregister_recv_cb();
                esp_now_deinit();
                WiFi.mode(WIFI_OFF);
                digitalWrite(SUPERCAP_ENABLE, HIGH); //Close supercap
                systemState = systemStates::UNKNOWN;
            }//case PROVISION_SENSOR
            break;
            default:
            {
            }//default
            break;
        }// switch systemState
    } //while(1)
}

void sendESPNOWmessage() {
    //Enable SuperCap to assist with power:
    digitalWrite(SUPERCAP_ENABLE, LOW);

    //Get the MAC address and channel from NVS:
    uint8_t macAddress[6];
    uint8_t channel;
    esp_err_t err = getGatewayData(macAddress, sizeof(macAddress), &channel);
    if (err != ESP_OK) {
        ESP_LOGE("READ-CHANNEL", "Error trying to read data from NVS: %s", esp_err_to_name(err));
    }

    //Enable Wi-Fi in station mode, to allow ESP-Now
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) { //if initing wasn't succesful, exit
#if defined(DEBUG) & defined(DEBUG_ERRORS)
        Serial.println("Error initializing ESP-NOW");
#endif
        esp_now_deinit();
        WiFi.mode(WIFI_OFF);
        return;
    }

    esp_now_register_send_cb(onDataSent); //register callback to fetch send reaction

#if defined(DEBUG)
//print the read channel and MAC address
    Serial.print(" Now validating NVS address: ");
    for (uint8_t i = 0; i < 6;i++) {
        Serial.print(macAddress[i], HEX);
        Serial.print(':');
    }
    Serial.printf("Sending on channel %u", channel);
#endif
    // Add peer
    memcpy(peerInfo.peer_addr, macAddress, 6);
    //Before sending, retrieve the Wi-Fi channel of the local network:
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

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


    prepareMessage.numberofMeasurements = currentMeasurement;
    prepareMessage.index = burstNumber;
    //Copy the temperature data into the struct
    memcpy(prepareMessage.pipeTemps1, temp1, MAX_SAMPLES_MEMORY * sizeof(temp1[0]));
    memcpy(prepareMessage.pipeTemps2, temp2, MAX_SAMPLES_MEMORY * sizeof(temp2[0]));
#if defined(DEBUG_ESPNOW_SEND) & defined(DEBUG)
    // Serial.printf("[ESPNOW]: prepareMessage.numberofMeasurements = %u\n", prepareMessage.numberofMeasurements);
    // Serial.printf("[ESPNOW]: prepareMessage.intervalTime = %u\n", prepareMessage.intervalTime);
    // Serial.printf("[ESPNOW]: preparemessage.index = %u\n", prepareMessage.index);
    // for (uint8_t counter1 = 0; counter1 < prepareMessage.numberofMeasurements; counter1++) {
    //     // printf("[ESPNOW]: prepareMessage.pipeTemps1[%u] = %f\n", counter1, prepareMessage.pipeTemps1[counter1]); //Old float version
    //     // printf("[ESPNOW]: prepareMessage.pipeTemps2[%u] = %f\n", counter1, prepareMessage.pipeTemps2[counter1]);
    //     Serial.printf("[ESPNOW]: prepareMessage.pipeTemps1[%u] = %f\n", counter1, prepareMessage.pipeTemps1[counter1] * 0.0078125f); //New int16_t, print as float for debugging
    //     Serial.printf("[ESPNOW]: prepareMessage.pipeTemps2[%u] = %f\n", counter1, prepareMessage.pipeTemps2[counter1] * 0.0078125f);
    // }
    // Serial.printf("[ESPNOW]: This should be the message\n");
#endif

    //Send the data through ESP-Now
    esp_err_t result = esp_now_send(macAddress, (uint8_t *)&prepareMessage, sizeof(prepareMessage));

    //Check that the message was correctly transmitted 
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

//callback on ESP message sent, use status to check for ACK
void onDataSent(const uint8_t *macAddress, esp_now_send_status_t status) {
#if defined(DEBUG) & defined(DEBUG_CALLBACK)
    Serial.printf("[ESPNOW]: unregister cb: %d\n", esp_now_unregister_send_cb());
    // Serial.printf("[ESPNOW]: delete peer: %d\n", esp_now_del_peer(gatewayAddress));
    Serial.println(String("[ESP-Now] De-init ") + String((esp_now_deinit() ? "failed" : "Succes")));
    Serial.println(String("[WiFi] Turn off WiFi ") + String(WiFi.mode(WIFI_OFF) ? "Succes" : "Failed"));
#else
    esp_now_unregister_send_cb();
    esp_now_del_peer(gatewayAddress);
    esp_now_deinit();
    WiFi.mode(WIFI_OFF);
#endif
    digitalWrite(SUPERCAP_ENABLE, HIGH); //Disable supercap
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
