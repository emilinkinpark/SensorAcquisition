//Insert all sensor header here.
#include "DOpH.cpp"
//#include "bme680.cpp"



//Topic Declaration
char tank_addr[8] = "TANK_1";           //Insert TANK Address

char HEARTBEAT_TOPIC[18] ;
char DO_TOPIC[20];
char pH_TOPIC[20];

//WIFI
#define SSID "GloryAgro"                //"Glory"                //         "GloryOffice"
#define PASS "Gloryart1!1"              // "razurahat@1!1"

//MQTT
//#define Client_Name "TANK1"
#define MQTT_Broker_IP "192.168.0.29"//<- Raspberry Server        //"192.168.1.9" <- GloryOffice
#define MQTT_Fallback_IP "0.0.0.0" //Implementation Required




