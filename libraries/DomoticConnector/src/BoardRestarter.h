#ifndef _BoardRestarter_h_
#define _BoardRestarter_h_

#include "Arduino.h"
#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
	#include <ESP8266WiFi.h>
	#define WDT_PARAM WDTO_t
#elif defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_MKRWIFI1010)
	// tested for ARDUINO_AVR_UNO_WIFI_REV2
	#include <avr/wdt.h>
	#define WDTO_8S   11 // for some reason this include is missing...
	#define WDT_PARAM const uint8_t
#else
	#error "This library was made for Lolin NodeMCU v3, Arduino UNO Wifi Rev2 or Arduino Wifi 1010"
#endif

class BoardRestarter {
  public:
	BoardRestarter(void) = default;
	
	/**
	  * Enable the WDT
	  */
	static void enableWatchdogTimer(WDT_PARAM timeout = WDTO_8S);
	
	/**
	  * Reset the WDT
	  */
	static void loop(void);
	
  private:
	static bool _enabled;
};

extern BoardRestarter WatchdogTimer; // to use static methods without '::'

#endif
