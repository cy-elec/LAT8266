/*
   LAT8266.cpp - Light-weight Interface for ESP8266
   Copyright (c) 2022 Felix Kröhnert. All rights reserved.

   This library is distributed under the terms of the MIT License and WITHOUT ANY WARRANTY
*/

#include "LAT8266.h"

int _queuer(void *obj, String arg)
{
	return static_cast<LAT8266Class *>(obj)->queue_reflect(arg);
}
String _resultr(void *obj, int id)
{
	return static_cast<LAT8266Class *>(obj)->queue_reflect(id);
}

void LAT8266ClassConstructor(void) {
	Serial.begin(LAT_BAUD);
}

LAT8266Class::LAT8266Class()
{
	strcpy(mdnsService, "http");

	MAC = WiFi.macAddress();
	MAC.replace(":", "");
	MDNSName = MDNS_DEFAULTNAMEPRE + MAC;
	WebServer.init(this, &_queuer, &_resultr);
}

String LAT8266Class::WiFiStatusStr(void)
{
	switch (WiFi.status())
	{
	case WL_CONNECTED:
		return "STATION_GOT_IP";
	case WL_NO_SSID_AVAIL:
		return "STATION_NO_AP_FOUND";
	case WL_CONNECT_FAILED:
		return "STATION_CONNECT_FAIL";
	case WL_WRONG_PASSWORD:
		return "STATION_WRONG_PASSWORD";
	case WL_IDLE_STATUS:
		return "STATION_IDLE";
	default:
		return "STATION_DISCONNECTED";
	}
}

bool LAT8266Class::connect(unsigned long timeout, bool silent, bool ap, bool hidden, int stations)
{

	unsigned long otim = timeout;
	timeout += millis();
	WiFi.mode(WIFI_AP_STA);

	if (ap)
	{
		if (!connectAP(hidden, stations, true))
		{
			addln("ERROR CAP");
			return false;
		}
	}

	if (!disconnect(otim))
	{
		rqueue_errored = true;
		addln("ERROR TIMEDOUT");
		return false;
	}
	if (WIFI_SSID == "")
	{
		rqueue_errored = true;
		addln("ERROR SSID");
		return false;
	}
	if (WIFI_PASSWD != "")
		WiFi.begin(WIFI_SSID, WIFI_PASSWD);
	else
		WiFi.begin(WIFI_SSID);
	if (!silent)
		toSay += ("Connecting");
	while (WiFi.status() != WL_CONNECTED && millis() < timeout)
	{
		delay(500);
		if (!silent)
			toSay += (".");
	}
	if (millis() >= timeout)
	{
		if (!silent)
			addln();
		rqueue_errored = true;
		addln("ERROR TIMEDOUT");
		return false;
	}

	if (!silent)
	{
		toSay += ("\nConnected, IP address: ");
		toSay += (WiFi.localIP().toString());
		toSay += ("(");
		toSay += (WiFi.RSSI());
		addln(")");
		toSay += ("MDNS Name: ");
		addln(MDNSName);
	}
	if (!MDNS.begin(MDNSName) && MDNSName != (MDNS_DEFAULTNAMEPRE + MAC))
	{
		MDNSName = MDNS_DEFAULTNAMEPRE + MAC;
		MDNS.begin(MDNSName);
	}

	return true;
}

bool LAT8266Class::connectAP(bool hidden, int stations, bool silent)
{
	if (AP_SSID == "")
	{
		rqueue_errored = true;
		if (!silent)
			addln("ERROR SSID");
		AP_CONN = false;
		return false;
	}
	bool r = WiFi.softAP(AP_SSID, AP_PASSWD, 1, hidden ? 1 : 0, stations > 8 ? 8 : stations < 1 ? 1 : stations);
	AP_CONN = r;
	if (silent)
		return r;
	if (r)
	{
		addln("OK");
	}
	else
	{
		rqueue_errored = true;
		addln("ERROR CAP");
	}
	return r;
}

