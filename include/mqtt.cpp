/*
  Library Used: PubSubClient.h by knolleary 
Initial Code Concent from Rui Santos - https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/
*/

#include <PubSubClient.h>
#include "mqtt_variables.h"
#include "esp_system.h"
#include "wifi_cred.h"

WiFiClient espClient;
PubSubClient client(espClient);

long lastReconnectAttempt = 0;

uint8_t wifi_fault_status = 0.00;

uint8_t sta_was_connected = false;

void wifi_reconnect()
{
  wifi_fault_status = wifi_fault_status + 1;
  EEPROM.write(0, wifi_fault_status);
  EEPROM.commit();
  delay(10000);
  WiFi.disconnect();
  WiFi.reconnect();
}

void setup_wifi()
{
  // WiFi.disconnect();
  uint16_t count = 0;
  delay(10);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.mode(WIFI_STA); // WiFi Station mode
  if (!sta_was_connected)
    WiFi.begin(SSID, PASS); //sta_was_connected is global, init'd to false.
  else
    WiFi.reconnect();
  while ((WiFi.status() != WL_CONNECTED) && (count < 50))
  {
    delay(500);
    count++;
    // if (Serial.available())
    // {
    //   Serial.printf("WiFi.status=%d\n", WiFi.status());
    //   Serial.read();
    // }
    // else
    //   Serial.print(".");
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

  // switch (WiFi.status())
  // {
  // case 1: //NO_SSID_AVAILABLE
  //   wifi_reconnect();
  //   break;
  //   case 4: //WL_CONNECT_FAILED
  //   wifi_reconnect();
  //   break;
  // case 5: //WL_CONNECTION_LOST
  //   wifi_reconnect();
  //   break;
  // case 6: //WL_DISCONNECTED
  //   wifi_reconnect();
  //   break;
  // default:
  //   // Not Required
  //   break;
  // }
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

  client.setServer(MQTT_Broker_IP, 1883);
  client.setCallback(callback);       // Required for subsribing to MQTT Topics

  //MQTT TOPIC Dependants
  strcat(HEARTBEAT_TOPIC, tank_addr);
  strcat(HEARTBEAT_TOPIC, "/DATA/HEART"); //TANK_x/DATA/HEART    17 characters
  strcat(DO_TOPIC, tank_addr);
  strcat(DO_TOPIC, "/DATA/LT105A"); //TANK_x/DATA/LT105A  20 characters
  strcat(pH_TOPIC, tank_addr);
  strcat(pH_TOPIC, "/DATA/LT1729D"); //TANK_x/DATA/LT1729D  20 characters
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

boolean reconnect()
{
  if (client.connect(tank_addr))
  {
    Serial.println("MQTT Broker Connected");
    // ... and resubscribe
    //client.subscribe("inTopic");
  }
  return client.connected();
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
