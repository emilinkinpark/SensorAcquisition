
#ifndef wifi_cred_h
#define wifi_cred_h

#include <WiFi.h>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#define ssid "GloryCTRL"   //"AsmaulsRedmi" //  "Glory"                //         "GloryOffice"       //
#define pass "Gloryhabib1@1" //"hackyou123"   //      // "razurahat@1!1"                                //

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;


IPAddress local_IP(10, 0, 1, 20);
IPAddress gateway(10, 0, 1, 1);
IPAddress subnet(255, 255, 0, 0);

#endif