bool LAT8266Class::disconnect(unsigned long timeout)
{
	timeout += millis();
	while (WiFi.status() == WL_CONNECTED && millis() < timeout)
	{
		WiFi.disconnect();
		delay(100);
	}
	if (millis() >= timeout)
		return false;
	return true;
}

bool LAT8266Class::disconnectAP(void)
{
	if(AP_CONN)
		return !(AP_CONN = !WiFi.softAPdisconnect());
	return true;
}

void LAT8266Class::scan(unsigned long timeout)
{
	unsigned long otim = timeout;
	timeout += millis();
	int isConnected = WiFi.isConnected();

	if (!disconnect(otim))
	{
		rqueue_errored = true;
		addln("ERROR TIMEDOUT");
		return;
	}

	if (WiFi.scanComplete() != -1)
		WiFi.scanNetworks(true);
	int n = -1;
	while ((n = WiFi.scanComplete()) < 0 && millis() < timeout)
	{
		delay(100);
	}
	if (isConnected)
	{
		if (millis() < timeout)
		{
			if (connect(timeout - millis(), true) && n > 0)
			{
				addln(n);
			}
		}
		else
		{
			rqueue_errored = true;
			addln("ERROR TIMEDOUT_NOCONN");
		}
	}
}

void LAT8266Class::printLastScan()
{
	int n = WiFi.scanComplete();
	if (n == -2)
	{
		rqueue_errored = true;
		addln("ERROR NOSCAN");
	}
	else if (n == -1)
	{
		rqueue_errored = true;
		addln("ERROR SCANNING");
	}
	else
	{
		for (int i = 0; i < n; i++)
		{
			addln(String(String(i) + ": " + WiFi.SSID(i) + ", Ch:" + WiFi.channel(i) + " (" + WiFi.RSSI(i) + "dBm) " + (WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "")));
		}
	}
}

void LAT8266Class::cmdwifiSSID(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		WIFI_SSID = String(pt);
		addln("OK");
		break;
	case MODE_GET:
		addln(WIFI_SSID);
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}
void LAT8266Class::cmdapSSID(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		AP_SSID = String(pt);
		addln("OK");
		break;
	case MODE_GET:
		addln(AP_SSID);
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}

void LAT8266Class::LAT8266_HIDE_WIFI_PASSWD()
{
	LAT8266_HIDE_WIFI_PASSWD_ = true;
}
void LAT8266Class::LAT8266_HIDE_AP_PASSWD()
{
	LAT8266_HIDE_AP_PASSWD_ = true;
}
void LAT8266Class::LAT8266_WEB_INTERFACE(bool tf)
{
	LAT8266_WEB_INTERFACE_ = tf;
}
bool LAT8266Class::LAT8266_WEB_INTERFACE()
{
	return LAT8266_WEB_INTERFACE_;
}
void LAT8266Class::cmdwifiPASSWD(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		WIFI_PASSWD = String(pt);
		addln("OK");
		break;
	case MODE_GET:
		if (LAT8266_HIDE_WIFI_PASSWD_)
		{
			rqueue_errored = true;
			addln("ERROR ACCESS");
		}
		else
			addln(WIFI_PASSWD);
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}
void LAT8266Class::cmdapPASSWD(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		if(String(pt).length()<8) {
			rqueue_errored = true;
			addln("ERROR PASSZ");
			break;
		}
		AP_PASSWD = String(pt);
		addln("OK");
		break;
	case MODE_GET:
		if (LAT8266_HIDE_AP_PASSWD_)
		{
			rqueue_errored = true;
			addln("ERROR ACCESS");
		}
		else
			addln(AP_PASSWD);
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}

void LAT8266Class::cmdwifiRSSI()
{
	addln(WiFi.RSSI());
}

