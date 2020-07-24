
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

hw_timer_t *watchdogTimer = NULL;

void IRAM_ATTR resetModule() //Resets on Watchdog
{
  ets_printf("reboot\n");
  esp_restart();
}

void setup()
{

  Serial.begin(9600); //TXD0 - used as serial decorder

  //Serial1.begin(9600, SERIAL_8N1, UART1_RX, UART1_TX);
  //Caution: Remove Pins before uploading firmware!!!!! // Shared with Flash
  Serial2.begin(9600, SERIAL_8N1, UART2_RX, UART2_TX);

  AverageDOmgl.begin(SMOOTHED_AVERAGE, 9); //Initialising Average class
  
  mqtt_init();  // Initalise MQTT

 /*  watchdogTimer = timerBegin(0, 80, true);                 //timer 0, div 80
  timerAttachInterrupt(watchdogTimer, &resetModule, true); // Does resetModule Function when watchdog hits
  timerAlarmWrite(watchdogTimer, 30000000, false);         // Watchdog Time set in us; Default 30 seconds
  timerAlarmEnable(watchdogTimer);                         //enable interrupt
 */
  //bmeInit(); // Initialising BME680 Dependencies
}

void loop()
{
  //timerWrite(watchdogTimer, 0); //Resets Watchdog Timer

  heartbeat = 1; //Heartbeat = 1 marks the start of loop

  
  publish(heartbeat, "ESP32", HEARTBEAT_TOPIC);

  DO(); //Measuring Dissolved Oxygen

  Serial.println("I'm out of DO");

  pH(); //Measuring pH

  Serial.println("I'm out of pH");

  if (millis() >= 7200000) // Resets the device in 2 hours
  {
    resetModule();
  }
  else
  {
    mqtt_send();          //Sends a bunch of data
  }
}
