/*
  MQTT Client Powered by Eclipse Paho
*/

#include <WIfi.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>

#include "mqtt_variables.h"
#include "wifi_cred.h"

WiFiClient espClient;

uint8_t wifi_fault_status = 0.00;
boolean sta_was_connected = false;

//WiFi and MQTT declarations
WiFiClient wifi;
IPStack ipstack(wifi);
MQTT::Client<IPStack, Countdown, 50, 1> client = MQTT::Client<IPStack, Countdown, 50, 1>(ipstack);
MQTT::Message message;

void mqtt_topic_declaration()
{
  strcat(HEARTBEAT_TOPIC, tank_addr);
  strcat(HEARTBEAT_TOPIC, "/DATA/HEART"); //TANK_x/DATA/HEART    17 characters
  strcat(DO_TOPIC, tank_addr);
  strcat(DO_TOPIC, "/DATA/LT105A"); //TANK_x/DATA/LT105A  20 characters
  strcat(pH_TOPIC, tank_addr);
  strcat(pH_TOPIC, "/DATA/LT1729D"); //TANK_x/DATA/LT1729D  20 characters
}

void mqtt_init()
{
  char hostname[] = MQTT_Broker_IP;
  int port = 1883;

  Serial.print("Connecting to ");
  Serial.print(hostname);
  Serial.print(":");
  Serial.println(port);

  int rc = ipstack.connect(hostname, port);
  if (rc != 1)
  {
    Serial.print("rc from TCP connect is ");
    Serial.println(rc);
  }

  Serial.println("MQTT connecting");
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.MQTTVersion = 4;
  data.clientID.cstring = (char *)tank_addr;
  rc = client.connect(data);
  if (rc != 0)
  {
    Serial.print("rc from MQTT connect is ");
    Serial.println(rc);
  }
  Serial.println("MQTT connected");
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
    Serial.print("Tx Power: ");
    Serial.println(WiFi.getTxPower()); // Debugging
  }
  else
  {
    WiFi.reconnect();
  }
  for (byte count = 0; (!WiFi.isConnected()) && (count < 30); count++) //(byte count = 0; (WiFi.status() != WL_CONNECTED) && (count < 50); count++)
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
  if (WiFi.isConnected()) //(WiFi.status() == WL_CONNECTED)
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

void wifi_check()
{
  //Serial.print("WiFi Status: "); Serial.println(WiFi.status());
  delay(1000);

  for (byte count = 0; (!WiFi.isConnected()) && (count < 30); count++) //(byte count = 0; (WiFi.status() != WL_CONNECTED) && (count < 50); count++)
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

  if (!WiFi.isConnected()) //(WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    WiFi.reconnect();
  }
  else
  {
    //DO Nothing
  }
}

void publish(float var, const char *tag, const char *publish_topic) // Easy Routine to send data
{
  char temp[8];

  char buf[100];
  dtostrf(var, 1, 2, temp);
  strcpy(buf, "{");
  strcat(buf, "\"");
  strcat(buf, tag);
  strcat(buf, "\"");
  strcat(buf, "\:");
  strcat(buf, temp);
  strcat(buf, "}");
  message.qos = MQTT::QOS1;
  message.retained = false;
  message.dup = false;
  message.payload = (void *)buf;
  message.payloadlen = strlen(buf);
  client.publish(publish_topic, message);
}

void mqtt_send() // This part needs to be in loop
{

  if (!client.isConnected())
  {
    mqtt_init();
  }
  else
  {
    float wifi_strength = WiFi.RSSI();
    publish(wifi_strength, "WiFi_RSSI", HEARTBEAT_TOPIC); // Sends out WiFi AP Signal Strength
    byte heartbeat = 0; //Heartbeat publishes 0 to mark end of transmission
    publish(heartbeat, "ESP32", HEARTBEAT_TOPIC);
    delay(8000); //Waits 8 seconds
  }

}
