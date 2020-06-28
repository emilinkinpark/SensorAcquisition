//Insert all sensor header here.
#include "DOpH.cpp"
//#include "bme680.cpp"



//Topic Declaration
char tank_addr[8] = "TANK_3";           //Insert TANK Address

char HEARTBEAT_TOPIC[18] ;
char DO_TOPIC[20];
char pH_TOPIC[20];

//WIFI
#define SSID "Glory"                //"GloryAgro"
#define PASS "razurahat@1!1"        //"Gloryart1!1"

//MQTT
#define Client_Name "TANK1"
#define MQTT_Broker_IP "192.168.0.19"//"192.168.0.29"
#define MQTT_Fallback_IP "0.0.0.0" //Implementation Required




