//time correction should be reviewed since the software is optimized

#include <Arduino.h>
#include <OneWire.h>           //for DS18B20 support
#include <DallasTemperature.h> //for DS18B20 support
#include <WiFi.h>
#include <esp_now.h>
#include <espnow_settings.h>
#include <esp_wifi.h>

//DEBUG DEFINES:
// #define DEBUG  1                //Global debug define enable

#define DEBUG_BOOT 1             //Debug startup (Able to boot, check for temp-sensors, CPU frequency)
#define DEBUG_TEMPERATURE 1     //Debug Temperature measurements (Print temperature values)
#define DEBUG_TIME 1            //Debug the time adjustment value (Print time since last boot in us)
#define DEBUG_ESPNOW_SEND 1     //Debug for when sending ESP-Now messages (Print payload and destination MAC address)
#define DEBUG_CALLBACK 1        //Debug for ESP-Now callback function (de-initalising and check for ACK)
#define DEBUG_ERRORS 1          //Debug error-messages (init-failures)

/** -------------------------------------------------/
 * TODO:
 * -Test sensor readings [V]
 * -set configurable settings using GPIO or Provisioning through Gateway []
 * - Change temp reading in float to temp reading in uint16_t [V]
 * - Put Debug/Serial operation in #if to disable for real tests [V]
----------------------------------------------------*/
#define hardwareFunction 1 //0: roomTemp, 1: boilerTemp

#define hardwarePin_sensor1 25 //DS18B20 temperature sensor 1/pipeTemp1
#define hardwarePin_sensor2 26 //DS18B20 temperature sensor 2/pipeTemp2

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 10 //Ten seconds between measurements
#define TIME_TO_CONVERSION 1 /* Time ESP32 will go to sleep for conversion(in seconds) */
#define INTERVAL_US (TIME_TO_SLEEP*uS_TO_S_FACTOR)  /* desired interval between conversions */
#define RETRY_INTERVAL 5     //The amount of new measurements before a ESP-Now retry after failure

#define maximum_samples_memory 60   //maximum memory allocated for samples
#define start_sample_sent_attempt 20 //amount of measurements where the sent attempts are starting

#if (maximum_samples_memory != maximum_samples_espnow)
#error This is not supported yet
#endif
//systemStates
#define systemState_unknown 0         //fresh start with new software
#define systemState_initMeasurement 1 //state where measurement is inited
#define systemState_readMeasurement 2 //state where inited measurement is read

RTC_DATA_ATTR uint8_t currentMeasurement = 0;    //variable in RTC memory where number of current measurement is stored
// RTC_DATA_ATTR uint8_t firstMeasurement = 0;      //variable in RTC memory where number of first measurement in array is stored, for forget previous samples while memory is full
RTC_DATA_ATTR uint8_t systemState = 0;           //variable in RTC memory where current system state is saved
RTC_DATA_ATTR uint64_t previous_time = 0;        //REVIEW
RTC_DATA_ATTR uint64_t time_correction = 400000; //REVIEW, initial time correction
RTC_DATA_ATTR uint16_t burstNumber = 0;         //Store the amount of databursts that have been done with ESP-Now

//Register peer
esp_now_peer_info_t peerInfo; //place to save ESPNOW peer info

//prototype functions
void sent_ESPNOW_message(); //function to send ESPNOW message

uint8_t gateway_mac_address[6] = destinationMacAddress;

OneWire data_Sensor1(hardwarePin_sensor1); //init software OneWire bus
OneWire data_Sensor2(hardwarePin_sensor2); //init software OneWire bus

DallasTemperature temperature_sensor1(&data_Sensor1); //init temperature bus
DallasTemperature temperature_sensor2(&data_Sensor2); //init temperature bus
/* OLD float method
typedef struct measurementFormat //struct to save temperatures on a standarized way
{
  float temperature1;
#if hardwareFunction
  float temperature2;
#endif
} measurementFormat;
*/

RTC_DATA_ATTR int16_t temperaturesIn[maximum_samples_memory];
RTC_DATA_ATTR int16_t temperaturesOut[maximum_samples_memory];
RTC_DATA_ATTR bool waitForCallback = false;

//NEW int16_t version
typedef struct ESP_message {
    uint8_t numberofMeasurements; //number of measurements
    uint16_t index;                                    //Number identifying the message, only increments on receiving an ACK from Gateway
    uint8_t intervalTime = defaultIntervalTime;        //(INTERVAL_US / 1000000); //intervalTime in milliSeconds REVIEW, this could be added?
    int16_t pipeTemps1[maximum_samples_espnow];
    int16_t pipeTemps2[maximum_samples_espnow];
} ESP_message;
ESP_message prepareMessage; //allocate space for this struct //Should/can this be done dynamically?

