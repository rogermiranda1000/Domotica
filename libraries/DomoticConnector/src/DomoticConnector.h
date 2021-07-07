#ifndef _DomoticConnector_h_
#define _DomoticConnector_h_

#include "Arduino.h"
//#include <WiFiNINA.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SD.h>
#include <string.h>

#define SD_PIN D8

// subscription types
#define NO_SUBSCRIPTION 0
#define ID_SUBSCRIPTION 1
#define GROUP_SUBSCRIPTION 2

// big brain; cortesia de https://stackoverflow.com/a/5586469/9178470
#ifdef  DEBUG
  #define DEBUG_PRINT(str) Serial.print(str)
  #define DEBUG_PRINTLN(str) Serial.println(str)
#else
  #define DEBUG_PRINT(str)
  #define DEBUG_PRINTLN(str)
#endif

class DomoticConnector {
  public:
    DomoticConnector(char *ip, short port, char *group, byte subscription, MQTT_CALLBACK_SIGNATURE, char *file_name);
    DomoticConnector(char *ip, short port, char *group, byte subscription, MQTT_CALLBACK_SIGNATURE) : DomoticConnector(ip, port, group, subscription, callback, NULL) {}
    DomoticConnector(char *ip, short port, char *group, char *file_name) : DomoticConnector(ip, port, group, NO_SUBSCRIPTION, NULL, file_name) {}
    DomoticConnector(char *ip, short port, char *group) : DomoticConnector(ip, port, group, NO_SUBSCRIPTION, NULL, NULL) {}
	~DomoticConnector(void);
	
	void setup(void);
	bool loop(void);
	char *getID();
	String getStringID();
	void sendMessage(char *group, char *msg);
	void sendMessage(char *group, String msg);
	
  private:
	byte _subscription;
    char *_id;
	char *_group;
	WiFiClient *_espClient;
	PubSubClient *_client;
	
	// temporales (se destruyen al entrar a setup)
	char *_sd_file_name;
	char *_ip;
	short _port;
	MQTT_CALLBACK_SIGNATURE;
	
	char *generateID(char *group);
	bool getDataFromSD(char *file_name, String *ssid, String *password);
	bool checkConnection();
};

#endif
