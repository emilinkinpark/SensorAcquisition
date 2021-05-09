/*   Caution Read This before updating field devices using ElegantOs

1) local_IP adress must be set accordingly;
2) tankADDR must be updated
3) Update Salinity in conversions.cpp 
 
*/


/* Static IP
10.0.1.19 -> Pond 8             OTA Address 10.0.1.19:4000/update
10.0.1.20 -> Tank 1             OTA Address 10.0.1.20:4000/update
10.0.1.21 -> Tank 2             OTA Address 10.0.1.21:4000/update
10.0.1.22 -> Tank 3             OTA Address 10.0.1.22:4000/update
*/

#define WIFI_SSID "GloryCTRL"
#define WIFI_PASSWORD "Gloryhabib1@1"

// Declare static IPs of device


/*Note for Future, DHCP might be used in the future, a though a reporting database needs to be setup to keep track of the field devices*/
IPAddress local_IP(10, 0, 1, 22);
IPAddress gateway(10, 0, 1, 1);
IPAddress subnet(255, 255, 0, 0);

#define MQTT_HOST IPAddress(10, 0, 1, 2)
#define MQTT_PORT 1883
#define tankADDR "MQTT/TANK3"
