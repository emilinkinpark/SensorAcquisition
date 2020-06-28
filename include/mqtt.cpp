/*
Initial Code Concent from Rui Santos - https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include "variables.h"



WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];


void setup_wifi(char *ssid, char *pass)
{
  delay(10);
  // We start by connecting to a WiFi network
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    //Serial.print(".");
  }

  // Serial.println("");
    Serial.println("WiFi connected"); Serial.println("IP address: "); Serial.println(WiFi.localIP());
}


void callback(char *topic, byte *message, unsigned int length)
{
  //Serial.print("Message arrived on topic: ");
  //Serial.print(topic);
  //Serial.print(". Message: ");
  /* String messageTemp;

  for (int i = 0; i < length; i++)
  {
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  //Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "TANK2/DATA/LT105A")
  {
    float DO = Subsribe_Sensor_Data(messageTemp);
    //Serial.print("Success: "); Serial.println(DO);   //Debugging 
  }

  else
  {
    Serial.println("No message");
  } */
}


void mqtt_init()
{
  setup_wifi(SSID,PASS);
  client.setServer(MQTT_Broker_IP, 1883);
  client.setCallback(callback);
  
  //MQTT TOPIC Dependants
  strcat(HEARTBEAT_TOPIC,tank_addr); strcat(HEARTBEAT_TOPIC,"/DATA/HEART");    //"TANK_1/DATA/HEART"    17 characters
  strcat(DO_TOPIC,tank_addr); strcat(DO_TOPIC,"/DATA/LT105A");                 //"TANK_1/DATA/LT105A"  20 characters
  strcat(pH_TOPIC,tank_addr); strcat(pH_TOPIC,"/DATA/LT1729D");                //"TANK_1/DATA/LT1729D"  20 characters

}



float Subsribe_Sensor_Data(String &SubscribedData) //Converts Subscribed MQTT Data to float value
{
  //Serial.println(messageTemp); // Working
  const char *temp = SubscribedData.c_str();
  /* Serial.println(temp);                 //Debugging */

  if (char *ret = strstr(temp, "\"DO\":"))
  {
    float val = atof(&SubscribedData[6]); //Converts String to float
                                          /*  Serial.print("Float is: ");           // Debugging
    Serial.println(val);                  // Debugging */
    return val;
  }
  else
  {
    // Do Nothing
  }
}


void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    delay(10000);

    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(tank_addr))
    {
      Serial.println("connected");
      // Subscribe
      //client.subscribe("TANK2/DATA/LT105A"); // Subscribes to Topic
    }
    else
    {
      Serial.print("failed, rc="); Serial.print(client.state()); Serial.println(" try again in 5 seconds"); // Wait 5 seconds before retrying
      delay(5000);
      mqtt_init();                                        // Re-initialises WiFi and MQTT
    }
  }  
}


char publish(float var, const char * tag, const char * publish_topic)
{
   char str[50];
   char temp[8];

    dtostrf(var, 1, 2, temp);
    strcpy(str, "{");
    //strcat(str, "\"HEARTBEAT\"");
    strcat(str, "\"");
    strcat(str, tag);
    strcat(str, "\"");
    strcat(str, "\:");
    strcat(str, temp);
    strcat(str, "}");
    client.publish(publish_topic, str);

    memset(str, 0, sizeof(str)); //Empties array
}

void mqttloop()           // This part needs to be in loop
{
  boolean heartbeat = 0; // Heartbeat of the device

  


  long now = millis();   //MQTT dependant

  if (!client.connected())
  { //Reconnect if network fails
    reconnect();
  }
  client.loop();

  if (now - lastMsg > 5000)
  {
    lastMsg = now;
    heartbeat = 1;  //Heartbeat publishes 1 to mark start of transmission

    publish(heartbeat,"ESP32",HEARTBEAT_TOPIC);
    publish(do_heart,"DO",HEARTBEAT_TOPIC);
    publish(ph_heart,"pH",pH_TOPIC);

    publish(DOmgl,"DO",DO_TOPIC);
    publish(DO_Temp,"Temperature",DO_TOPIC);

    publish(ORP,"ORP",pH_TOPIC);
    publish(ph_val,"pH",pH_TOPIC);
    publish(ph_temperature,"Temperature",pH_TOPIC);
    publish(resitance,"Resistance",pH_TOPIC);
    


    heartbeat = 0;  //Heartbeat publishes 0 to mark end of transmission

    publish(heartbeat,"HEART",HEARTBEAT_TOPIC);


  }
  //MQTT End
}