
/* The codes are licensed under GNU LESSER GENERAL PUBLIC LICENSE
*   Copyright © 2007 Free Software Foundation, Inc. <https://fsf.org/>
*   More on the lincense at <https://www.gnu.org/licenses/lgpl-3.0.en.html>
*   Everyone is permitted to copy and distribute verbatim copies of this license document, but changing is not allowed.
*   
*   Acknowledgement: The project uses a lot of free libraries from different contributors. An special thanks to all hard working software people. 
*                    Specific acknowledgement is made into the seperate files in the include folder.
*    
*   The purpose of this project is to create a decentralised or independent controller for Habib Aquaculture Ptv. Ltd (Proposed) Bangladesh Intensive Prawn Farming Project. 
*   The field slave devices talks back to ESP32 (Master) over RS485 Modbus.
*   The data is taken in and send to MQTT Broker over Wifi
*   
*   Dedicated to Tyler Goodwin, the mate that influenced me to join the software side;
*   
*/

#include <esp_system.h>
#include "DOpH.cpp"
#include "OTA.h"

//Serial Pins Definition
#define UART1_RX 4
#define UART1_TX 2
#define UART2_RX 16
#define UART2_TX 17

#define ESP32_RTOS // OTA dependency

unsigned long entry; // OTA dependency

byte heartbeat = 0;

//hw_timer_t *watchdogTimer = NULL;

void keepWiFiAlive(void *pvParameters)
{
  /* 
  UBaseType_t uxHighWaterMark;
  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL); */

  for (;;)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      continue;
    }

    Serial.println("[WIFI] Connecting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    WiFi.setHostname(tank_addr);
    WiFi.setTxPower(WIFI_POWER_19_5dBm);

    unsigned long startAttemptTime = millis();

    // Keep looping while we're not connected and haven't reached the timeout
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000)
    {
    }

    // When we couldn't make a WiFi connection (or the timeout expired)
    // sleep for a while and then retry.
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("[WIFI] FAILED");
      vTaskDelay(30000 / portTICK_PERIOD_MS);
      continue;
    }

    Serial.println("[WIFI] Connected: " + WiFi.localIP());
    /* Serial.print("keepWiFiAlive Used Space: ");
    Serial.println(uxHighWaterMark); */
  }
}

void ota_handle(void *pvParameters)
{
  setupOTA(tank_addr);

  for (;;)
  {
    entry = micros();
    ArduinoOTA.handle();

    TelnetStream.println(micros() - entry);
  }
}

void mqttPublish(void *pvParameters)
{
  Serial2.begin(9600, SERIAL_8N1, UART2_RX, UART2_TX);

  AverageDOmgl.begin(SMOOTHED_AVERAGE, 9); //Initialising Average class

  mqtt_init(); // Initalise MQTT

  //pH Calibration
  /* modbusMasterTransmit(3,0x01,0x06,0x00,0x05,0x03,0x32);
  if (Serial2.available() > 0)
    {
      serial_flush_buffer(3); //Cleaning Response
    } */

  for (;;)
  {

    heartbeat = 1; //Heartbeat = 1 marks the start of loop

    publish(heartbeat, "ESP32", HEARTBEAT_TOPIC);

    DO();
    //DO
    publish(do_heart, "DO", HEARTBEAT_TOPIC);

    if (averagedomgl != 0.00)
    {
      publish(averagedomgl, "DO", DO_TOPIC); // Sends average DOmg/L Data to Broker
    }

    publish(DO_Temp, "Temperature", DO_TOPIC); // Sends DO_Temp Data to Broker

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    pH(); //Measuring pH
    //pH
    publish(ph_heart, "pH", HEARTBEAT_TOPIC);
    publish(ph_val, "pH", pH_TOPIC);
    publish(ph_temperature, "Temperature", pH_TOPIC);

    float wifi_strength = WiFi.RSSI();
    publish(wifi_strength, "WiFi_RSSI", HEARTBEAT_TOPIC); // Sends out WiFi AP Signal Strength
    heartbeat = 0;                                        //Heartbeat publishes 0 to mark end of transmission
    publish(heartbeat, "ESP32", HEARTBEAT_TOPIC);

    vTaskDelay(10000 / portTICK_PERIOD_MS); //Waits 8 seconds
    /* Serial.print("mqttPublish Used Space: ");
    Serial.println(uxHighWaterMark); */
  }
}

void setup()
{

  Serial.begin(9600); //TXD0 - used as serial decorder

  xTaskCreatePinnedToCore(
      ota_handle,   /* Task function. */
      "OTA_HANDLE", /* String with name of task. */
      10000,        /* Stack size in bytes. */
      NULL,         /* Parameter passed as input of the task */
      1,            /* Priority of the task. */
      NULL,
      1); /* Task handle. */

  xTaskCreatePinnedToCore(
      keepWiFiAlive,
      "keepWiFiAlive", // Task name
      2000,            // Stack size (bytes)
      NULL,            // Parameter
      2,               // Task priority
      NULL,            // Task handle
      1                // Run on Core 1
  );

  xTaskCreatePinnedToCore(
      mqttPublish,
      "mqttPublish", // Task name
      1000,          // Stack size (bytes)
      NULL,          // Parameter
      3,             // Task priority
      NULL,          // Task handle
      1              // Run on Core 1
  );

  //Serial1.begin(9600, SERIAL_8N1, UART1_RX, UART1_TX);
  //Caution: Remove Pins before uploading firmware!!!!! // Shared with Flash
}

void loop()
{
  //KISS - Keep it Super Simple
}
