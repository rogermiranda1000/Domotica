#include "BoardRestarter.h"

bool BoardRestarter::_enabled = false;

void BoardRestarter::enableWatchdogTimer(WDT_PARAM timeout) {
	BoardRestarter::_enabled = true;
	
#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
	ESP.wdtEnable(timeout);
#else
	wdt_enable(timeout);
#endif
}

void BoardRestarter::loop(void) {
#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
	ESP.wdtFeed();
#else
	wdt_reset();
#endif
}