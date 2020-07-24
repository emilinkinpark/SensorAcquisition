
#ifndef wifi_cred_h
#define wifi_cred_h

#include <WiFi.h>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#define ssid "GloryAgro"   //"AsmaulsRedmi" //  "Glory"                //         "GloryOffice"       //
#define pass "Gloryart1!1" //"hackyou123"   //      // "razurahat@1!1"                                //

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

//Static IP definitions
/* 
IPAddress local_IP(192, 168, 0, 17);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
 */

#endif