void LAT8266Class::cmdwifiCONN(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		if (connect(strtoul(pt, NULL, 0), true))
			addln("OK");
		break;
	case MODE_GET:
		addln(WiFi.isConnected());
		break;
	default:
		if (connect(WIFI_CONNDEFAULTTIME, true))
			addln("OK");
		break;
	}
}
void LAT8266Class::cmdapCONN(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
			connectAP(AP_HIDDEN, strtol(pt, NULL, 0));
		break;
	case MODE_GET:
		addln(AP_CONN);
		break;
	default:
		connectAP(AP_HIDDEN);
		break;
	}
}
void LAT8266Class::cmdapHide(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		capitalizeStr(pt);
		if (!strcmp(pt, "TRUE"))
			AP_HIDDEN = true;
		else if (!strcmp(pt, "FALSE"))
			AP_HIDDEN = false;
		else
		{
			rqueue_errored = true;
			addln("ERROR VALUE");
			break;
		}
		addln("OK");
		break;
	case MODE_GET:
		addln(AP_HIDDEN);
		break;
	default:
		AP_HIDDEN = !AP_HIDDEN;
		addln("OK");
		break;
	}
}

void LAT8266Class::cmdwifiIP()
{
	addln(WiFi.localIP().toString());
}
void LAT8266Class::cmdapIP()
{
	addln(WiFi.softAPIP().toString());
}

void LAT8266Class::cmdwifiCHNL()
{
	addln((uint8_t)WiFi.channel());
}
void LAT8266Class::cmdapStationNum()
{
	addln((uint8_t)WiFi.softAPgetStationNum());
}

void LAT8266Class::cmdwifiMAC()
{
	addln(WiFi.macAddress());
}
void LAT8266Class::cmdapMAC()
{
	addln(WiFi.softAPmacAddress());
}

void LAT8266Class::cmdwifiNAME(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		WiFi.hostname(String(pt));
		break;
	case MODE_GET:
		addln(WiFi.hostname());
		break;
	default:
		addln(WiFi.hostname());
		break;
	}
}

void LAT8266Class::cmdwifiAUTO(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		capitalizeStr(pt);
		if (!strcmp(pt, "TRUE"))
			WiFi.setAutoReconnect(true);
		else if (!strcmp(pt, "FALSE"))
			WiFi.setAutoReconnect(false);
		else
		{
			rqueue_errored = true;
			addln("ERROR VALUE");
			break;
		}
		addln("OK");
		break;
	case MODE_GET:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}

void LAT8266Class::cmdwifiAUTOSTART(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		capitalizeStr(pt);
		if (!strcmp(pt, "TRUE"))
			WiFi.setAutoConnect(true);
		else if (!strcmp(pt, "FALSE"))
			WiFi.setAutoConnect(false);
		else
		{
			rqueue_errored = true;
			addln("ERROR VALUE");
			break;
		}
		addln("OK");
		break;
	case MODE_GET:
		addln(WiFi.getAutoConnect());
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}

void LAT8266Class::cmdwifiSCAN(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		scan(strtoul(pt, NULL, 0));
		break;
	case MODE_GET:
		printLastScan();
		break;
	default:
		scan(WIFI_SCANDEFAULTTIME);
		break;
	}
}

int LAT8266Class::queue_reflect(String in)
{
	if (rqueue[(rqueue_ptr + 1) % REFLECT_QUEUE_SIZE].arg != "" && rqueue[(rqueue_ptr + 1) % REFLECT_QUEUE_SIZE].millis > millis())
		return -1;
	rqueue_ptr = (rqueue_ptr + 1) % REFLECT_QUEUE_SIZE;
	rqueue[rqueue_ptr] = {in, 0, ""};
	if (noreturn(in))
		return -2;
	return rqueue_ptr;
}

bool LAT8266Class::noreturn(String in)
{
	if (in == "reboot")
		return true;
	else
		return false;
}

