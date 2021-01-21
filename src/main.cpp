//time correction should be reviewed since the software is optimized

#include <Arduino.h>
#include <OneWire.h>           //for DS18B20 support
#include <DallasTemperature.h> //for DS18B20 support
#include <WiFi.h>
#include <esp_now.h>
#include <espnow_settings.h>

#define hardwareFunction 1 //0: roomTemp, 1: boilerTemp

#define hardwarePin_sensor1 25 //DS18B20 temperature sensor 1/pipeTemp1
#define hardwarePin_sensor2 26 //DS18B20 temperature sensor 2/pipeTemp2

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 2           //4           /* Time ESP32 will go to sleep (in seconds) */
#define TIME_TO_CONVERSION 1      /* Time ESP32 will go to sleep for conversion(in seconds) */
#define INTERVAL_US 5000000       /* desired interval between conversions */

#define maximum_samples_memory 20   //maximum memory allocated for samples
#define start_sample_sent_attempt 10 //amount of measurements where the sent attempts are starting

#if (maximum_samples_memory != maximum_samples_espnow)
#error This is not supported yet
#endif

//systemStates
#define systemState_unknown 0         //fresh start with new software
#define systemState_initMeasurement 1 //state where measurement is inited
#define systemState_readMeasurement 2 //state where inited measurement is readed

RTC_DATA_ATTR uint8_t currentMeasurement = 0;    //variable in RTC memory where number of current measurement is stored
RTC_DATA_ATTR uint8_t firstMeasurement = 0;      //variable in RTC memory where number of first measurement in array is stored, for forget previous samples while memory is full
RTC_DATA_ATTR uint8_t systemState = 0;           //variable in RTC memory where current system state is saved
RTC_DATA_ATTR uint64_t previous_time = 0;        //REVIEW
RTC_DATA_ATTR uint64_t time_correction = 400000; //REVIEW, initial time correction

//Register peer
esp_now_peer_info_t peerInfo; //place to save ESPNOW peer info

//prototype functions
void sent_ESPNOW_message(); //function to send ESPNOW message

uint8_t gateway_mac_address[] = destinationMacAddress;

OneWire data_Sensor1(hardwarePin_sensor1); //init software OneWire bus
OneWire data_Sensor2(hardwarePin_sensor2); //init software OneWire bus

DallasTemperature temperature_sensor1(&data_Sensor1); //init temperature bus
DallasTemperature temperature_sensor2(&data_Sensor2); //init temperature bus

typedef struct measurementFormat //struct to save temperatures on a standarized way
{
  float temperature1;
#if hardwareFunction
  float temperature2;
#endif
} measurementFormat;

RTC_DATA_ATTR measurementFormat measurement[maximum_samples_memory]; //allocate space in RTC memory for saving measurements

typedef struct ESP_message //struct to save ESP message on a standarized way/ compatible with gateway
{
  uint8_t numberofMeasurements = currentMeasurement; //number of measurements
  uint8_t intervalTime = defaultIntervalTime;        //(INTERVAL_US / 1000000); //intervalTime in milliSeconds REVIEW, this could be added?
  float pipeTemps1[maximum_samples_espnow];
  float pipeTemps2[maximum_samples_espnow];
} ESP_message;

ESP_message prepareMessage; //allocate space for this struct

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) //call back on ESP message sent
{
  Serial.printf("[ESPNOW]: unregister cb: %d\n", esp_now_unregister_send_cb());
  Serial.printf("[ESPNOW]: delete peer: %d\n", esp_now_del_peer(gateway_mac_address));
  esp_now_deinit();
  WiFi.mode(WIFI_OFF);

  Serial.print("\r\nLast Packet Send Status:\t");
  if (status == ESP_NOW_SEND_SUCCESS)
  {
    Serial.printf("Delivery Success, so currentMeasurement =0\n");
    currentMeasurement = 0; //zero this if measurement succeed
    firstMeasurement = 0; //zero this if measurement succeed
  }
  else
  {
    Serial.printf("Delivery Fail\n");
  }
}

#define debugMessages 1

