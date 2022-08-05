/*
   WebServer.cpp - WebServer for "LAT8266"
   Copyright (c) 2022 Felix Kröhnert. All rights reserved.

   This library is distributed under the terms of the MIT License and WITHOUT ANY WARRANTY
*/

#include "WebServer.h"
#include "WebServerSrc"

AsyncWebServer server(82);
bool WEB_INTERFACE=true;

void nof(AsyncWebServerRequest *req) {
	req->send_P(404, "text/html", nof_html);
}


void WebServerInit(void * LAT, int (*queue)(void *, String), String (*result)(void *, int)) {

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
		req->send(200, "text/html", root_html);
	});
	server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *req) {
		req->send_P(200, "text/html", index_html, processor);
	});
	server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *req) {
		req->send_P(200, "text/css", styles_css);
	});
	server.on("/scripts.js", HTTP_GET, [](AsyncWebServerRequest *req) {
		req->send_P(200, "application/javascript ", scripts_js);
	});
	server.on("/image", HTTP_GET, [](AsyncWebServerRequest *req) {
		req->send_P(200, "text/plain ", image, processor);
	});

	server.on("/reflect", HTTP_ANY, [=](AsyncWebServerRequest *req) {
		if(!WEB_INTERFACE) {
			req->send(401, "text/html", "<h1>Interface disabled</h1>");
			return;
		}
		if(req->method() == HTTP_GET) {
			req->send(418, "text/plain", "I'm a teapot");
		}
		else if(req->method() == HTTP_POST) {
			if(req->hasParam("reflect", true)) {
				int id=queue(LAT, req->getParam("reflect", true)->value());
				if(id==-1)
					req->send(429, "text/plain", String(id));
				else if(id==-2)
					req->send(200, "text/plain", "OK\n");
				else
					req->send(202, "text/plain", String(id));
			}
			else
				req->send(400, "text/plain", "Incorrect Argument");
		}
		else if(req->method()==HTTP_PUT) {
			if(req->hasParam("id", true)) {
				String r=result(LAT, req->getParam("id", true)->value().toInt());
				req->send((r=="wait"?202:(r=="inc"?408:200)), "text/plain", r);
			}
			else
				req->send(400, "text/plain", "Incorrect Argument");
		}
		else {
			nof(req);
		}
	});
	
	server.onNotFound([](AsyncWebServerRequest *req) {
		nof(req);
	});

	server.begin();
}

void WebServerRun(bool WEB_INTERFACE_) {
	WEB_INTERFACE = WEB_INTERFACE_;
}


String htmlFormat(String var) {
	var.replace("ö", "&ouml;");
	var.replace("ä", "&uuml;");
	var.replace("ü", "&auml;");
	var.replace("Ö", "&Ouml;");
	var.replace("Ä", "&Auml;");
	var.replace("Ü", "&Uuml;");
	var.replace("ß", "&szlig;");
	return var;
}

String processor(const String& var){
  if(var == "IP"){
    return String(WiFi.localIP().toString());
  }
  if(var == "VERSION") {
	return String(htmlFormat(LAT_VERSION));
  }
  if(var == "MAX_REFLECT") {
	return String(MAX_REFLECT);
  }
  return String();
}