String LAT8266Class::queue_reflect(int id)
{
	if (id < 0 || id >= REFLECT_QUEUE_SIZE || rqueue[id].arg == "")
		return "inc";
	if (rqueue[id].millis == 0)
		return "wait";
	if (rqueue[id].millis < millis())
		return "inc";
	rqueue[id].arg = "";
	return rqueue[id].result;
}
void LAT8266Class::queue_reflect()
{
	if (rqueue_time > millis())
		return;
	if (rqueue_ptr == rqueued_ptr && REFLECT_QUEUE_SIZE > 1)
		return;
	rqueue_time = millis() + 100;

	rqueued_ptr = (rqueued_ptr + 1) % REFLECT_QUEUE_SIZE;
	reflect(rqueue[rqueued_ptr].arg, true);
	rqueue[rqueued_ptr].result = String(toSay.c_str()); // copy
	rqueue[rqueued_ptr].millis = millis() + REFLECT_QUEUE_TIMEOUT;
}

void LAT8266Class::addln()
{
	toSay += "\n";
}
void LAT8266Class::addln(int n)
{
	toSay += String(n) + "\n";
}
void LAT8266Class::addln(String str)
{
	toSay += str + "\n";
}

void LAT8266Class::reflect(String in, bool silent)
{
	char input2[maxInput + 2];
	memcpy(input2, input, maxInput + 2);
	in.toCharArray(input, maxInput + 3);
	processCmd(silent);
	memcpy(input, input2, maxInput + 2);
}

void LAT8266Class::loop_cmd()
{
	/*
	 * Wait for command
	 */
	MDNS.update();
	WebServer.run(LAT8266_WEB_INTERFACE_);
	queue_reflect();
	if (!Serial.available())
		return;
	input[currentPt] = Serial.read();
	currentPt++;
	if (currentPt >= 2 && input[currentPt - 2] == '\r' && input[currentPt - 1 == '\n'])
	{
		processCmd();
		currentPt = maxInput + 2;
	}
	if (currentPt >= maxInput + 2)
	{
		currentPt = 0;
	}
}

void LAT8266Class::processCmd(bool silent)
{
	toSay = "";
	rqueue_errored = false;
	input[currentPt - 2] = '\0';

	normalizeStr(input);
	int num = separateStr(input, ';');
	char *pt = input;
	currentMode = MODE_CMD;

	for (int i = 0; i < num; i++)
	{
		currentMode = getNextMode(pt);
		processArg(pt);
		while (*pt != '\0')
			pt++;
		pt++;

		// Adjust pt after mode
		if (currentMode == MODE_SET)
		{
			while (*pt != '\0')
				pt++;
			pt++;
		}
		else if (currentMode == MODE_GET)
		{
			pt++;
		}
	}
	if (!silent)
		Serial.print(toSay);
	else
	{
		if (rqueue_errored)
			toSay = "<<ERROR>>\n" + toSay;
		else
			toSay = "<<SUCCESS>>\n" + toSay;
	}
}

