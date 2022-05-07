/*
   LAT8266.h - Light-weight Interface for ESP8266
   Copyright(c) 2022 Felix Kröhnert. All right reserved.

   This library is distributed under the terms of the <license> and WITHOUT ANY WARRANTY
*/

#ifndef LAT8266_H_
#define LAT8266_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "HTTPContent.h"

#define LAT_VERSION F("Version.1.1 - LAT by Felix Kröhnert")
#define LAT_HELP F("Please refer to the documentation here <link>")

/*
     * CMD - 0
     * SET - 1
     * GET - 2
    */
typedef enum cmdmodes {
  MODE_CMD = 0, MODE_SET = 1, MODE_GET = 2
} cmdmodes_t;
 
class LAT8266Class {
  public:
  
    char* WIFI_SSID = "";
    char* WIFI_PSSWD = NULL;
  
    bool wifi_connect(int);

    void reflect(String);

    void loop_cmd();

    /*
     * HTTP Section
     */
    
    void printHeader();
    void printBody();
    int getHttpCode();
    void httpUsingDefault();
    void httpRequest();
    
  private:

    const static int maxInput = 1024;
    char input[maxInput+2];
    int currentPt=0;
    cmdmodes_t currentMode;
    
    WiFiClient client;

    /*
     * HTTP SECTION
     * 
     * ################################*/
    String HTTP_host = "";
    String HTTP_type = "GET";
    String HTTP_path = "/";
    int HTTP_Port = 80;
    HTTPContent httpHeader = HTTPContent(HTTP_HEADER);
    HTTPContent httpBody = HTTPContent(HTTP_BODY);

    bool readHeader();
    bool readBody();

    //command interaction
    void cmdhttpHost(char *pt);
    void cmdhttpPath(char *pt);
    void cmdhttpType(char *pt);
    void cmdhttpPort(char *pt);
    void cmdhttpHeader(char *pt);
    void cmdhttpBody(char *pt);
    void cmdhttpCode();
    
    /* ################################
     * HTTP SECTION
     */
     
    void processCmd();
    void processArg(char *);
    
    cmdmodes_t getNextMode(char *);
    
    int separateStr(char *, char);
    void capitalizeStr(char *);
    void normalizeStr(char *);
    
};

extern LAT8266Class LAT;

#endif
