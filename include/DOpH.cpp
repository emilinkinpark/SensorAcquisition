#include "modbus.cpp"
#include "conversions.cpp"
#include "mqtt.cpp"

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
float ph_val;         //Div by 100
float ph_temperature; // Div by 10
//int resitance;        // Div by 1

void DO()
{
  int o2[13]; //O2 buffer length must have a size of 12 bytes
  //int incomingData[7];
  //Modbus Master Start
  memset(o2, 0, sizeof(o2)); //Empties array
  //memset(incomingData, 0, sizeof(incomingData)); //Empties array

  // Start Measurement
  modbusMasterTransmit(3, O2_slaveID, 0x03, 0x25, 0x00, 0x00, 0x01); //Serial2 used for Transceive Data
  serial_flush_buffer(3);                                            //Cleaning Response
  delay(2000);
  //Serial.println("Starting Measurement");

  modbusMasterTransmit(3, O2_slaveID, 0x03, 0x26, 0x00, 0x00, 0x04);
 
 for (int i = 0; i <= 5; i++)
  {
    modbusRead(3, O2_slaveID_DEC, 13, o2); //Acquiring Data and saving into o2
    delay(100);
  }


  //Serial.println("Data Acquired");

  if (o2[0] != O2_slaveID_DEC) //Slave ID Check
  {
    // Serial.println("Slave ID not matched Transmission Halt!");
    Serial.println(o2[0], HEX);
  }
  else
  {
    DO_Temp = floatTOdecimal(o2[3], o2[4], o2[5], o2[6]);
    //float Temp_Manipulation = Conv_Temp * 100;
    //Temp_Send = Temp_Manipulation;

    float Conv_DOPerc = floatTOdecimal(o2[7], o2[8], o2[9], o2[10]);
    DOmgl = domglcalc(DO_Temp, Conv_DOPerc);

    do_heart = 1;
  }
  if (isnan(DOmgl) != 0) // Check if DO Sensor Fails
  {
    do_heart = 0; //Sends out when DO Sensor Fails
    
    Serial.println("DO Sensor Failed");
  }

  // Stop Measurement
  modbusMasterTransmit(3, O2_slaveID, 0x03, 0x2E, 0x00, 0x00, 0x01);
  serial_flush_buffer(3); //Cleaning Response
  
  delay(500);
  publish(do_heart,"DO",HEARTBEAT_TOPIC);
  publish(DOmgl,"DO",DO_TOPIC);             // Sends DOmg/L Data to Broker
  publish(DO_Temp,"Temperature",DO_TOPIC);  // Sends DOmg/L Data to Broker
  
  //delay(100);
  //  Serial.println("Stop Measurement");
}

void pH()
{
  int ph_temp[13];                     //pH sensor buffer
  memset(ph_temp, 0, sizeof(ph_temp)); //Empties array

  ph_heart = 0;

  modbusMasterTransmit(3, 0x01, 0x03, 0x00, 0x00, 0x00, 0x04); //Requesting ORP, pH, Temperature and Resistance

  for (byte i = 1; i <= 5; i++)
  {
    modbusRead(3, pH_slaveID_DEC, 15, ph_temp); //Acquiring Data and saving into ph_temp
    delay(100);
  }
  if (ph_temp[0] != pH_slaveID_DEC) //Slave ID Check
  {
    //Serial.println("Slave ID not matched Transmission Halt!");
    //Serial.println(o2[0], HEX);
    ph_heart = 0;
    memset(ph_temp, 0, sizeof(ph_temp)); //Empties array
  }
  else
  {
    //ORP = hex16_signedint(ph_temp[3], ph_temp[4]) / 10.00;
    ph_val = hex16_signedint(ph_temp[5], ph_temp[6]) / 100.00;
    ph_temperature = hex16_signedint(ph_temp[7], ph_temp[8]) / 10.00;
    //resitance = hex16_signedint(ph_temp[9], ph_temp[10]);
    ph_heart = 1;
  }
    serial_flush_buffer(3); //Cleaning Response
    
    delay(500);
    publish(ph_heart,"pH",HEARTBEAT_TOPIC);
    publish(ph_val,"pH",pH_TOPIC);
    publish(ph_temperature,"Temperature",pH_TOPIC);
}