/*
   LAT8266.cpp - Light-weight Interface for ESP8266
   Copyright (c) 2022 Felix Kröhnert. All rights reserved.

   This library is distributed under the terms of the MIT License and WITHOUT ANY WARRANTY
*/

#include "LAT8266.h"

LAT8266Class::LAT8266Class() {
  strcpy(mdnsService, "http");
  
  MAC = WiFi.macAddress();
  MAC.replace(":","");
  MDNSName = MDNS_DEFAULTNAMEPRE+MAC;
  WebServerInit();
}

bool LAT8266Class::connect(unsigned long timeout, bool silent) {

  unsigned long otim=timeout;
  timeout+=millis();
  WiFi.mode(WIFI_STA);

  if(!disconnect(otim)) {
    Serial.println("ERROR TIMEDOUT");
    return false;
  }
  if(WIFI_SSID=="") {
    Serial.println("ERROR SSID");
    return false;
  }
  if(WIFI_PASSWD!="")
    WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  else
    WiFi.begin(WIFI_SSID);
  if(!silent)
    Serial.print("Connecting");
  while(WiFi.status()!=WL_CONNECTED && millis()<timeout) {
    delay(500);
    if(!silent)
      Serial.print(".");
  }
  if(millis()>=timeout) {
    if(!silent) Serial.println();
    Serial.println("ERROR TIMEDOUT");
    return false;
  }
  
  if(!silent) {
    Serial.print("\nConnected, IP address: ");
    Serial.print(WiFi.localIP());
    Serial.print("(");
    Serial.print(WiFi.RSSI());
    Serial.println(")");
    Serial.print("MDNS Name: ");
    Serial.println(MDNSName);
  }
  if(!MDNS.begin(MDNSName) && MDNSName!=(MDNS_DEFAULTNAMEPRE+MAC)) {
	MDNSName = MDNS_DEFAULTNAMEPRE+MAC;
  	MDNS.begin(MDNSName);
  }
  
  return true;
}

bool LAT8266Class::disconnect(unsigned long timeout) {
  timeout+=millis();
  while(WiFi.status()==WL_CONNECTED && millis()<timeout) {
    WiFi.disconnect();
    delay(100);
  }
  if(millis()>=timeout) return false;
  return true;
}

void LAT8266Class::scan(unsigned long timeout) {
  unsigned long otim = timeout;
  timeout+=millis();
  int isConnected = WiFi.isConnected();

  if(!disconnect(otim)) {
    Serial.println("ERROR TIMEDOUT");
    return;
  }

  if(WiFi.scanComplete()!=-1)
    WiFi.scanNetworks(true);
  int n=-1;
  while((n=WiFi.scanComplete())<0 && millis()<timeout) {
    delay(100);
  }
  if(isConnected) {
	if(millis()<timeout) {
		if(connect(timeout-millis(), true)&&n>0) {
		Serial.println(n);
		}
	}
	else
		Serial.println("ERROR TIMEDOUT_NOCONN");
  }
}

void LAT8266Class::printLastScan() {
  int n=WiFi.scanComplete();
  if(n==-2) {
    Serial.println("ERROR NOSCAN");
  }
  else if(n==-1) {
    Serial.println("ERROR SCANNING");
  }
  else {
    for(int i=0; i<n; i++) {
      Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
    }
  }
}

void LAT8266Class::cmdwifiSSID(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        WIFI_SSID = String(pt);
        Serial.println("OK");
        break;
    case MODE_GET: 
        Serial.println(WIFI_SSID);
        break;
    default: Serial.println("ERROR NOCMD"); break;
  }
}

void LAT8266Class::LAT8266_HIDE_PASSWD() {
  LAT8266_HIDE_PASSWD_=true;
}
void LAT8266Class::cmdwifiPASSWD(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        WIFI_PASSWD = String(pt);
        Serial.println("OK");
        break;
    case MODE_GET: 
        if(LAT8266_HIDE_PASSWD_)
          Serial.println("ERROR ACCESS");
        else
          Serial.println(WIFI_PASSWD);
        break;
    default: Serial.println("ERROR NOCMD"); break;
  }
}

void LAT8266Class::cmdwifiRSSI() {
  Serial.println(WiFi.RSSI());
}

void LAT8266Class::cmdwifiCONN(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        if(connect(strtoul(pt, NULL, 0), true))
          Serial.println("OK");
        break;
    case MODE_GET: 
        Serial.println(WiFi.isConnected());
        break;
    default: 
        if(connect(WIFI_CONNDEFAULTTIME, true))
          Serial.println("OK");
        break;
  }
}

