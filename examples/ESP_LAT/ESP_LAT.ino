/*
  ESP_LAT.ino - Example code for the LAT8266 library
  -> reading time
*/

#include <LAT8266.h>

#define SSID YOUR-WIFI-SSID
#define PASSWD YOUR-WIFI-PASSWD

void setup() {
  Serial.begin(LAT_BAUD); //enable Serial with 115200 BAUD. This can, however, be changed at will
  delay(500);
  Serial.println();
  
  LAT.LAT8266_HIDE_PASSWD(); //disable read access of password
  LAT.WIFI_SSID = SSID; //setting SSID
  //Need to reflect the password as it is not accessibly from the code directly
  LAT.reflect("wifipasswd = "+PASSWD);
  //Connect with default timeout (20s)
  if(LAT.connect()) {
    //push a simple http request to optain the time in Berlin
    LAT.reflect("hthost=worldtimeapi.org;htpath=/api/timezone/Europe/Berlin;htdefault;htcode;htheader?;htbody?");

    //push dummy request in raw mode
    LAT.setBody("{\"text\":\"This is the body\"}");
    LAT.setHeader("PUT /put HTTP/1.1\r\nHost: httpbin.org\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n");
    LAT.reflect("hthost=httpbin.org;htrequest;htcode;htheader?;htbody?");
  }
}

// the loop function runs over and over again forever
void loop() {
  //call to the command receiver. This is designed in a way so that users can easily enable or disable Serial request by disabling the command receiver, while still being able to "reflect" commands.
  LAT.loop_cmd();
}
