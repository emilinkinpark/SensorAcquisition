
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
*   The data is taken in and send to Raspberrypi MQTT server over Wifi
*   
*/

#include "mqtt.cpp"

//Serial Pins Definition

#define UART1_RX 4
#define UART1_TX 2
#define UART2_RX 16
#define UART2_TX 17

long Starttime = 0;
long elapsedtime = 0;

/* void Core0code(void *pvParameters) //Add Dual Core Capabilities; Dont use while using BLE or WIFI
{
  for (;;)
  {

    // bmeRun();          // Running bme680 codes
    //   //TCS3200 Code Start
    //   // Setting RED (R) filtered photodiodes to be read
    //   digitalWrite(S2,LOW);
    //   digitalWrite(S3,LOW);

    //   // Reading the output frequency
    //   redFrequency = pulseIn(sensorOut, LOW);

    //    // Printing the RED (R) value
    //   Serial.print("R = ");
    //   Serial.print(redFrequency);
    //   delay(100);

    //   // Setting GREEN (G) filtered photodiodes to be read
    //   digitalWrite(S2,HIGH);
    //   digitalWrite(S3,HIGH);

    //   // Reading the output frequency
    //   greenFrequency = pulseIn(sensorOut, LOW);

    //   // Printing the GREEN (G) value
    //   Serial.print(" G = ");
    //   Serial.print(greenFrequency);
    //   delay(100);

    //   // Setting BLUE (B) filtered photodiodes to be read
    //   digitalWrite(S2,LOW);
    //   digitalWrite(S3,HIGH);

    //   // Reading the output frequency
    //   blueFrequency = pulseIn(sensorOut, LOW);

    //   // Printing the BLUE (B) value
    //   Serial.print(" B = ");
    //   Serial.println(blueFrequency);
    //   delay(100);
    // // TCS3200 code end
  }
} */

void setup()
{

  //xTaskCreatePinnedToCore(Core0code, "Task0", 10000, NULL, 1, NULL, 0);   // Handle to access core 0, ill advised to use while using WiFi or BLE

  Serial.begin(9600); //TXD0 - used as serial decorder
  /* Serial.println("Setup Time");
  Serial.println(millis()); */
  //Serial1.begin(9600, SERIAL_8N1, UART1_RX, UART1_TX);
  //Caution: Remove Pins before uploading firmware!!!!! // Shared with Flash

  Serial2.begin(9600, SERIAL_8N1, UART2_RX, UART2_TX);

  mqtt_init(); //Initialising MQTT Dependencies Runs on Core 0;
  
  //bmeInit(); // Initialising BME680 Dependencies
 
}

void loop() // All Modbus Operation
{
  Serial.println("Loop Starting");
  Serial.println(millis());
  mqttloop(); //MQTT Start

  //bmeRun(); //BME680 reading
  DO(); //Measuring Dissolved Oxygen

  pH(); //Measuring pH

  //delay(100);
  /* Serial.println("End of Loop");
  Serial.println(millis()); */
}
