
/* The codes are licensed under GNU LESSER GENERAL PUBLIC LICENSE
*   Copyright © 2007 Free Software Foundation, Inc. <https://fsf.org/>
*   More on the lincense at <https://www.gnu.org/licenses/lgpl-3.0.en.html>
*   Everyone is permitted to copy and distribute verbatim copies of this license document, but changing is not allowed.
*   
*   Acknowledgement: The project uses a lot of free libraries from different contributors. An special thanks to all hard working software people. 
*                    Specific acknowledgement is made into the seperate files in the include folder.
*    
*   The purpose of this project is to create a decentralised or independent controller for Rouf Aquaculture Ptv. Ltd (Proposed) Bangladesh Intensive Prawn Farming Project. 
*   The field slave devices talks back to ESP32 (Master) over RS485 Modbus.
*   The data is taken in and send to MQTT Broker over Wifi
*   
*   Dedicated to Tyler Goodwin, the mate that influenced me to join the software side;
*   
*/

#include "mqtt.cpp" // Change Tank address here
/* Change Wifi IP Addresses in mqtt_variables.h*/
#include "OTA.h"
#include "DOpH.cpp"
#include "MAX31865.h"

bool heart = 0;
float midTemp = 0.00;
uint8_t pt100Fault = 0;

void setup()
{
  Serial.begin(9600);
  mqttInit(); // Initialising MQTT Parameters, check mqtt.h for more
  otaInit();  // Intialising OTA
  doInit();   // Intialises DO sensor
  pt100Init(); // Initialises MAX31865 sensor
}

void loop()
{
  publish(0, HEART_TOPIC);

  //pt100;
  midTemp = temperature();
  pt100Fault = fault();

  DO(); // Starts DO measurements

  if (averagedomgl != 0.00)
  {
    publish(averagedomgl, DO_TOPIC); // Sends average DOmg/L Data to Broker

  }
  publish(averagedomgl, DO_TOPIC);
  publish(doTemp, TEMPBOT_TOPIC);
  publish(midTemp, TEMPMID_TOPIC);
  publish(doHeart, HEART_TOPIC);

  vTaskDelay(20000 / portTICK_PERIOD_MS); // delay is introduced to reduce bandwidth of the network

  otaLoop();
}