void setup()
{
  //setCpuFrequencyMhz(80); // to set the cpu frequency to 80Mhz to be more energy efficient
  memcpy(peerInfo.peer_addr, gateway_mac_address, 6); //add gateway mac to peerInfo
  peerInfo.channel = 0;                               //espnow_channel;
  peerInfo.encrypt = false;                           //espnow_encryption;

  Serial.begin(115200); //use for debugging

  while (1)
  {
    switch (systemState) //this is a state machine
    {
    case systemState_unknown: //first run/sensors not connected?, this could be detection for hardware variant
      Serial.println("Device has started");
      Serial.println("booted for the first time");
      temperature_sensor1.begin();
      temperature_sensor2.begin();
      delay(10); //give time for initing

      if (temperature_sensor1.getDeviceCount())
      {
        Serial.printf("Found temperature_sensor1\n");
        if (temperature_sensor2.getDeviceCount())
        {
          Serial.printf("Found temperature_sensor2, complete so start measurements\n");
          systemState = systemState_initMeasurement;
        }
        else
        {
          Serial.printf("Did not find temperature_sensor2\n");
          delay(1000);
        }
      }
      else
      {
        Serial.printf("Did not find temperature_sensor1\n");
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

      esp_deep_sleep_start(); //go to deepsleep
      break;
    case systemState_readMeasurement: //read measurements
    {
#if debugMessages
      Serial.println("Device has started");
#endif
      temperature_sensor1.begin(); //initialize again to find the device adress of the sensor
      temperature_sensor2.begin(); //initialize again to find the device adress of the sensor

      measurement[currentMeasurement].temperature1 = temperature_sensor1.getTempCByIndex(0); // get the temperature of the sensor
      measurement[currentMeasurement].temperature2 = temperature_sensor2.getTempCByIndex(0); // get the temperature of the sensor

      if ((measurement[currentMeasurement].temperature1 == -127) || (measurement[currentMeasurement].temperature2 == -127)) //REVIEW, this fault should be fetched on a better way
      {
#if debugMessages
        Serial.println("\n\n\n\n\n\n\n\n\nNow everything should be deleted, and starting again"); //temp error message
#endif
        currentMeasurement = 0;            //set to zero to reinit all.
        systemState = systemState_unknown; //go back to previous step
        break;                             // init everything again
      }

      if (firstMeasurement == 0 && currentMeasurement < maximum_samples_memory)
      {
        currentMeasurement++; //step to next measurement REVIEW, block this when memory is full
        Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\ncurrentMeasurement: %u\n", currentMeasurement);
        Serial.printf("firstMeasurement: %u\n", firstMeasurement);
      }
      else if (currentMeasurement >= maximum_samples_memory)
      {
        currentMeasurement = 0;
        firstMeasurement = 1;
        Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\ncurrentMeasurement: %u\n", currentMeasurement);
        Serial.printf("firstMeasurement: %u\n", firstMeasurement);
      }
      else if (firstMeasurement == maximum_samples_memory)
      {
        currentMeasurement++;
        firstMeasurement = 0;
        Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\ncurrentMeasurement: %u\n", currentMeasurement);
        Serial.printf("firstMeasurement: %u\n", firstMeasurement);
        ;
      }
      else
      {
        currentMeasurement++;
        firstMeasurement++;
        Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\ncurrentMeasurement: %u\n", currentMeasurement);
        Serial.printf("firstMeasurement: %u\n", firstMeasurement);
      }

      struct timeval tv;
      gettimeofday(&tv, NULL);
      int64_t time_us = (int64_t)tv.tv_sec * 1000000L + (int64_t)tv.tv_usec; //code needed to get the time in microseconds
      int32_t time_delta = time_us - previous_time;                          //calculate the time that is elapsed since the last measurement
      previous_time = time_us;                                               // write current time to memory for next cycle
      Serial.println(time_delta);
      time_correction += (time_delta - INTERVAL_US); // finetune time correction
      //Serial.println((int32_t)time_correction);
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR - time_correction); //sleep until next temperature reading
      //Serial.println(wake_after_conversion);
      //Serial.println("Going to sleep now");
      //Serial.flush();

      if (currentMeasurement < start_sample_sent_attempt) //if there are not enough samples yet
      {
        Serial.printf("Collecting new measurement\n");
      }
      else if (currentMeasurement >= start_sample_sent_attempt || (firstMeasurement != 0)) //if there are enough samples to sent
      {
        sent_ESPNOW_message();
      }

      systemState = systemState_initMeasurement;

      esp_deep_sleep_start(); //go to deepsleep
    }
    break;
    }
  }
}