void LAT8266Class::cmdwifiIP() {
  Serial.println(WiFi.localIP());
}

void LAT8266Class::cmdwifiCHNL() {
  Serial.println(WiFi.channel());
}

void LAT8266Class::cmdwifiMAC() {
  Serial.println(WiFi.macAddress());
}

void LAT8266Class::cmdwifiNAME(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        WiFi.hostname(String(pt));
        break;
    case MODE_GET: 
        Serial.println(WiFi.hostname());
        break;
    default: Serial.println(WiFi.hostname()); break;
  }
}

void LAT8266Class::cmdwifiAUTO(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        capitalizeStr(pt);
        if(!strcmp(pt, "TRUE")) WiFi.setAutoReconnect(true);
        else if(!strcmp(pt, "FALSE")) WiFi.setAutoReconnect(false);
        else {
          Serial.println("ERROR VALUE");
          break;
        }
        Serial.println("OK");
        break;
    case MODE_GET: 
        Serial.println("ERROR NOCMD"); break;
    default: 
        Serial.println("ERROR NOCMD"); break;
  }
}

void LAT8266Class::cmdwifiAUTOSTART(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        capitalizeStr(pt);
        if(!strcmp(pt, "TRUE")) WiFi.setAutoConnect(true);
        else if(!strcmp(pt, "FALSE")) WiFi.setAutoConnect(false);
        else {
          Serial.println("ERROR VALUE");
          break;
        }
        Serial.println("OK");
        break;
    case MODE_GET: 
        Serial.println(WiFi.getAutoConnect()); break;
    default: 
        Serial.println("ERROR NOCMD"); break;
  }
}

void LAT8266Class::cmdwifiSCAN(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        scan(strtoul(pt, NULL, 0));
        break; 
    case MODE_GET: 
        printLastScan();
        break;
    default: scan(WIFI_SCANDEFAULTTIME); break;
  }
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
  MDNS.update();
  WebServerRun();
  if(!Serial.available()) return;
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
    Serial.println(LAT_HELP + (WiFi.isConnected()?String(" or here http://" + MDNSName + ":82/"):""));
  }
  else if(!strcmp(src, "HTREQUEST")) {    //HTTP Request
    httpRequest();
  }
  else if(!strcmp(src, "HTTIMEOUT")) {    //HTTP Timeout
    cmdhttpTimeout(src);
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
  else if(!strcmp(src, "HTHEADERIN")) {     //HTTP HeaderInput
    printHeaderInput();
  }
  else if(!strcmp(src, "HTBODYIN")) {     //HTTP BodyInput
    printBodyInput();
  }
  else if(!strcmp(src, "HTCODE")) {     //HTTP Request Code
    cmdhttpCode();
  }
  else if(!strcmp(src, "HTBUFFER")) {     //HTTP Toggle Buffer
    cmdhttpToggle(src);
  }
  else if(!strcmp(src, "WIFISSID")) {     //WIFI SSID
    cmdwifiSSID(src);
  }
  else if(!strcmp(src, "WIFIPASSWD")) {     //WIFI PASSWD
    cmdwifiPASSWD(src);
  }
  else if(!strcmp(src, "WIFIRSSI")) {     //WIFI RSSI
    cmdwifiRSSI();
  }
  else if(!strcmp(src, "WIFICONN")) {     //WIFI Connect + timeout
    cmdwifiCONN(src);
  }
  else if(!strcmp(src, "WIFIIP")) {       //WIFI IP
    cmdwifiIP();
  }
  else if(!strcmp(src, "WIFICHNL")) {       //WIFI Channel
    cmdwifiCHNL();
  }
  else if(!strcmp(src, "WIFIMAC")) {       //WIFI MacAddress
    cmdwifiMAC();
  }
  else if(!strcmp(src, "WIFINAME")) {       //WIFI Name
    cmdwifiNAME(src);
  }
  else if(!strcmp(src, "WIFIAUTO")) {       //WIFI AutoReconnect
    cmdwifiAUTO(src);
  }
  else if(!strcmp(src, "WIFIAUTOSTART")) {       //WIFI AutoConnect
    cmdwifiAUTOSTART(src);
  }
  else if(!strcmp(src, "WIFISCAN")) {       //WIFI SCAN
    cmdwifiSCAN(src);
  }
  else if(!strcmp(src, "WIFIDISC")) {       //WIFI Disconnect
    disconnect();
    Serial.println("OK");
  }
  else if(!strcmp(src, "MDNSSERVICE")) {       //MDNS Service
    cmdmdnsService(src);
  }
  else if(!strcmp(src, "MDNSQNAME")) {       //MDNS Query Name
    cmdmdnsQueryName(src);
  }
  else if(!strcmp(src, "MDNSNAME")) {       //MDNS Host Name
    Serial.println(MDNSName);
  }
  else if(!strcmp(src, "REBOOT")) {       //reboot
    ESP.reset();
  }


  else {
    Serial.print("ERROR ");
    Serial.println(src);
    Serial.println(LAT_HELP);
  }
}


