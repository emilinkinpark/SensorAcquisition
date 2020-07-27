/*  Smoothed by Matthew Fryer
*/

#include "modbus.cpp"
#include "conversions.cpp"
#include "mqtt.cpp"

#include <Smoothed.h>

Smoothed<float> AverageDOmgl;

float averagedomgl = 0.00;

// DO sensor ID
#define O2_slaveID 0x0E
#define O2_slaveID_DEC 14

#define pH_slaveID 0x01
#define pH_slaveID_DEC 1

// DO variables
boolean do_heart = 0;
float DOmgl = 0.00;
float DO_Temp = 0.00;

// pH variables
boolean ph_heart = 0;
//float ORP;            //Div by 10
float ph_val = 0.00;         //Div by 100
float ph_temperature = 0.00; // Div by 10
//int resitance;        // Div by 1

void DO()
{
  /* Flow Chart
                Start Measurement -> Delay 2 seconds ->               Request DO and Temp Data
                                                                |                                     ^
                                                                v                                     |
                                                        Gathers 10 DOmg/l samples for averaging -> Delay 1 second
                                                                |
                                                                v
                                                        Get Average DOmg/l data
                                                                |
                                                                v
                                                        Stop Measurement
                                                                |
                                                                v
                                                        Send Data to MQTT Broker
*/
  int o2[13]; //O2 buffer length must have a size of 12 bytes

  byte DOfaultstatus = 0;

  // Start Measurement
  modbusMasterTransmit(3, O2_slaveID, 0x03, 0x25, 0x00, 0x00, 0x01); //Serial2 used for Transceive Data
  if (Serial2.available() > 0)
  {
    serial_flush_buffer(3); //Cleaning Response
    delay(100);

    for (byte count = 0; count <= 10; count++) // Receiving DO data 10 times and averaging
    {
      modbusMasterTransmit(3, O2_slaveID, 0x03, 0x26, 0x00, 0x00, 0x04); // Request Data Block from Sensor

      modbusRead(3, O2_slaveID_DEC, 13, o2); //Acquiring Data and saving into o2
      delay(100);

      //Serial.println("Data Acquired");

      DO_Temp = floatTOdecimal(o2[3], o2[4], o2[5], o2[6]);
      //float Temp_Manipulation = Conv_Temp * 100;
      //Temp_Send = Temp_Manipulation;

      float Conv_DOPerc = floatTOdecimal(o2[7], o2[8], o2[9], o2[10]);
      memset(o2, 0, sizeof(o2)); //Empties array

      DOmgl = domglcalc(DO_Temp, Conv_DOPerc);

      if (isnan(DOmgl) != 0.00) // Checks Error Data Received
      {
        count = count - 1;
        if (Serial2.available() > 0)
        {
          serial_flush_buffer(3); //Cleaning Response
        }
        DOfaultstatus++; // DOfaultstatus increment
        delay(1000);
      }
      else
      {
        AverageDOmgl.add(DOmgl); // Adds Data for averaging
        DOfaultstatus = 0;       // DOfaultstatus reset
      }
      if (DOfaultstatus >= 15) // If Sensor does not respond 15 times then, publish error and break;
      {
        do_heart = 0;                     //Sends out when DO Sensor Fails
        ets_printf("DO Sensor Failed\n"); // Reports error
        break;
      }
      else
      {
        do_heart = 1;
      }
    }

    // Stop Measurement
    modbusMasterTransmit(3, O2_slaveID, 0x03, 0x2E, 0x00, 0x00, 0x01);

    if (Serial2.available() > 0)
    {
      serial_flush_buffer(3); //Cleaning Response
    }
    //delay(100);
    //  Serial.println("Stop Measurement");

    averagedomgl = AverageDOmgl.get(); // Stores the average value

    if (isnan(averagedomgl) != 0.00) // Checks Error Data Received
    {
      do_heart = 0;
    }
    else
    {
      AverageDOmgl.clear(); // Clears Average data
    }
  }
  else
  {
    do_heart = 0;
    Serial.println("DO Sensor Not Detected");
    delay(1000);
  }
}

void pH()
{
  int ph_temp[13]; //pH sensor buffer

  ph_heart = 0;

  modbusMasterTransmit(3, 0x01, 0x03, 0x00, 0x00, 0x00, 0x04); //Requesting ORP, pH, Temperature and Resistance
  if (Serial2.available() > 0)
  {
    serial_flush_buffer(3); //Cleaning Response
    delay(100);

    modbusMasterTransmit(3, 0x01, 0x03, 0x00, 0x00, 0x00, 0x04); //Requesting ORP, pH, Temperature and Resistance
    modbusRead(3, pH_slaveID_DEC, 15, ph_temp);                  //Acquiring Data and saving into ph_temp
    delay(100);

    //Serial.println("Slave ID not matched Transmission Halt!");
    //Serial.println(o2[0], HEX);
    //ORP = hex16_signedint(ph_temp[3], ph_temp[4]) / 10.00;
    ph_val = hex16_signedint(ph_temp[5], ph_temp[6]) / 100.00;
    ph_temperature = hex16_signedint(ph_temp[7], ph_temp[8]) / 10.00;
    memset(ph_temp, 0, sizeof(ph_temp)); //Empties array

    //resitance = hex16_signedint(ph_temp[9], ph_temp[10]);
    ph_heart = 1;
    delay(100);
  }
  else
  {
    ph_heart = 0;
    Serial.println("pH Sensor Not Detected");
  }
}
