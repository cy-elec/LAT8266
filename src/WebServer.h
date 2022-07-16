/*
   WebServer.h - WebServer for "LAT8266"
   Copyright (c) 2022 Felix Kröhnert. All rights reserved.

   This library is distributed under the terms of the MIT License and WITHOUT ANY WARRANTY
*/
#ifndef WebServer_H_
#define WebServer_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

void WebServerInit();

String processor(const String&);

#endif