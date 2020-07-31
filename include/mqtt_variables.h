
/* IP Reserved List
TANK_1      192,168,0,14
TANK_2      192,168,0,15
TANK_3      192,168,0,16
*/

//Topic Declaration12
char tank_addr[8] = "TANK_X"; //Insert TANK Address

char HEARTBEAT_TOPIC[18];
char DO_TOPIC[20];
char pH_TOPIC[20];

//MQTT
#define MQTT_Broker_IP   "192.168.0.2" //<- Raspberry Broker "192.168.0.4" //<- AgroPC Broker    //"192.168.1.9" <- GloryOffice ////   "192.168.43.1"   // -< Redmi Broker
#define MQTT_Fallback_IP "0.0.0.0"   //Implementation Required