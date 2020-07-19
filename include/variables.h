//Insert all sensor header here.
//#include "DOpH.cpp"
//#include "bme680.cpp"

//Topic Declaration12
char tank_addr[8] = "TANK_X"; //Insert TANK Address
IPAddress local_IP(192, 168, 0, 15);

IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);


char HEARTBEAT_TOPIC[18];
char DO_TOPIC[20];
char pH_TOPIC[20];

//WIFI
#define SSID "GloryAgro"   //"Glory"                //         "GloryOffice"              //"AsmaulsRedmi"//
#define PASS "Gloryart1!1" // "razurahat@1!1"                                             //"hackyou123"//

//MQTT
//#define Client_Name "TANK1"
#define MQTT_Broker_IP "192.168.0.4" //<- AgroPC Broker//"192.168.0.2" //<- Raspberry Broker        //"192.168.1.9" <- GloryOffice
#define MQTT_Fallback_IP "0.0.0.0"    //Implementation Required