void LAT8266Class::processArg(char *src)
{
	capitalizeStr(src);
	if (!strcmp(src, "LAT"))
	{ // TEST
		addln("OK");
	}
	else if (!strcmp(src, "VER"))
	{ // VERSION
		addln(LAT_VERSION);
	}
	else if (!strcmp(src, "HELP"))
	{ // HELP
		addln(LAT_HELP + (WiFi.isConnected() ? String(" or here http://" + MDNSName + ":82/") : ""));
	}
	else if (!strcmp(src, "HTREQUEST"))
	{ // HTTP Request
		httpRequest();
	}
	else if (!strcmp(src, "HTTIMEOUT"))
	{ // HTTP Timeout
		cmdhttpTimeout(src);
	}
	else if (!strcmp(src, "HTDEFAULT"))
	{ // HTTP Default Request
		httpUsingDefault();
	}
	else if (!strcmp(src, "HTHOST"))
	{ // HTTP Host
		cmdhttpHost(src);
	}
	else if (!strcmp(src, "HTPATH"))
	{ // HTTP Path
		cmdhttpPath(src);
	}
	else if (!strcmp(src, "HTTYPE"))
	{ // HTTP Type
		cmdhttpType(src);
	}
	else if (!strcmp(src, "HTPORT"))
	{ // HTTP Port
		cmdhttpPort(src);
	}
	else if (!strcmp(src, "HTHEADER"))
	{ // HTTP Header
		cmdhttpHeader(src);
	}
	else if (!strcmp(src, "HTBODY"))
	{ // HTTP Body
		cmdhttpBody(src);
	}
	else if (!strcmp(src, "HTHEADERIN"))
	{ // HTTP HeaderInput
		printHeaderInput();
	}
	else if (!strcmp(src, "HTBODYIN"))
	{ // HTTP BodyInput
		printBodyInput();
	}
	else if (!strcmp(src, "HTCODE"))
	{ // HTTP Request Code
		cmdhttpCode();
	}
	else if (!strcmp(src, "HTBUFFER"))
	{ // HTTP Toggle Buffer
		cmdhttpToggle(src);
	}
	else if (!strcmp(src, "WIFISSID"))
	{ // WIFI SSID
		cmdwifiSSID(src);
	}
	else if (!strcmp(src, "WIFIPASSWD"))
	{ // WIFI PASSWD
		cmdwifiPASSWD(src);
	}
	else if (!strcmp(src, "WIFIRSSI"))
	{ // WIFI RSSI
		cmdwifiRSSI();
	}
	else if (!strcmp(src, "WIFICONN"))
	{ // WIFI Connect + timeout
		cmdwifiCONN(src);
	}
	else if (!strcmp(src, "WIFIIP"))
	{ // WIFI IP
		cmdwifiIP();
	}
	else if (!strcmp(src, "WIFICHNL"))
	{ // WIFI Channel
		cmdwifiCHNL();
	}
	else if (!strcmp(src, "WIFIMAC"))
	{ // WIFI MacAddress
		cmdwifiMAC();
	}
	else if (!strcmp(src, "WIFINAME"))
	{ // WIFI Name
		cmdwifiNAME(src);
	}
	else if (!strcmp(src, "WIFIAUTO"))
	{ // WIFI AutoReconnect
		cmdwifiAUTO(src);
	}
	else if (!strcmp(src, "WIFIAUTOSTART"))
	{ // WIFI AutoConnect
		cmdwifiAUTOSTART(src);
	}
	else if (!strcmp(src, "WIFISCAN"))
	{ // WIFI SCAN
		cmdwifiSCAN(src);
	}
	else if (!strcmp(src, "WIFIDISC"))
	{ // WIFI Disconnect
		if(disconnect())
			addln("OK");
		else {
			rqueue_errored = true;
			addln("ERROR DISC");
		}
	}
	else if (!strcmp(src, "APCONN"))
	{ // AP Connect + station number
		cmdapCONN(src);
	}
	else if (!strcmp(src, "APDISC"))
	{ // AP Disconnect
		if(disconnectAP())
			addln("OK");
		else {
			rqueue_errored = true;
			addln("ERROR DISC");
		}
	}
	else if (!strcmp(src, "APSSID"))
	{ // AP SSID
		cmdapSSID(src);
	}
	else if (!strcmp(src, "APPASSWD"))
	{ // AP PASSWD
		cmdapPASSWD(src);
	}
	else if (!strcmp(src, "APIP"))
	{ // AP IP
		cmdapIP();
	}
	else if (!strcmp(src, "APSTATION"))
	{ // AP Station
		cmdapStationNum();
	}
	else if (!strcmp(src, "APMAC"))
	{ // AP MAC
		cmdapMAC();
	}
	else if (!strcmp(src, "APHIDE"))
	{ // AP Hidden Network
		cmdapHide(src);
	}
	else if (!strcmp(src, "MDNSSERVICE"))
	{ // MDNS Service
		cmdmdnsService(src);
	}
	else if (!strcmp(src, "MDNSQNAME"))
	{ // MDNS Query Name
		cmdmdnsQueryName(src);
	}
	else if (!strcmp(src, "MDNSNAME"))
	{ // MDNS Host Name
		addln(MDNSName);
	}
	else if (!strcmp(src, "REBOOT"))
	{ // reboot
		delay(200);
		ESP.reset();
	}

	else
	{
		rqueue_errored = true;
		toSay += ("ERROR ");
		addln(src);
		addln(LAT_HELP + (WiFi.isConnected() ? String(" or here http://" + MDNSName + ":82/") : ""));
	}
}

