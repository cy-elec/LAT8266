/*
   WebServer.cpp - WebServer for "LAT8266"
   Copyright (c) 2022 Felix Kröhnert. All rights reserved.

   This library is distributed under the terms of the MIT License and WITHOUT ANY WARRANTY
*/

#include "WebServer.h"
#include "WebServerSrc"
AsyncWebServer server(82);

void WebServerInit() {
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
		req->send(200, "text/html", root_html);
	});
	server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *req) {
		req->send_P(200, "text/html", index_html, processor);
	});
	server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *req) {
		req->send(200, "text/css", styles_css);
	});
	server.on("/scripts.js", HTTP_GET, [](AsyncWebServerRequest *req) {
		req->send(200, "application/javascript ", scripts_js);
	});


	server.begin();
}

String processor(const String& var){
  if(var == "IP"){
    return String(WiFi.localIP().toString());
  }
  return String();
}