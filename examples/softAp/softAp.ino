/*
  softAP.ino - Example code for the LAT8266 library
  -> simple UART Interface to test this library
*/

#include <LAT8266.h>

#define ASSID YOUR-AP-SSID
#define APASSWD YOUR-AP-PASSWORD

void setup() {
  LAT.MDNSName = "lat";
  LAT.reflect("apssid="+String(ASSID)+";appasswd="+String(APASSWD)+";apconn=8");
  Serial.println("LAT8266 play-ground\nType 'LAT' or 'VER' to test the functionality of this library\nTo access the Website enter 'http://lat:82' on your browser.");
}

void loop() {
  LAT.loop_cmd();
}