void LAT8266Class::httpUsingDefault()
{
	if (HTTP_host == "" || HTTP_path == "")
	{
		rqueue_errored = true;
		addln("ERROR INCOMPLETE");
		return;
	}
	/**/
	if (!httpHeaderInput.set(HTTP_type + " " + HTTP_path + " HTTP/1.1\r\n" +
							 "Host: " + HTTP_host + "\r\n" +
							 "Connection: close\r\n" +
							 "\r\n"))
		return;
	if (HTTP_type == "GET" || HTTP_type == "HEAD" || HTTP_type == "DELETE")
	{
		if (!httpBodyInput.clear())
		{
			rqueue_errored = true;
			addln("ERROR BODYFOUND");
			return;
		}
	}
	httpRequest();
}

void LAT8266Class::httpRequest()
{

	bool errored = false;
	if (toggleBuffer)
	{
		if (!httpHeader.clear())
			errored = true;
		if (!httpBody.clear())
			errored = true;
	}

	if (WiFi.status() == WL_CONNECTED)
	{
		unsigned long endtimeout = glb_RequestTimeout ? millis() + glb_RequestTimeout : 0;

		if (HTTP_host.endsWith(".local"))
		{
			int n = mdnsQueryName(HTTP_host.c_str(), glb_RequestTimeout);
			if (n == -1)
			{
				rqueue_errored = true;
				addln("ERROR MDNSQ");
				return;
			}
			HTTP_Port = MDNS.port(n);
			HTTP_host = toIP(MDNS.IP(n));
		}

		if (client.connect(HTTP_host, HTTP_Port))
		{
			String req = httpHeaderInput.toString().substring(0, httpHeaderInput.getSize() - 2);
			req += (httpBodyInput.getSize() == 0 ? "" : "Content-length: " + String(httpBodyInput.getSize()) + "\r\n") + "\r\n";
			req += httpBodyInput.toString();
			client.print(req);

			bool writeBody = false;
			while (!errored && (client.connected() || client.available()) && (glb_RequestTimeout == 0 || millis() < endtimeout))
			{
				if (client.available())
				{
					endtimeout = glb_RequestTimeout ? millis() + glb_RequestTimeout : 0;
					String line = client.readStringUntil('\n') + "\n";
					if (toggleBuffer)
					{
						if (line != "\r\n")
							if (writeBody)
							{
								if (!httpBody.addLine(line))
								{
									errored = true;
									break;
								}
							}
							else
							{
								if (!httpHeader.addLine(line))
								{
									errored = true;
									break;
								}
							}
						else
							writeBody = true;
					}
					else
						toSay += (line);
				}
			}
			if (glb_RequestTimeout != 0 && httpHeader.getSize() == 0 && millis() >= endtimeout)
			{
				rqueue_errored = true;
				addln("ERROR TIMEDOUT");
			}
			else if (httpHeader.getSize() == 0 || errored || !WiFi.isConnected())
			{
				rqueue_errored = true;
				addln("ERROR CANCELLED");
			}
			else if (toggleBuffer)
				addln("OK");
			client.stop();
		}
		else
		{
			rqueue_errored = true;
			addln("ERROR REQUEST");
		}
	}
	else
	{
		rqueue_errored = true;
		addln("ERROR NOCONN");
	}
}

