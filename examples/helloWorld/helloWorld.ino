/*
  ESP_LAT.ino - Example code for the LAT8266 library
  -> simple UART Interface to test this library
*/

#include <LAT8266.h>

void setup() {
  Serial.begin(LAT_BAUD); //enable Serial with 115200 BAUD. This can, however, be changed at will
  delay(500);
  Serial.println("LAT8266 play-ground\nType 'LAT' or 'VER' to test the functionality of this library");
}

void loop() {
  LAT.loop_cmd();
}

