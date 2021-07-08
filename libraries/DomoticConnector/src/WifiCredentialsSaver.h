#ifndef _WifiCredentialsSaver_h_
#define _WifiCredentialsSaver_h_

#include "Arduino.h"
#include <EEPROM.h>

#ifdef ARDUINO_AVR_UNO_WIFI_REV2
	#define EEPROM_LENGHT 256
#else
	#define EEPROM_LENGHT 512 // NodeMCU, entre otras muchas
#endif

class WifiCredentialsSaver {
  public:
	static void setSSID(char *ssid);
	static void setPassword(char *pass);
	static void emptyEEPROM(void);

	static bool readCredentials(const char **ssid, const char **pass);
	
  private:
	static char *_saved_ssid;
	static char *_saved_pass;
	
	static bool readSSID(const char **ssid);
	static bool readPassword(const char **pass);
};

extern WifiCredentialsSaver WifiSaver; // to use static methods without '::'

#endif