void LAT8266Class::cmdhttpTimeout(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		glb_RequestTimeout = strtoul(pt, NULL, 0);
		addln("OK");
		break;
	case MODE_GET:
		addln(glb_RequestTimeout);
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}

bool LAT8266Class::readHeader()
{
	return readHTTPContent(&httpHeaderInput);
}

bool LAT8266Class::readBody()
{
	return readHTTPContent(&httpBodyInput);
}

bool LAT8266Class::setHeader(String in)
{
	return httpHeaderInput.set(in);
}

bool LAT8266Class::setBody(String in)
{
	return httpBodyInput.set(in);
}

bool LAT8266Class::readHTTPContent(HTTPContent *c)
{
	Serial.println("READ");

	bool reading = true;
	String line = "";
	c->clear();

	while (reading)
	{
		/*
		 * Wait for input
		 */
		unsigned long timeout = millis() + HTTP_READDEFAULTTIME;
		while (!Serial.available())
		{
			if (millis() >= timeout)
			{
				rqueue_errored = true;
				addln("ERROR TIMEDOUT");
				return false;
			}
		}
		line += String((char)Serial.read());
		if (line.endsWith("\r\n"))
		{
			if (!c->addLine(line))
			{
				return false;
			}
			if (line == "\r\n")
				break;
			line = "";
		}
	}
	addln("OK");
	return true;
}

// command interaction
void LAT8266Class::cmdhttpHost(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		HTTP_host = String(pt);
		addln("OK");
		break;
	case MODE_GET:
		addln(HTTP_host);
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}

void LAT8266Class::cmdhttpPath(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		HTTP_path = String(pt);
		addln("OK");
		break;
	case MODE_GET:
		addln(HTTP_path);
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}

void LAT8266Class::cmdhttpType(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		HTTP_type = String(pt);
		HTTP_type.toUpperCase();
		addln("OK");
		break;
	case MODE_GET:
		addln(HTTP_type);
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}
void LAT8266Class::cmdhttpPort(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		HTTP_Port = atoi(pt);
		addln("OK");
		break;
	case MODE_GET:
		addln(HTTP_Port);
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}

void LAT8266Class::cmdhttpHeader(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		readHeader();
		break;
	case MODE_GET:
		printHeader();
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}

void LAT8266Class::cmdhttpBody(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		readBody();
		break;
	case MODE_GET:
		printBody();
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}

void LAT8266Class::cmdhttpCode()
{
	addln(getHttpCode());
}

void LAT8266Class::cmdhttpToggle(char *pt)
{
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		capitalizeStr(pt);
		if (!strcmp(pt, "TRUE"))
			toggleBuffer = true;
		else if (!strcmp(pt, "FALSE"))
			toggleBuffer = false;
		else
		{
			rqueue_errored = true;
			addln("ERROR VALUE");
			break;
		}
		addln("OK");
		break;
	case MODE_GET:
		addln(toggleBuffer);
		break;
	default:
		toggleBuffer = !toggleBuffer;
		addln("OK");
		break;
	}
}

int LAT8266Class::mdnsQueryName(const char *name_, unsigned long timeout)
{

	char *name = (char *)malloc(strlen(name_));
	if (!name)
	{
		rqueue_errored = true;
		addln("ERROR ALLOCQ");
		return -1;
	}
	strcpy(name, name_);
	bool lset = false;
	int l = strlen(name);
	if (!strcmp(name + (l - 6), ".local"))
	{
		name[l - 6] = '\0';
	}

	timeout += millis();
	int res = -1;

	if (!MDNS.begin(MDNSName))
	{
		rqueue_errored = true;
		addln("ERROR MDNSBEGIN");
		return -1;
	}

	while (millis() < timeout && res == -1)
	{
		// try tcp
		int n = MDNS.queryService(mdnsService, "tcp");
		if (n == 0)
			n = MDNS.queryService(mdnsService, "udp");
		if (n != 0)
		{
			for (int i = 0; i < n; i++)
			{
				if (!strcmp(name, MDNS.hostname(i).substring(0, MDNS.hostname(i).length() - 6).c_str()))
				{
					res = i;
					break;
				}
			}
		}
	}

	return res;
}

