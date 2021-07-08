#ifndef _WifiCredentialsSaver_h_
#define _WifiCredentialsSaver_h_

#include "Arduino.h"
#include <EEPROM.h>

#ifdef ARDUINO_AVR_UNO_WIFI_REV2
	#define EEPROM_LENGHT 256
#else
	#define EEPROM_LENGHT 512 // NodeMCU, entre otras muchas
#endif

#define CREDENTIALS_LENGHT 50 // str size
#define CREDENTIALS_DEFAULT (char)-1

class WifiCredentialsSaver {
  public:
	static void setup(void);
  
	/**
	* It sets the password on the EEPROM
	* /!\ If you're not sure if it will be called before setSSID() call safeSetSSID()
	*/
	static bool setSSID(const char *ssid);
	static bool safeSetSSID(const char *ssid);
	
	/**
	* It sets the password on the EEPROM
	* /!\ If you're not sure if it will be called after setSSID() call safeSetPassword()
	*/
	static bool setPassword(const char *pass);
	static bool safeSetPassword(const char *pass);
	
	static void emptyEEPROM(void);

	static bool readCredentials(const char **ssid, const char **pass);
	
  private:
	static uint16_t _passwordIndex;
	static char _saved_ssid[CREDENTIALS_LENGHT];
	static char _saved_pass[CREDENTIALS_LENGHT];
	
	static bool readSSID(const char **ssid);
	static bool readPassword(const char **pass);
};

extern WifiCredentialsSaver WifiSaver; // to use static methods without '::'

#endif
