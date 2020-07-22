/*
  Library Used: PubSubClient.h by knolleary 
Initial Code Concent from Rui Santos - https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/
*/

#include <PubSubClient.h>
#include "mqtt_variables.h"
#include "wifi_cred.h"

WiFiClient espClient;
PubSubClient client(espClient);

long lastReconnectAttempt = 0;

uint8_t wifi_fault_status = 0.00;
boolean sta_was_connected = false;


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
  client.setServer(MQTT_Broker_IP, 1883);
  client.setCallback(callback); // Required for subsribing to MQTT Topics

  //MQTT TOPIC Dependants
  strcat(HEARTBEAT_TOPIC, tank_addr);
  strcat(HEARTBEAT_TOPIC, "/DATA/HEART"); //TANK_x/DATA/HEART    17 characters
  strcat(DO_TOPIC, tank_addr);
  strcat(DO_TOPIC, "/DATA/LT105A"); //TANK_x/DATA/LT105A  20 characters
  strcat(pH_TOPIC, tank_addr);
  strcat(pH_TOPIC, "/DATA/LT1729D"); //TANK_x/DATA/LT1729D  20 characters
}

void setup_wifi()
{
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA); // WiFi Station mode
  if (!sta_was_connected)
  {
    WiFi.begin(ssid, pass);
    WiFi.mode(WIFI_STA);                // Station Mode Enable
    WiFi.setHostname(tank_addr);        // Sets custom Device Name
    Serial.println(WiFi.getHostname()); // Debugging
    //WiFi.persistent(true);       // Store ssid and pass into SDK flash //deprected
    WiFi.setAutoReconnect(true);         //Enables auto reconnect
    WiFi.setTxPower(WIFI_POWER_19_5dBm); // Sets to max Transmit Power
    Serial.print("Tx Power: "); Serial.println(WiFi.getTxPower());   // Debugging
  }
  else
  {
    WiFi.reconnect();
  }
  for (byte count = 0; (WiFi.status() != WL_CONNECTED) && (count < 50); count++)
  {
    delay(500);
    if (Serial.available())
    {
      Serial.printf("WiFi.status=%d\n", WiFi.status());
      Serial.read();
    }
    else
      Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    sta_was_connected = WiFi.status() == WL_CONNECTED;
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Godspeed!!!");
  }
  else
  {
    Serial.println("");
    Serial.printf("WiFi Connection Failed, status=%d", WiFi.status());
  }
}

boolean reconnect() // MQTT Reconnect Sub routine
{
  if (client.connect(tank_addr))
  {
    Serial.println("MQTT Broker Connected");
    // ... and resubscribe
    //client.subscribe("inTopic");
  }
  else
  {
    Serial.println("MQTT Broker Not Found");
  }
  return client.connected();
}

void wifi_check()
{
  //Serial.print("WiFi Status: "); Serial.println(WiFi.status());
  delay(1000);

  for (byte count = 0; (WiFi.status() != WL_CONNECTED) && (count < 50); count++)
  {
    delay(500);
    if (Serial.available())
    {
      Serial.printf("WiFi.status=%d\n", WiFi.status());
      Serial.read();
    }
    else
      Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.reconnect();
    reconnect();
    /* Serial.print("Auto Reconnect Status: ");
    Serial.println(WiFi.getAutoReconnect()); */
  }
  else
  {
    //DO Nothing
  } 
}

float Subsribe_Sensor_Data(String &SubscribedData) //Converts Subscribed MQTT Data to float value
{
  //Serial.println(messageTemp); // Working
  const char *temp = SubscribedData.c_str();
  /* Serial.println(temp);                 //Debugging */

  if (strstr(temp, "\"DO\":"))
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

void publish(float var, const char *tag, const char *publish_topic)
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

void mqttloop() // This part needs to be in loop
{

  if (!client.connected())
  {
    long now = millis();
    if (now - lastReconnectAttempt > 5000)
    {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect())
      {
        lastReconnectAttempt = 0;
      }
    }
  }
  else
  {
    // Client connected

    client.loop();
  }

  //MQTT End
}