void LAT8266Class::cmdmdnsService(char *pt)
{
	char *tmp;
	switch (currentMode)
	{
	case MODE_SET:
		while (*(pt++) != '\0')
			;
		tmp = (char *)realloc(mdnsService, strlen(pt));
		if (!tmp)
		{
			rqueue_errored = true;
			addln("ERROR ALLOCM");
		}
		else
		{
			mdnsService = tmp;
			strcpy(mdnsService, pt);
			addln("OK");
		}
		break;
	case MODE_GET:
		addln(mdnsService);
		break;
	default:
		rqueue_errored = true;
		addln("ERROR NOCMD");
		break;
	}
}

String LAT8266Class::toIP(unsigned long lip)
{
	int a = lip % 256;
	lip = lip >> 8;
	int b = lip % 256;
	lip = lip >> 8;
	int c = lip % 256;
	lip = lip >> 8;
	int d = lip % 256;
	return String(a) + "." + String(b) + "." + String(c) + "." + String(d);
}

void LAT8266Class::cmdmdnsQueryName(char *pt)
{
	if (currentMode != MODE_SET)
	{
		rqueue_errored = true;
		addln("ERROR NOCMD");
		return;
	}
	while (*(pt++) != '\0')
		;

	int n = mdnsQueryName(pt);
	if (n == -1)
	{
		rqueue_errored = true;
		addln("ERROR MDNSQ");
		return;
	}
	addln(toIP(MDNS.IP(n)) + ":" + MDNS.port(n));
}

void LAT8266Class::printHeader()
{
	addln(httpHeader.toString());
}

void LAT8266Class::printBody()
{
	addln(httpBody.toString());
}

void LAT8266Class::printHeaderInput()
{
	addln(httpHeaderInput.toString());
}

void LAT8266Class::printBodyInput()
{
	addln(httpBodyInput.toString());
}

int LAT8266Class::getHttpCode()
{
	String hd = httpHeader.toString();
	if (hd.substring(0, 4) != "HTTP")
		return -1;
	int index1 = hd.indexOf(' ');
	if (index1 < 1)
		return -2;
	int index2 = hd.indexOf(' ', index1 + 1);
	if (index2 < 1)
		return -3;
	return hd.substring(index1, index2).toInt();
}

/*
 * CMD - 0
 * SET - 1
 * GET - 2
 */
cmdmodes_t LAT8266Class::getNextMode(char *src)
{
	while (*src != '\0')
	{
		if (*src == '=')
		{
			*src = '\0';
			return MODE_SET;
		}
		else if (*src == '?')
		{
			*src = '\0';
			return MODE_GET;
		}
		src++;
	}
	return MODE_CMD;
}

int LAT8266Class::separateStr(char *src, char sep)
{
	int i = 1, l = 0;
	while (*src != '\0')
	{
		if (*src == sep && (l < 1 || *(src - 1) != '\\'))
		{
			*src = '\0';
			i++;
		}
		src++;
		l++;
	}
	return i;
}
void LAT8266Class::capitalizeStr(char *src)
{
	while (*src != '\0')
	{
		if (*src > 96 && *src < 123)
			*src &= ~(1 << 5);
		src++;
	}
}
void LAT8266Class::normalizeStr(char *src)
{
	int l = 0;
	while (*src != '\0')
	{
		if (*src == ' ' && (l < 1 || *(src - 1) != '\\'))
			strcpy(src, src + 1);
		else if (*src == ' ')
			strcpy(src - 1, src);
		src++;
		l++;
	}
}

LAT8266Class LAT;
