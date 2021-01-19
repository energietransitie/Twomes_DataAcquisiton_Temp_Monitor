#include <Arduino.h>
#include <OneWire.h>           //for DS18B20 support
#include <DallasTemperature.h> //for DS18B20 support
#include <WiFi.h>
#include <esp_now.h>

#define hardwareFunction 1 //0: roomTemp, 1: boilerTemp

#define hardwarePin_sensor1 25 //DS18B20 temperature sensor 1/pipeTemp1
#define hardwarePin_sensor2 26 //DS18B20 temperature sensor 2/pipeTemp2

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 4           /* Time ESP32 will go to sleep (in seconds) */
#define TIME_TO_CONVERSION 1      /* Time ESP32 will go to sleep for conversion(in seconds) */
#define INTERVAL_US 5000000       /* desired interval between conversions */

#define maximum_samples_memory 50
#define start_sample_sent_attempt 10

//systemStates
#define systemState_unknown 0
#define systemState_initMeasurement 1
#define systemState_readMeasurement 2

RTC_DATA_ATTR uint8_t currentMeasurement = 0;
RTC_DATA_ATTR uint8_t systemState = 0;
RTC_DATA_ATTR uint64_t previous_time = 0;
RTC_DATA_ATTR uint64_t time_correction = 400000; //initial time correction

//prototype functions
boolean sent_ESPNOW_message();

uint8_t destinationAddress[] = {0xAA, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6}; //REVIEW, this is a broadcast address

OneWire data_Sensor1(hardwarePin_sensor1);
OneWire data_Sensor2(hardwarePin_sensor2);

DallasTemperature temperature_sensor1(&data_Sensor1);
DallasTemperature temperature_sensor2(&data_Sensor2);

typedef struct measurementFormat
{
  float temperature1;
#if hardwareFunction
  float temperature2;
#endif
} measurementFormat;

measurementFormat measurement[maximum_samples_memory];

typedef struct ESP_message
{
  uint8_t numberofMeasurements = currentMeasurement;
  uint8_t intervalTime = 5; //(INTERVAL_US / 1000000); //intervalTime in milliSeconds
  float pipeTemps1[maximum_samples_memory];
  float pipeTemps2[maximum_samples_memory];
} ESP_message;

ESP_message prepareMessage;

boolean ESP_return_state = false;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  if (status == ESP_NOW_SEND_SUCCESS)
  {
    Serial.printf("Delivery Success\n");
    ESP_return_state = true;
  }
  else
  {
    Serial.printf("Delivery Fail\n");
  }
}

#define debugMessage 1

void setup()
{
  setCpuFrequencyMhz(80); // to set the cpu frequency to 80Mhz to be more energy efficient

  while (1)
  {
    switch (systemState)
    {
    case systemState_unknown:
      Serial.begin(115200);
      Serial.println("Device has started");
      Serial.println("booted for the first time");
      temperature_sensor1.begin();
      temperature_sensor1.requestTemperatures();
      Serial.printf("Temperature got: %f\n", temperature_sensor1.getTempCByIndex(0));

      if (temperature_sensor1.getDeviceCount())
      {
        Serial.printf("Found temperature_sensor1\n");
        systemState = systemState_initMeasurement;
      }
      else
      {
        Serial.printf("Did not find temperature_sensor1\n");
        delay(1000);
      }
      Serial.end();
      break;
    case systemState_initMeasurement:
      temperature_sensor1.begin();                     //initialize sensor
      temperature_sensor1.setWaitForConversion(false); //disable the wait for conversion
      temperature_sensor1.requestTemperatures();       //start conversion

      esp_sleep_enable_timer_wakeup(TIME_TO_CONVERSION * uS_TO_S_FACTOR); //sleep for 1 second during conversion

      //Serial.println(wake_after_conversion);
      //Serial.println("Going to sleep now");
      //Serial.flush();
      systemState = systemState_readMeasurement; //set conversion is started bit

      esp_deep_sleep_start(); //go to deepsleep
      break;
    case systemState_readMeasurement:
    {
#if debugMessages
      Serial.begin(115200);
      Serial.println("Device has started");
#endif
      temperature_sensor1.begin(); //initialize again to find the device adress of the sensor

      if ((temperature_sensor1.getTempCByIndex(0)) == -127)
      {
#if debugMessages
        Serial.println("\n\n\n\n\n\n\n\n\nNow everything should be deleted, and starting again");
#endif
        delay(10000);
        systemState = systemState_initMeasurement;
        break; // init everything again
      }

      measurement[currentMeasurement].temperature1 = temperature_sensor1.getTempCByIndex(0); // get the temperature of the sensor
      currentMeasurement++;

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
      if (currentMeasurement >= maximum_samples_memory)
      {
#if debugMessages
        Serial.println("Maximum memory space reached");
#endif
        delay(6000);
      }
      if ((currentMeasurement >= start_sample_sent_attempt) && (sent_ESPNOW_message()))
      {
#if debugMessages
        Serial.printf("%u measurements sended\n", currentMeasurement);
#endif
        currentMeasurement = 0;
      }
      else if ((currentMeasurement >= start_sample_sent_attempt))
      {
#if debugMessages
        Serial.printf("\n\n\n\n\n\n\ncurrentMeasurement = %u\n", currentMeasurement);
#endif
      }

      systemState = systemState_initMeasurement; //reset conversion is started bit
      Serial.end();
      esp_deep_sleep_start(); //go to deepsleep
    }
    break;
    }
  }
}

boolean sent_ESPNOW_message()
{
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return false;
  }

  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, destinationAddress, 6);
  peerInfo.channel = 6;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return false;
  }

  Serial.println("Connected");

  esp_err_t result = esp_now_send(destinationAddress, (uint8_t *)&prepareMessage, sizeof(prepareMessage));

  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }
  while (ESP_return_state)
  {
    Serial.println("waiting for positive return");
    delay(2000);
  }
  return false;
}

void loop()
{
}