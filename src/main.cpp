
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
*/

#include <esp_system.h>
#include "DOpH.cpp"

//Serial Pins Definition
#define UART1_RX 4
#define UART1_TX 2
#define UART2_RX 16
#define UART2_TX 17

byte heartbeat = 0;

//hw_timer_t *watchdogTimer = NULL;

void keepWiFiAlive(void *pvParameters)
{
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

    unsigned long startAttemptTime = millis();

    // Keep looping while we're not connected and haven't reached the timeout
    while (WiFi.status() != WL_CONNECTED &&
           millis() - startAttemptTime < 10000)
    {
    }

    // When we couldn't make a WiFi connection (or the timeout expired)
    // sleep for a while and then retry.
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("[WIFI] FAILED");
      vTaskDelay(20000 / portTICK_PERIOD_MS);
      continue;
    }

    Serial.println("[WIFI] Connected: " + WiFi.localIP());
  }
}

void mqttpublish(void *pvParameters)
{
  for (;;)
  {
    heartbeat = 1; //Heartbeat = 1 marks the start of loop

    publish(heartbeat, "ESP32", HEARTBEAT_TOPIC);

    DO();
    //DO
    publish(do_heart, "DO", HEARTBEAT_TOPIC);
    publish(averagedomgl, "DO", DO_TOPIC);     // Sends average DOmg/L Data to Broker
    publish(DO_Temp, "Temperature", DO_TOPIC); // Sends DO_Temp Data to Broker

    pH(); //Measuring pH
    //pH
    publish(ph_heart, "pH", HEARTBEAT_TOPIC);
    publish(ph_val, "pH", pH_TOPIC);
    publish(ph_temperature, "Temperature", pH_TOPIC);

    float wifi_strength = WiFi.RSSI();
    publish(wifi_strength, "WiFi_RSSI", HEARTBEAT_TOPIC); // Sends out WiFi AP Signal Strength
    heartbeat = 0;                                        //Heartbeat publishes 0 to mark end of transmission
    publish(heartbeat, "ESP32", HEARTBEAT_TOPIC);

    delay(8000); //Waits 8 seconds
  }
}

void setup()
{

  Serial.begin(9600); //TXD0 - used as serial decorder

  //Serial1.begin(9600, SERIAL_8N1, UART1_RX, UART1_TX);
  //Caution: Remove Pins before uploading firmware!!!!! // Shared with Flash
  Serial2.begin(9600, SERIAL_8N1, UART2_RX, UART2_TX);

  mqtt_init(); // Initalise MQTT

  AverageDOmgl.begin(SMOOTHED_AVERAGE, 9); //Initialising Average class

  /* watchdogTimer = timerBegin(0, 80, true);                 //timer 0, div 80
  timerAttachInterrupt(watchdogTimer, &resetModule, true); // Does resetModule Function when watchdog hits
  timerAlarmWrite(watchdogTimer, 30000000, false);         // Watchdog Time set in us; Default 30 seconds
  timerAlarmEnable(watchdogTimer);                         //enable interrupt
 */
  //bmeInit(); // Initialising BME680 Dependencies

  xTaskCreatePinnedToCore(
      keepWiFiAlive,
      "keepWiFiAlive", // Task name
      5000,            // Stack size (bytes)
      NULL,            // Parameter
      2,               // Task priority
      NULL,            // Task handle
      1                // Run on Core 1
  );

  xTaskCreatePinnedToCore(
      mqttpublish,
      "mqttpublish", // Task name
      5000,          // Stack size (bytes)
      NULL,          // Parameter
      1,             // Task priority
      NULL,          // Task handle
      1              // Run on Core 1
  );
}

void loop()
{
  /* if (millis() >= 60000) //1800000) // Resets the device in 30 minutes
  {
    resetModule();
  } */
}
