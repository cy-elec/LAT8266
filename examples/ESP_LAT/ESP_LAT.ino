/*
  ESP_LAT.ino - Example code for the LAT8266 library
*/

#include <ESP8266WiFi.h>
#include "LAT8266.h"

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println();
  
  LAT.WIFI_SSID = "ILR8OURxtkEI8D1p";
  LAT.WIFI_PSSWD = "Z1I25SWOmgqY3rSw";
  if(LAT.wifi_connect(5000)) {
    LAT.reflect("hthost=worldtimeapi.org;htpath=/api/timezone/Europe/Berlin;htdefault;htcode;htheader?;htbody?");
  }
}

// the loop function runs over and over again forever
void loop() {
  LAT.loop_cmd();
}
