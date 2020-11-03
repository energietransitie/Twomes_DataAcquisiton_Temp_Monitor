/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-many-to-one-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  //byte id; // must be unique for each sender board
  int x;
  int y;
} struct_message;

//Create a struct_message called myData
struct_message myData;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

#define timeMeasurePin 2

void setup()
{
  pinMode(timeMeasurePin, OUTPUT);
  boolean state = true;
  for (byte testSync = 0; testSync < 254; testSync++)
  {
    digitalWrite(timeMeasurePin, state);
    state = !state;
    delay(100);
  }

  // Init Serial Monitor
  Serial.begin(115200);
  Serial.println("active");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.getTxPower();
  WiFi.setTxPower(1);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  //esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  Serial.print("sizeof(myData) = ");
  Serial.println(sizeof(myData));
}

void loop()
{
// Set values to send
#define maximumValueSendTest 32000
#define minimumValueSendTest 1
  //myData.id = 1;
  if (myData.x < maximumValueSendTest)
  {
    myData.x++;
  }
  else
  {
    myData.x = minimumValueSendTest;
  }
  if (myData.y > minimumValueSendTest)
  {
    myData.y--;
  }
  else
  {
    myData.y = maximumValueSendTest;
  }

  // Send message via ESP-NOW
  // long startTime = millis();
  digitalWrite(timeMeasurePin, HIGH);
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
  digitalWrite(timeMeasurePin, LOW);
  // long endTime = millis();

  // Serial.print("Time used for send operation: ");
  //Serial.println(endTime - startTime);

  if (result == ESP_OK)
  {
    // Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }
  delay(100);
}