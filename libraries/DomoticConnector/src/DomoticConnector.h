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
#include "WifiCredentialsSaver.h"

// subscription types
#define NO_SUBSCRIPTION 	(uint8_t)0
#define ID_SUBSCRIPTION 	(uint8_t)1
#define GROUP_SUBSCRIPTION 	(uint8_t)2

#define EEPROM_DELETE		"+DEL"
#define EEPROM_SET_SSID		"+SSID:"
#define EEPROM_SET_PASSWORD	"+PASS:"

class DomoticConnector {
  public:
	DomoticConnector(void) = default;
	DomoticConnector(const char *ip, uint16_t port, const char *group, void (*on_reconnect)(void), uint8_t subscription, MQTT_CALLBACK_SIGNATURE);
	DomoticConnector(const char *ip, uint16_t port, const char *group) : DomoticConnector(ip, port, group, NULL, NO_SUBSCRIPTION, NULL) {}
	~DomoticConnector(void);
	
	static void setup(bool debug_mode, const char *ssid, const char *password, byte sd_pin, char *file_name);
	static void setup(bool debug_mode, const char *ssid, const char *password);
	
	/**
	* Llamar siempre que Serial.available()
	* Para cambiar la SSID enviar '+SSID:<nueva SSID>'
	* Para cambiar la contraseña enviar '+PASS:<nueva contraseña>'
	* /!\ Los valores no se leeran hasta el próximo reinicio
	* @param str Retorno de Serial.readString()
	* @return Si el comando ha sido procesado (si era un comando de la libreria; true), o si no (si no es un comando de la libreria y, por lo tanto, es del código externo; false)
	*/
	static bool eepromUpdate(String str);
	
	bool loop(void);
	char *getID();
	String getStringID();
	void publish(const char *group, const char *msg);
	void publish(const char *group, String msg);
	
  private:
	static bool _debug_mode;
	
	void (*_on_reconnect)(void);
	uint8_t _subscription;
	char *_id;
	char *_group;
	WiFiClient *_espClient;
	PubSubClient *_client;
	
	static bool getDataFromSD(byte sd_pin, char *file_name, String *ssid, String *password);
	static void conditionalPrintln(const char *str);
	static void conditionalPrintln(String str);
	static void conditionalPrint(const char *str);
	
	char *generateID(const char *group);
	bool checkConnection();
};

extern DomoticConnector Connector; // to use static methods without '::'

#endif
