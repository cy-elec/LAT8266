/*
  helloWorld.ino - Example code for the LAT8266 library
  -> simple UART Interface to test this library
*/

#include <LAT8266.h>

#define WSSID YOUR-WIFI-SSID
#define WPASSWD YOUR-WIFI-PASSWORD

void setup() {
  LAT.MDNSName = "lat";
  LAT.reflect("wifissid="+String(WSSID)+";wifipasswd="+String(WPASSWD)+";wificonn");
  Serial.println("LAT8266 play-ground\nType 'LAT' or 'VER' to test the functionality of this library\nTo access the Website enter 'http://lat:82' on your browser.");
}

void loop() {
  LAT.loop_cmd();
}