void LAT8266Class::httpUsingDefault() {
  if(HTTP_host == "" || HTTP_path == "") {
    Serial.println("ERROR INCOMPLETE");
    return;
  }
  /**/
  if(!httpHeaderInput.set(HTTP_type + " " + HTTP_path + " HTTP/1.1\r\n" +
                   "Host: " + HTTP_host + "\r\n" +
                   "Connection: close\r\n" +
                   "\r\n"
                  ))
    return;
  if(HTTP_type=="GET" || HTTP_type=="HEAD" || HTTP_type=="DELETE") {
    if(!httpBodyInput.clear()) {
      Serial.println("ERROR BODYFOUND");
      return;
    }
  }
  httpRequest();
}

void LAT8266Class::httpRequest() {
  
  bool errored=false;
  if(toggleBuffer) {
    if(!httpHeader.clear())
      errored=true;
    if(!httpBody.clear())
      errored=true;
  }
  
  if (WiFi.status() == WL_CONNECTED)
  {
    unsigned long endtimeout = glb_RequestTimeout?millis()+glb_RequestTimeout:0;

    if(HTTP_host.endsWith(".local")) {
      int n = mdnsQueryName(HTTP_host.c_str(), glb_RequestTimeout);
      if(n == -1) {
        Serial.println("ERROR MDNSQ");
        return;
      }
      HTTP_Port = MDNS.port(n);
      HTTP_host = toIP(MDNS.IP(n));
    }

    if (client.connect(HTTP_host, HTTP_Port))
    {
      String req = httpHeaderInput.toString().substring(0, httpHeaderInput.getSize()-2);
      req+=(httpBodyInput.getSize()==0?"":"Content-length: "+String(httpBodyInput.getSize())+"\r\n")+"\r\n";
      req+=httpBodyInput.toString();  
      client.print(req);
      
      bool writeBody=false;
      while (!errored && (client.connected() || client.available()) && (glb_RequestTimeout == 0 || millis()<endtimeout))
      {
        if (client.available())
        {
          endtimeout = glb_RequestTimeout?millis()+glb_RequestTimeout:0;
          String line = client.readStringUntil('\n') + "\n";
          if(toggleBuffer) {
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
          else
            Serial.print(line);
        }
      }
      if(glb_RequestTimeout!=0 && httpHeader.getSize()==0 && millis()>=endtimeout) {
        Serial.println("ERROR TIMEDOUT");
      }
      else if(httpHeader.getSize()==0||errored||!WiFi.isConnected())
        Serial.println("ERROR CANCELLED");
      else if(toggleBuffer)
        Serial.println("OK");
      client.stop();
    }
    else
      Serial.println("ERROR REQUEST");
  }
  else 
    Serial.println("ERROR NOCONN");
}

void LAT8266Class::cmdhttpTimeout(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        glb_RequestTimeout = strtoul(pt, NULL, 0);
        Serial.println("OK");
        break;
    case MODE_GET: 
        Serial.println(glb_RequestTimeout);
        break;
    default: Serial.println("ERROR NOCMD"); break;
  }
}

bool LAT8266Class::readHeader() {
  return readHTTPContent(&httpHeaderInput);
}

bool LAT8266Class::readBody() {
  return readHTTPContent(&httpBodyInput);
}

bool LAT8266Class::setHeader(String in) {
  return httpHeaderInput.set(in);
}

bool LAT8266Class::setBody(String in) {
  return httpBodyInput.set(in);
}