#define debug_sent_ESPNOW_message 1 //low priority debug messages start with this: "[ESPNOW]: "

void sent_ESPNOW_message()
{
  WiFi.mode(WIFI_STA);          //this mode is required for ESP NOW, if you forget this the CPU will panic'd.
  if (esp_now_init() != ESP_OK) //if initing wasn't succesful
  {
    Serial.println("Error initializing ESP-NOW");
    esp_now_deinit();
    WiFi.mode(WIFI_OFF);
    return;
  }
  esp_now_register_send_cb(OnDataSent); //register call back to fetch send reaction

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) //add to peer
  {
    Serial.println("Failed to add peer");
    esp_now_deinit();
    WiFi.mode(WIFI_OFF);
    //esp_now_del_peer(&peerInfo.peer_addr);
    return;
  }
#if debug_sent_ESPNOW_message
  Serial.println("Connected");
#endif
  //everthing look ok until now, allocate memory

  //then copy data in ESP_message(RAM memory)
  if (firstMeasurement == 0)
  {
    for (uint8_t counter0 = 0; counter0 < currentMeasurement; counter0++)
    {
      //printf("measurement[%u].temperature1: %f\n", counter0, measurement[counter0].temperature1);
      prepareMessage.pipeTemps1[counter0] = measurement[counter0].temperature1;
      prepareMessage.pipeTemps2[counter0] = measurement[counter0].temperature2;
    }
    prepareMessage.numberofMeasurements = currentMeasurement;
  }
  else
  {
    uint8_t counter2 = firstMeasurement;
    uint8_t totalSamples = currentMeasurement;
    if (firstMeasurement != 0)
    {
      totalSamples = maximum_samples_memory;
    }
    for (uint8_t counter1 = 0; counter1 < (totalSamples); counter1++)
    {

      //printf("measurement[%u].temperature1: %f\n", counter1, measurement[counter1].temperature1);
      prepareMessage.pipeTemps1[counter1] = measurement[counter2].temperature1;
      prepareMessage.pipeTemps2[counter1] = measurement[counter2].temperature2;
      if (counter2 < maximum_samples_memory)
      {
        counter2++;
      }
      else
      {
        counter2 = 0;
      }
    }
    prepareMessage.numberofMeasurements = totalSamples;
  }

#if debug_sent_ESPNOW_message
  printf("[ESPNOW]: prepareMessage.numberofMeasurements = %u\n", prepareMessage.numberofMeasurements);
  printf("[ESPNOW]: prepareMessage.intervalTime = %u\n", prepareMessage.intervalTime);
  for (uint8_t counter1 = 0; counter1 < prepareMessage.numberofMeasurements; counter1++)
  {
    printf("[ESPNOW]: prepareMessage.pipeTemps1[%u] = %f\n", counter1, prepareMessage.pipeTemps1[counter1]);
    printf("[ESPNOW]: prepareMessage.pipeTemps2[%u] = %f\n", counter1, prepareMessage.pipeTemps2[counter1]);
  }
  printf("[ESPNOW]: This should be the message\n");
#endif

  esp_err_t result = esp_now_send(gateway_mac_address, (uint8_t *)&prepareMessage, sizeof(prepareMessage));

  if (result != ESP_OK)
  {
    Serial.printf("Error sending the data, with return %u\n", result);
    esp_now_deinit();
    WiFi.mode(WIFI_OFF);
    return;
  }
#if debug_sent_ESPNOW_message
  else
  {
    Serial.println("[ESPNOW]: Sent with success");
  }
#endif
}

void loop()
{
}