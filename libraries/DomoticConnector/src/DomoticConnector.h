#ifndef _DomoticConnector_h_
#define _DomoticConnector_h_

#include "Arduino.h"
#ifdef ARDUINO_AVR_UNO_WIFI_REV2
	#include <WiFiNINA.h>
#elif defined(ARDUINO_ESP8266_NODEMCU_ESP12E)
	#include <ESP8266WiFi.h>
#else
	#error "This library was made for Lolin NodeMCU v3 or Arduino UNO Wifi Rev2"
#endif
#include <PubSubClient.h>

#include <string.h>

#include <SPI.h>
#include <SD.h>

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
	
	static void setup(bool debug_mode, const char *ssid, const char *password, byte sd_pin, char *file_name);
	
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
	
	static bool getDataFromSD(byte sd_pin, char *file_name, String *ssid, String *password);
	static void conditionalPrintln(const char *str);
	static void conditionalPrintln(String str);
	
	char *generateID(const char *group);
	bool checkConnection();
};

extern DomoticConnector Connector; // to use static methods without '::'

#endif
