/*
   LAT8266.cpp - Light-weight Interface for ESP8266
   Copyright(c) 2022 Felix Kröhnert. All rights reserved.

   This library is distributed under the terms of the <license> and WITHOUT ANY WARRANTY
*/

#include "LAT8266.h"

/*
 * 
 * TODO
 * 
 * -read header
 * -read body
 * -wifi-psswd
 * -wifi-ssid
 * -wifi-rssi
 * -wifi-connect
 * -reboot
 */

bool LAT8266Class::connect(unsigned int timeout) {
  timeout+=millis();
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if(!WIFI_SSID||WIFI_SSID=="") {
    Serial.println("ERROR SSID");
    return false;
  }
  if(WIFI_PSSWD&&WIFI_PSSWD!="")
    WiFi.begin(WIFI_SSID, WIFI_PSSWD);
  else
    WiFi.begin(WIFI_SSID);
  Serial.print("Connecting");
  while(WiFi.status()!=WL_CONNECTED && millis()<timeout) {
    delay(500);
    Serial.print(".");
  }
  if(millis()>=timeout) {
    Serial.println("\nERROR TIMED OUT");
    return false;
  }
  
  Serial.print("\nConnected, IP address: ");
  Serial.print(WiFi.localIP());
  Serial.print("(");
  Serial.print(WiFi.RSSI());
  Serial.println(")");
  return true;
}

void print0(String str) {
  for(int i=0; i<str.length(); i++) {
    Serial.print((int)str.charAt(i));Serial.print("("+ String(str.charAt(i))+")");
  }
}

void LAT8266Class::reflect(String in) {
  char input2[maxInput+2];
  memcpy(input2, input, maxInput+2);
  in.toCharArray(input, maxInput+3);
  processCmd();
  memcpy(input, input2, maxInput+2);
}

void LAT8266Class::loop_cmd() {
  /*
   * Wait for command
  */  
  while(!Serial.available());
  input[currentPt] = Serial.read();
  currentPt++;
  if(currentPt>=2 && input[currentPt-2]=='\r' && input[currentPt-1=='\n']) {
    processCmd();
    currentPt = maxInput+2;
  }
  if(currentPt>=maxInput+2) {
    currentPt = 0;
  }
}

void LAT8266Class::processCmd() {
  input[currentPt-2] = '\0';

  normalizeStr(input);
  int num = separateStr(input, ';');
  char *pt = input;
  currentMode=MODE_CMD;
  
  for(int i=0; i<num; i++) {
    currentMode = getNextMode(pt);
    processArg(pt);
    while(*pt!='\0') pt++;
    pt++;
    
    //Adjust pt after mode
    if(currentMode==MODE_SET) {
      while(*pt!='\0') pt++;
      pt++;
    }
    else if(currentMode==MODE_GET) {
      pt++;
    }
  }
}

void LAT8266Class::processArg(char *src) {
  capitalizeStr(src);
  if(!strcmp(src, "LAT")) {           //TEST
    Serial.println("OK");
  }
  else if(!strcmp(src, "VER")) {      //VERSION
    Serial.println(LAT_VERSION);
  }
  else if(!strcmp(src, "HELP")) {     //HELP
    Serial.println(LAT_HELP);
  }
  else if(!strcmp(src, "HTREQUEST")) {    //HTTP Request
    httpRequest();
  }
  else if(!strcmp(src, "HTDEFAULT")) {    //HTTP Default Request
    httpUsingDefault();
  }
  else if(!strcmp(src, "HTHOST")) {     //HTTP Host
    cmdhttpHost(src);
  }
  else if(!strcmp(src, "HTPATH")) {     //HTTP Path
    cmdhttpPath(src);
  }
  else if(!strcmp(src, "HTTYPE")) {     //HTTP Type
    cmdhttpType(src);
  }
  else if(!strcmp(src, "HTPORT")) {     //HTTP Port
    cmdhttpPort(src);
  }
  else if(!strcmp(src, "HTHEADER")) {     //HTTP Header
    cmdhttpHeader(src);
  }
  else if(!strcmp(src, "HTBODY")) {     //HTTP Body
    cmdhttpBody(src);
  }
  else if(!strcmp(src, "HTCODE")) {     //HTTP Request Code
    cmdhttpCode();
  }

  
  else {
    Serial.print("ERROR ");
    Serial.println(src);
    Serial.println(LAT_HELP);
  }
}


void LAT8266Class::httpUsingDefault() {
  if(HTTP_host == "" || HTTP_path == "") {
    Serial.println("ERROR INCOMPlETE");
    return;
  }
  /**/
  if(!httpHeader.set(HTTP_type + " " + HTTP_path + " HTTP/1.1\r\n" +
                   "Host: " + HTTP_host + "\r\n" +
                   "Connection: close\r\n" +
                   "\r\n"
                  ))
    return;
  if(HTTP_type=="GET" || HTTP_type=="HEAD" || HTTP_type=="DELETE") {
    if(!httpBody.clear())
      return;
  }
  httpRequest();
}

