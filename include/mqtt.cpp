/*
  MQTT Client Powered by AsyncMqttClient
*/

#include <WiFi.h>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include "mqtt_variables.h"

//Topic Declaration
char tankAddr[11] = "MQTT/TANK1"; //Insert TANK Address
char HEART_TOPIC[20];
char DO_TOPIC[20];
char TEMPMID_TOPIC[20];
char TEMPBOT_TOPIC[20];

//Class Declaration
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

void mqtt_topic_declaration() // Stores Topics into variables
{
  strcat(DO_TOPIC, tankAddr);
  strcat(DO_TOPIC, "/DO"); //TANK_x/DATA/DO  20 characters
  strcat(HEART_TOPIC, tankAddr);
  strcat(HEART_TOPIC, "/heart"); //TANK_x/DATA/heart  20 characters
  strcat(TEMPMID_TOPIC, tankAddr);
  strcat(TEMPMID_TOPIC, "/tempMid"); //TANK_x/DATA/tempMid  20 characters
  strcat(TEMPBOT_TOPIC, tankAddr);
  strcat(TEMPBOT_TOPIC, "/tempBot"); //TANK_x/DATA/tempBot  20 characters

  // Deprecated from this 2021 version
  // strcat(pH_TOPIC, tank_addr);
  // strcat(pH_TOPIC, "/DATA/LT1729D"); //TANK_x/DATA/LT1729D  20 characters
}

void connectToWifi()
{
  Serial.println("Connecting to Wi-Fi...");
  WiFi.config(local_IP, gateway, subnet, INADDR_NONE, INADDR_NONE);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt()
{
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event)
{
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event)
  {
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    connectToMqtt();
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi lost connection");
    xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    xTimerStart(wifiReconnectTimer, 0);
    break;
  }
}

void onMqttConnect(bool sessionPresent) /// Check for mqtt publish and subscribe examples
{
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  //uint16_t packetIdSub = mqttClient.subscribe("test/lol", 2);
  //Serial.print("Subscribing at QoS 2, packetId: ");
  //Serial.println(packetIdSub);
  //mqttClient.publish("test/lol", 0, true, "test 1");
  //Serial.println("Publishing at QoS 0");
  //uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
  // Serial.print("Publishing at QoS 1, packetId: ");
  // Serial.println(packetIdPub1);
  // uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
  // Serial.print("Publishing at QoS 2, packetId: ");
  // Serial.println(packetIdPub2);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected())
  {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
}

void onMqttPublish(uint16_t packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

/*void mqttKepwarepublish(float var, const char *tag, const char *publish_topic) //Deprecated from 2021 versions
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
  mqttClient.publish(publish_topic, 1, false, buf);
}
*/

void publish(float var, const char *publish_topic) // Inputs the variable and sends to the specified topics
{
  char buf[100];
  dtostrf(var, 1, 2, buf);
  mqttClient.publish(publish_topic, 0, false, buf);
}

void mqttInit()
{
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqtt_topic_declaration();
  connectToWifi();
}