//call back on ESP message sent
void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
#if defined(DEBUG) & defined(DEBUG_CALLBACK)
    Serial.printf("[ESPNOW]: unregister cb: %d\n", esp_now_unregister_send_cb());
    Serial.printf("[ESPNOW]: delete peer: %d\n", esp_now_del_peer(gateway_mac_address));
    Serial.println(String("[ESP-Now] De-init ") + String((esp_now_deinit() ? "failed" : "Succes")));
    Serial.println(String("[WiFi] Turn off WiFi ") + String(WiFi.mode(WIFI_OFF) ? "Succes" : "Failed"));
#else
    esp_now_unregister_send_cb();
    esp_now_del_peer(gateway_mac_address);
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
    systemState = systemState_initMeasurement;
    waitForCallback = false;
    return;
}

//Function to get the channel of the local Wi-Fi network using supplied SSID:
int32_t getWiFiChannel(const char* ssid) {
    if (int32_t n = WiFi.scanNetworks()) {
        for (uint8_t i = 0; i < n; i++) {
            if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
                return WiFi.channel(i);
            }
        }
    }
    return 0;
}

void setup() {
#if defined(DEBUG) //Serial only has to be started when debugging:
    Serial.begin(115200);
#endif
    // setCpuFrequencyMhz(80); // to set the cpu frequency to 80Mhz to be more energy efficient - SHOULD BE DONE IN .ini FILE! (board_build.f_cpu=80000000L)
    memcpy(peerInfo.peer_addr, gateway_mac_address, 6); //add gateway mac to peerInfo
    peerInfo.channel = 0;                               //espnow_channel;
    peerInfo.encrypt = false;                           //espnow_encryption;
#if defined(DEBUG) & defined(DEBUG_BOOT)
    Serial.print("CPU frequency: ");
    Serial.println(getCpuFrequencyMhz());   //Check for80Mhz
#endif
    while (1) {
        switch (systemState) //this is a state machine
        {
        case systemState_unknown: //first run/sensors not connected?, this could be detection for hardware variant
#if defined(DEBUG) & defined(DEBUG_BOOT)
            Serial.println("Device has started");
            Serial.println("booted for the first time");
#endif
            temperature_sensor1.begin();
            temperature_sensor2.begin();
            delay(10); //give time for initing

            if (temperature_sensor1.getDeviceCount()) {
#if defined(DEBUG) & defined(DEBUG_BOOT)
                Serial.printf("Found temperature_sensor1\n");
#endif
                if (temperature_sensor2.getDeviceCount()) {
#if defined(DEBUG) & defined(DEBUG_BOOT)
                    Serial.printf("Found temperature_sensor2, complete so start measurements\n");
#endif
                    systemState = systemState_initMeasurement;
                } else {
#if defined(DEBUG) & defined(DEBUG_BOOT)
                    Serial.printf("Did not find temperature_sensor2\n");
#endif
                    delay(1000);
                }
            } else {
#if defined(DEBUG) & defined(DEBUG_BOOT)
                Serial.printf("Did not find temperature_sensor1\n");
#endif
                delay(1000);
            }

            break;
        case systemState_initMeasurement:                  //init measurement
            temperature_sensor1.begin();                     //initialize sensor
            temperature_sensor1.setWaitForConversion(false); //disable the wait for conversion
            temperature_sensor1.requestTemperatures();       //start conversion
            temperature_sensor2.begin();                     //initialize sensor
            temperature_sensor2.setWaitForConversion(false); //disable the wait for conversion
            temperature_sensor2.requestTemperatures();       //start conversion

            esp_sleep_enable_timer_wakeup(TIME_TO_CONVERSION * uS_TO_S_FACTOR); //sleep for 1 second during conversion

            //Serial.println(wake_after_conversion);
            //Serial.println("Going to sleep now");
            //Serial.flush();
            systemState = systemState_readMeasurement; //set conversion is started bit

            esp_light_sleep_start(); //go to deepsleep <- Should this be light sleep? (Power measurement!)
            break;
        case systemState_readMeasurement: //read measurements
        {
#if defined(DEBUG) & defined(DEBUG_BOOT)
            Serial.println("Device has started");
#endif
            temperature_sensor1.begin(); //initialize again to find the device adress of the sensor
            temperature_sensor2.begin(); //initialize again to find the device adress of the sensor
            /** OLD
            measurement[currentMeasurement].temperature1 = temperature_sensor1.getTempCByIndex(0); // get the temperature of the sensor
            measurement[currentMeasurement].temperature2 = temperature_sensor2.getTempCByIndex(0); // get the temperature of the sensor
            */
            if (currentMeasurement >= maximum_samples_memory) {
                //move all samples "RETRY_INTERVAL" amount of spaces in memory, overwriting "RETRY_INTERVAL" old measurements and making room for "RETRY_INTERVAL" new ones.
                //Theoretically speaking, currentMeasurement should in this case always be equal to maximum_samples_memor, and never exceed it. 
                memmove(temperaturesIn, &temperaturesIn[(currentMeasurement - maximum_samples_memory) + RETRY_INTERVAL], (maximum_samples_memory - RETRY_INTERVAL) * sizeof(temperaturesIn[0]));
                memmove(temperaturesOut, &temperaturesOut[(currentMeasurement - maximum_samples_memory) + RETRY_INTERVAL], (maximum_samples_memory - RETRY_INTERVAL) * sizeof(temperaturesOut[0]));
                currentMeasurement = maximum_samples_memory - RETRY_INTERVAL;
            }

            DeviceAddress address1;
            DeviceAddress address2;
            temperature_sensor1.getAddress(address1, 0);
            temperature_sensor2.getAddress(address2, 0);
            temperaturesIn[currentMeasurement] = temperature_sensor1.getTemp(address1);
            temperaturesOut[currentMeasurement] = temperature_sensor2.getTemp(address2);
#if defined(DEBUG) & defined(DEBUG_TEMPERATURE)
            Serial.print("Temp1: ");
            Serial.println(temperaturesIn[currentMeasurement], HEX);
            Serial.print("Temp2: ");
            Serial.println(temperaturesOut[currentMeasurement], HEX);
            Serial.print("Celsius values: ");
            Serial.print("Temp 1: ");
            Serial.print(temperaturesIn[currentMeasurement] * 0.0078125f); //Conversion to Celsius from DallasTemperature library to check if HEX value was read correctly
            Serial.print("Temp 2: ");
            Serial.println(temperaturesOut[currentMeasurement] * 0.0078125f); //Conversion to Celsius from DallasTemperature library to check if HEX value was read correctly
#endif
            if ((temperaturesIn[currentMeasurement] == -127) || (temperaturesOut[currentMeasurement] == -127)) //REVIEW, this fault should be fetched on a better way
            {
#if defined(DEBUG) & defined(DEBUG_TEMPERATURE)
                Serial.println("\n\n\n\nNow everything should be deleted, and starting again"); //temp error message
#endif
                currentMeasurement = 0;            //set to zero to reinit all.
                systemState = systemState_unknown; //go back to previous step
                break;                             // init everything again
            }

            currentMeasurement++; //Add to currentMeasurement AFTER measuring, but BEFORE sending! (0 indexing for writing to memory, but not for comparing to ready_to_send)
#if defined(DEBUG) & defined(DEBUG_TEMPERATURE)
            Serial.printf("\n\n\ncurrentMeasurement: %u\n", currentMeasurement);
#endif
            struct timeval tv;
            gettimeofday(&tv, NULL);
            int64_t time_us = (int64_t)tv.tv_sec * 1000000L + (int64_t)tv.tv_usec; //code needed to get the time in microseconds
            int32_t time_delta = time_us - previous_time;                          //calculate the time that is elapsed since the last measurement
            previous_time = time_us;                                               // write current time to memory for next cycle
#if defined(DEBUG) & defined(DEBUG_TIME)
            Serial.println(time_delta);
#endif
            time_correction += (time_delta - INTERVAL_US); // finetune time correction
            esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR - time_correction); //sleep until next temperature reading

            if (currentMeasurement < start_sample_sent_attempt) //if there are not enough samples yet
            {
#if defined(DEBUG) & defined(DEBUG_TEMPERATURE)
                Serial.printf("Collecting new measurement\n");
#endif
            } else if (currentMeasurement == start_sample_sent_attempt || (currentMeasurement > start_sample_sent_attempt && !((currentMeasurement - start_sample_sent_attempt) % RETRY_INTERVAL))) { //if there are enough samples to send
                sent_ESPNOW_message();
                waitForCallback = true;
                while (waitForCallback) {
#if defined(DEBUG) && defined(DEBUG_CALLBACK)
                    Serial.println("waiting for callback");
#endif
                    delay(500);
                } //Wait for ESP-Now Callback function 
            }

            systemState = systemState_initMeasurement;
#ifdef DEBUG
            Serial.println("Entering Sleep mode");
#endif
            esp_light_sleep_start(); //go to deepsleep

        }
        break;
        }
    }
}



void sent_ESPNOW_message() {
    uint8_t channel = 1;
    WiFi.mode(WIFI_STA);          //this mode is required for ESP NOW, if you forget this the CPU will panic.
    if (esp_now_init() != ESP_OK) {//if initing wasn't succesful
#if defined(DEBUG) & defined(DEBUG_ERRORS)
        Serial.println("Error initializing ESP-NOW");
#endif
        esp_now_deinit();
        WiFi.mode(WIFI_OFF);
        return;
    }
    esp_now_register_send_cb(OnDataSent); //register call back to fetch send reaction

    // Add peer
    memcpy(peerInfo.peer_addr, gateway_mac_address, 6);
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
    memcpy(prepareMessage.pipeTemps1, temperaturesIn, maximum_samples_memory * sizeof(temperaturesIn[0])); //REVIEW should this be currentMeasurement*sizeof(temperaturesIn[0])?
    memcpy(prepareMessage.pipeTemps2, temperaturesOut, maximum_samples_memory * sizeof(temperaturesOut[0]));
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

    esp_err_t result = esp_now_send(gateway_mac_address, (uint8_t*)&prepareMessage, sizeof(prepareMessage));

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
    else return;
#endif
}

void loop() {}