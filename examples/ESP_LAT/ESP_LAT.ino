/*
  ESP_LAT.ino - Example code for the LAT8266 library
  -> reading time
*/

#include <ESP8266WiFi.h>
#include <LAT8266.h>

void setup() {
  Serial.begin(LAT_BAUD); //enable Serial with 115200 BAUD. This can, however, be changed at will
  delay(500);
  Serial.println();
  
  LAT.LAT8266_HIDE_PSSWD(); //disable read access of password
  LAT.WIFI_SSID = "ILR8OURxtkEI8D1p"; //setting SSID
  //Need to reflect the password as it is not accessibly from the code directly
  LAT.reflect("wifipsswd = Z1I25SWOmgqY3rSw");
  //Connect with default timeout (20s)
  if(LAT.connect()) {
    //push a simple http request to optain the time in Berlin
    LAT.reflect("hthost=worldtimeapi.org;htpath=/api/timezone/Europe/Berlin;htdefault;htcode;htheader?;htbody?");
  }
}

// the loop function runs over and over again forever
void loop() {
  //call to the command receiver. This is designed in a way so that users can easily enable or disable Serial request by disabling the command receiver, while still being able to "reflect" commands.
  LAT.loop_cmd();
}
