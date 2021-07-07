#ifndef _DomoticConnector_h_
#define _DomoticConnector_h_

#include "Arduino.h"
//#include <WiFiNINA.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <string.h>

#include <SPI.h>
#include <SD.h>

#define SD_PIN D8

// subscription types
#define NO_SUBSCRIPTION 	(uint8_t)0
#define ID_SUBSCRIPTION 	(uint8_t)1
#define GROUP_SUBSCRIPTION 	(uint8_t)2

class DomoticConnector {
  public:
	DomoticConnector(void) = default;
    DomoticConnector(const char *ip, uint16_t port, const char *group, uint8_t subscription, MQTT_CALLBACK_SIGNATURE);
    DomoticConnector(const char *ip, uint16_t port, const char *group) : DomoticConnector(ip, port, group, NO_SUBSCRIPTION, NULL) {}
	~DomoticConnector(void);
	
	static void setup(bool debug_mode, const char *ssid, const char *password, char *file_name);
	
	bool loop(void);
	char *getID();
	String getStringID();
	void publish(const char *group, const char *msg);
	void publish(const char *group, String msg);
	
  private:
	static bool _debug_mode;
	
	uint8_t _subscription;
    char *_id;
	char *_group;
	WiFiClient *_espClient;
	PubSubClient *_client;
	
	static bool getDataFromSD(char *file_name, String *ssid, String *password);
	static void conditionalPrintln(const char *str);
	static void conditionalPrintln(String str);
	
	char *generateID(const char *group);
	bool checkConnection();
};

extern DomoticConnector Connector; // to use static methods without '::'

#endif
