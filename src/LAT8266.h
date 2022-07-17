/*
   LAT8266.h - Light-weight Interface for ESP8266
   Copyright (c) 2022 Felix Kröhnert. All rights reserved.

   This library is distributed under the terms of the MIT License and WITHOUT ANY WARRANTY
*/

#ifndef LAT8266_H_
#define LAT8266_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "WebServer.h"
#include "HTTPContent.h"
#include "PREDEFINE.h"

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
    LAT8266Class();

    String WIFI_SSID = "";
    void LAT8266_HIDE_PASSWD();    

    bool connect(unsigned long = WIFI_CONNDEFAULTTIME, bool = false);
    bool disconnect(unsigned long = WIFI_CONNDEFAULTTIME);
    void scan(unsigned long = WIFI_SCANDEFAULTTIME);
    void printLastScan();
    
    void reflect(String);

    void loop_cmd();

    /*
     * HTTP Section
     */
    
    void printHeader();
    void printBody();
    void printHeaderInput();
    void printBodyInput();
    bool setHeader(String);
    bool setBody(String);
    int getHttpCode();
    void httpUsingDefault();
    unsigned long glb_RequestTimeout = HTTP_REQDEFAULTTIME;
    void httpRequest();
    
    String MAC;
    String MDNSName;

  private:

    const static int maxInput = 1024;
    char input[maxInput+2];
    int currentPt=0;
    cmdmodes_t currentMode;
    
    WiFiClient client;
  
    String WIFI_PASSWD = "";

    //command interaction
    void cmdwifiSSID(char *pt);
    /*
      WRITE ONLY MODE CAN BE ENABLED BY CALLING LAT8266_HIDE_PASSWD() !!WARNING!! THIS CAN ONLY BE DONE ONCE BEFORE RESTARTING CHIP
    */
    bool LAT8266_HIDE_PASSWD_=false;
    void cmdwifiPASSWD(char *pt);
    void cmdwifiRSSI();
    void cmdwifiCONN(char *pt);
    void cmdwifiIP();
    void cmdwifiCHNL();
    void cmdwifiMAC();
    void cmdwifiNAME(char *pt);
    void cmdwifiAUTO(char *pt);
    void cmdwifiAUTOSTART(char *pt);
    void cmdwifiSCAN(char *pt);

    /*
     * HTTP SECTION
     * ################################*/
    String HTTP_host = "";
    String HTTP_type = "GET";
    String HTTP_path = "/";
    int HTTP_Port = 80;
    bool toggleBuffer = true;
    HTTPContent httpHeader = HTTPContent(HTTP_HEADER);
    HTTPContent httpBody = HTTPContent(HTTP_BODY);
    HTTPContent httpHeaderInput = HTTPContent(HTTP_HEADER);
    HTTPContent httpBodyInput = HTTPContent(HTTP_BODY);

    bool readHeader();
    bool readBody();
    bool readHTTPContent(HTTPContent*);

    //command interaction
    void cmdhttpTimeout(char *pt);
    void cmdhttpHost(char *pt);
    void cmdhttpPath(char *pt);
    void cmdhttpType(char *pt);
    void cmdhttpPort(char *pt);
    void cmdhttpHeader(char *pt);
    void cmdhttpBody(char *pt);
    void cmdhttpCode();
    void cmdhttpToggle(char *pt);
    
    /* ################################
     * HTTP SECTION
     */
     
    /*
     * MDNS SECTION (to resolve name for http request)
     * ################################*/

    int mdnsQueryName(const char *, unsigned long = 2000);
    char *mdnsService = (char*)malloc(5);
    void cmdmdnsService(char *);
    String toIP(unsigned long);
    void cmdmdnsQueryName(char *);

    /* ################################
     * MDNS SECTION
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
