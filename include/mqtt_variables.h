

#define WIFI_SSID "GloryCTRL"
#define WIFI_PASSWORD "Gloryhabib1@1"

// Declare static IPs of device


/* Static IP
10.0.1.19 -> Pond 8             OTA Address 10.0.1.19:4000/update
10.0.1.20 -> Tank 1             OTA Address 10.0.1.20:4000/update
10.0.1.21 -> Tank 2             OTA Address 10.0.1.21:4000/update
10.0.1.22 -> Tank 3             OTA Address 10.0.1.22:4000/update
*/


/*Note for Future, DHCP might be used in the future, a though a reporting database needs to be setup to keep track of the field devices*/
IPAddress local_IP(10, 0, 1, 20);
IPAddress gateway(10, 0, 1, 1);
IPAddress subnet(255, 255, 0, 0);


#define MQTT_HOST IPAddress(10, 0, 1, 2)
#define MQTT_PORT 1883