bool LAT8266Class::readHTTPContent(HTTPContent *c) {
  Serial.println("READ");
  
  bool reading = true;
  String line = "";
  c->clear();

  while(reading) {
    /*
    * Wait for input
    */  
    unsigned long timeout=millis()+HTTP_READDEFAULTTIME;
    while(!Serial.available()) {
      if(millis()>=timeout) {
        Serial.println("ERROR TIMEDOUT");
        return false;
      }
    }
    line += String((char)Serial.read());
    Serial.println(line);
    if(line.endsWith("\r\n")) {
      if(!c->addLine(line)) {
        return false;
      }
      if(line=="\r\n")
        break;
      line="";
    }
  }
  Serial.println("OK");
  return true;
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
        HTTP_type.toUpperCase();
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
        HTTP_Port = atoi(pt);
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
    case MODE_SET: readHeader(); break;
    case MODE_GET: printHeader(); break;
    default: Serial.println("ERROR NOCMD"); break;
  }
}

void LAT8266Class::cmdhttpBody(char *pt) {
  switch(currentMode) {
    case MODE_SET: readBody(); break;
    case MODE_GET: printBody(); break;
    default: Serial.println("ERROR NOCMD"); break;
  }
}

void LAT8266Class::cmdhttpCode() {
  Serial.println(getHttpCode());
}

void LAT8266Class::cmdhttpToggle(char *pt) {
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        capitalizeStr(pt);
        if(!strcmp(pt, "TRUE")) toggleBuffer = true;
        else if(!strcmp(pt, "FALSE")) toggleBuffer = false;
        else {
          Serial.println("ERROR VALUE");
          break;
        }
        Serial.println("OK");
        break;
    case MODE_GET: 
        Serial.println(toggleBuffer);
        break;
    default: 
        toggleBuffer=!toggleBuffer;
        Serial.println("OK"); break;
  }
}

int LAT8266Class::mdnsQueryName(const char *name_, unsigned long timeout) {

  char *name = (char*)malloc(strlen(name_));
  if(!name) {
    Serial.println("ERROR ALLOCQ");
    return -1;
  }
  strcpy(name, name_);
  bool lset = false;
  int l = strlen(name);
  if(!strcmp(name+(l-6), ".local")) {
    name[l-6] = '\0';
  }

  timeout += millis();
  int res = -1;
  
  if(!MDNS.begin(MDNSName)) {
    Serial.println("ERROR MDNSBEGIN");
    return -1;
  }
  
  while(millis()<timeout && res==-1) {
    //try tcp
    int n = MDNS.queryService(mdnsService, "tcp");
    if(n==0)
      n = MDNS.queryService(mdnsService, "udp");
    if(n!=0) {
      for(int i=0; i<n; i++) {
        if(!strcmp(name, MDNS.hostname(i).substring(0, MDNS.hostname(i).length()-6).c_str())) {
          res = i;
          break;
        }
      }
    }
  }

  return res;
}

void LAT8266Class::cmdmdnsService(char *pt) {
  char *tmp;
  switch(currentMode) {
    case MODE_SET: 
        while(*(pt++)!='\0');
        tmp = (char*)realloc(mdnsService, strlen(pt));
        if(!tmp) {
          Serial.println("ERROR ALLOCM");
        }
        else {
          mdnsService = tmp;
          strcpy(mdnsService, pt);
          Serial.println("OK");
        }
        break;
    case MODE_GET: 
        Serial.println(mdnsService);
        break;
    default: 
        Serial.println("ERROR NOCMD"); break;
  }
}

String LAT8266Class::toIP(unsigned long lip) {
  int a = lip%256;
  lip=lip>>8;
  int b = lip%256;
  lip=lip>>8;
  int c = lip%256;
  lip=lip>>8;
  int d = lip%256;
  return String(a)+"."+String(b)+"."+String(c)+"."+String(d);
}

void LAT8266Class::cmdmdnsQueryName(char *pt) {
  if(currentMode!=MODE_SET) {
    Serial.println("ERROR NOCMD");
    return;
  }
  while(*(pt++)!='\0');
    
  int n = mdnsQueryName(pt);
  if(n == -1) {
    Serial.println("ERROR MDNSQ");
    return;
  }
  Serial.printf("%s:%d\n", toIP(MDNS.IP(n)).c_str(), MDNS.port(n));
}

void LAT8266Class::printHeader() {
  Serial.println(httpHeader.toString());
}

void LAT8266Class::printBody() {
  Serial.println(httpBody.toString());
}

void LAT8266Class::printHeaderInput() {
  Serial.println(httpHeaderInput.toString());
}

void LAT8266Class::printBodyInput() {
  Serial.println(httpBodyInput.toString());
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
    if(*src==' ' && (l<1||*(src-1)!='\\'))
      strcpy(src, src+1);
    else if(*src==' ')
      strcpy(src-1, src);
    src++;
    l++;
  }
}

LAT8266Class LAT;
