#include <Arduino.h>
#include <WiFi.h>
#include <identification.h>
#include <HTTPClient.h>
#include <esp_now.h>

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //REVIEW, this is a broadcast address

typedef struct struct_message
{
  int temperature1;
  int temperature2;
} struct_message;

struct_message myData;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup()
{

  Serial.begin(115200);
  Serial.println("Device has started");

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 6;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Connected");
}

void loop()
{
// Set values to send
#define test_maximumValueSendTest 32000
#define test_minimumValueSendTest 1
  //myData.id = 1;
  //Serial.println("start");
  if (myData.temperature1 < test_maximumValueSendTest)
  {
    myData.temperature1++;
  }
  else
  {
    myData.temperature1 = test_minimumValueSendTest;
  }
  if (myData.temperature2 > test_minimumValueSendTest)
  {
    myData.temperature2--;
  }
  else
  {
    myData.temperature2 = test_maximumValueSendTest;
  }
  
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }
  delay(500);
}