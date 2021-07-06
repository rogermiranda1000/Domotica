#ifndef _DomoticConnector_h_
#define _DomoticConnector_h_

#include "Arduino.h"
//#include <WiFiNINA.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SD.h>
#include <string.h>

#define SD_PIN D8

class DomoticConnector {
  public:
    DomoticConnector(char *ip, short port, char *group, char *file_name);
    DomoticConnector(char *ip, short port, char *group) : DomoticConnector(ip, port, group, NULL) {}
	~DomoticConnector(void);
	
  private:
    char *_id;
	char *_group;
	WiFiClient _espClient;
	PubSubClient _client;
	
	char *getID(char *group);
	bool getDataFromSD(String *ssid, String *password);
};

#endif