void LAT8266Class::httpRequest() {
  if (WiFi.status() == WL_CONNECTED) {
    if (client.connect(HTTP_host, HTTP_Port))
    {
      String req = httpHeader.toString();
      req+=httpBody.toString();  
      client.print(req);

      
      bool writeBody=false, errored=false;
      if(!httpHeader.clear())
        errored=true;
      if(!httpBody.clear())
        errored=true;
      while (!errored&& (client.connected() || client.available()))
      {
        if (client.available())
        {
          String line = client.readStringUntil('\n') + "\n";
          if(line != "\r\n")
            if(writeBody) {
              if(!httpBody.addLine(line)) {
                errored=true;
                break;
              }
            }
            else {
              if(!httpHeader.addLine(line)) {
                errored=true;
                break;
              }
            }
          else
            writeBody=true; 
        }
      }
      if(!errored)
        Serial.println("OK");
      client.stop();
    }
    else
    {
      Serial.println("ERROR REQUEST");
      client.stop();
    }
  }
}

bool LAT8266Class::readHeader() {
  
}

bool LAT8266Class::readBody() {
  
}

//command interaction
void LAT8266Class::cmdhttpHost(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        HTTP_host = String(pt);
        Serial.println("OK");
        break;
    case MODE_GET: 
        Serial.println(HTTP_host);
        break;
    default: Serial.println("ERROR NOCMD"); break;
  }
}

void LAT8266Class::cmdhttpPath(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        HTTP_path = String(pt);
        Serial.println("OK");
        break;
    case MODE_GET: 
        Serial.println(HTTP_path);
        break;
    default: Serial.println("ERROR NOCMD"); break;
  }
}

void LAT8266Class::cmdhttpType(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        HTTP_type = String(pt);
        Serial.println("OK");
        break;
    case MODE_GET: 
        Serial.println(HTTP_type);
        break;
    default: Serial.println("ERROR NOCMD"); break;
  }
}
void LAT8266Class::cmdhttpPort(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        HTTP_Port = String(pt).toInt();
        Serial.println("OK");
        break;
    case MODE_GET: 
        Serial.println(HTTP_Port);
        break;
    default: Serial.println("ERROR NOCMD"); break;
  }
}

void LAT8266Class::cmdhttpHeader(char *pt) {
  switch(currentMode) {
    case MODE_SET: Serial.println("ERROR NOCMD"); break;
    case MODE_GET: printHeader(); break;
    default: Serial.println("ERROR NOCMD"); break;
  }
}

void LAT8266Class::cmdhttpBody(char *pt) {
  switch(currentMode) {
    case MODE_SET: Serial.println("ERROR NOCMD"); break;
    case MODE_GET: printBody(); break;
    default: Serial.println("ERROR NOCMD"); break;
  }
}

void LAT8266Class::cmdhttpCode() {
  Serial.println(getHttpCode());
}

void LAT8266Class::printHeader() {
  Serial.println(httpHeader.toString());
}

void LAT8266Class::printBody() {
  Serial.println(httpBody.toString());
}

int LAT8266Class::getHttpCode() {
  String hd = httpHeader.toString();
  if(hd.substring(0, 4)!="HTTP") return -1;
  int index1 = hd.indexOf(' ');
  if(index1 < 1) return -2;
  int index2 = hd.indexOf(' ', index1+1);
  if(index2 < 1) return -3;
  return hd.substring(index1, index2).toInt();
}

/*
 * CMD - 0
 * SET - 1
 * GET - 2
*/
cmdmodes_t LAT8266Class::getNextMode(char *src) {
  while(*src!='\0') {
    if(*src=='=') {
      *src='\0';
      return MODE_SET;
    }
    else if(*src=='?') {
      *src='\0';
      return MODE_GET;
    }
    src++;
  }
  return MODE_CMD;
}

int LAT8266Class::separateStr(char *src, char sep) {
  int i=1, l=0;
  while(*src!='\0') {
    if(*src==sep && (l<1||*(src-1)!='\\')) {
      *src='\0';
      i++;
    }
    src++;
    l++;
  }
  return i;
}
void LAT8266Class::capitalizeStr(char *src) {
  while(*src!='\0') {
    if(*src>96 && *src<123)
      *src&=~(1<<5); 
    src++;
  }
}
void LAT8266Class::normalizeStr(char *src) {
  int l=0;
  while(*src!='\0') {
    if(*src==32 && (l<1||*(src-1)!='\\'))
     strcpy(src, src+1);
    src++;
  }
}

LAT8266Class LAT;
