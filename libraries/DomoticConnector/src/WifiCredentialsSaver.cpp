#include "WifiCredentialsSaver.h"

uint16_t WifiCredentialsSaver::_passwordIndex;
char WifiCredentialsSaver::_saved_ssid[CREDENTIALS_LENGHT] = {CREDENTIALS_DEFAULT};
char WifiCredentialsSaver::_saved_pass[CREDENTIALS_LENGHT] = {CREDENTIALS_DEFAULT};

void WifiCredentialsSaver::setup(void) {
	#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
	EEPROM.begin(EEPROM_LENGHT);
	#endif
}

bool WifiCredentialsSaver::safeSetSSID(const char *ssid) {
	bool retorno = WifiCredentialsSaver::setSSID(ssid);
	if (retorno && WifiCredentialsSaver::_saved_pass[0] != CREDENTIALS_DEFAULT) retorno = WifiCredentialsSaver::setPassword((const char*)WifiCredentialsSaver::_saved_pass);
	return retorno;
}

bool WifiCredentialsSaver::safeSetPassword(const char *pass) {
	if (WifiCredentialsSaver::_saved_ssid[0] == CREDENTIALS_DEFAULT) {
		strncpy(WifiCredentialsSaver::_saved_pass, pass, CREDENTIALS_LENGHT);
		return false;
	}
	
	bool retorno = WifiCredentialsSaver::setSSID((const char*)WifiCredentialsSaver::_saved_ssid);
	if (retorno) retorno = WifiCredentialsSaver::setPassword(pass);
	return retorno;
}

bool WifiCredentialsSaver::setSSID(const char *ssid) {
	char last;
	uint16_t n = 0;
	byte checksum = 0;
	
	if (EEPROM_LENGHT < 2) return false;
	
	do {
		if (n == EEPROM_LENGHT-1) return false; // we need to save the checksum
		last = ssid[n];
		EEPROM.write(n, last);
		WifiCredentialsSaver::_saved_ssid[n] = last;
		checksum ^= (byte)last;
		n++;
	} while (last != '\0');
	
	EEPROM.write(n, (byte)~checksum);
	
	#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
	EEPROM.commit();
	#endif
	
	WifiCredentialsSaver::_passwordIndex = n+1; // n for checksum; n+1 for password
	
	return true;
}

bool WifiCredentialsSaver::setPassword(const char *pass) {
	char last;
	uint16_t n = WifiCredentialsSaver::_passwordIndex, aux = 0;
	byte checksum = 0;
	
	do {
		if (n >= EEPROM_LENGHT-1) return false; // we need to save the checksum
		last = pass[aux];
		EEPROM.write(n, last);
		WifiCredentialsSaver::_saved_pass[aux] = last;
		checksum ^= (byte)last;
		n++;
		aux++;
	} while (last != '\0');
	
	EEPROM.write(n, (byte)~checksum);
	
	#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
	EEPROM.commit();
	#endif
	
	return true;
}

void WifiCredentialsSaver::emptyEEPROM(void) {
	for (uint16_t n = 0; n < EEPROM_LENGHT; n++) EEPROM.write(n, 0x00);
	
	#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
	EEPROM.commit();
	#endif
	
	WifiCredentialsSaver::_saved_ssid[0] = CREDENTIALS_DEFAULT;
	WifiCredentialsSaver::_saved_pass[0] = CREDENTIALS_DEFAULT;
}

bool WifiCredentialsSaver::readSSID(const char **ssid) {
	if (EEPROM_LENGHT < 2) return false;
	
	if (WifiCredentialsSaver::_saved_ssid[0] == CREDENTIALS_DEFAULT) {
		// not in cache, get it
		uint16_t n = 0;
		byte checksum = 0;
		char last;
		
		do {
			if (n == EEPROM_LENGHT-1 || n == CREDENTIALS_LENGHT) return false; // we need to read the checksum
			last = (byte)EEPROM.read(n);
			WifiCredentialsSaver::_saved_ssid[n] = last;
			checksum ^= (byte)last;
			n++;
		} while (last != '\0');
		
		// the next character it's the checksum
		if ((byte)~checksum != (byte)EEPROM.read(n)) return false;
		
		WifiCredentialsSaver::_passwordIndex = n+1; // n es el checksum; n+1 el password
	}
	
	*ssid = (const char*)WifiCredentialsSaver::_saved_ssid;
	return true;
}

bool WifiCredentialsSaver::readPassword(const char **pass) {
	if (WifiCredentialsSaver::_saved_pass[0] == CREDENTIALS_DEFAULT) {
		// not in cache, get it
		uint16_t n = WifiCredentialsSaver::_passwordIndex, aux = 0;
		byte checksum = 0;
		char last;
		
		do {
			if (n >= EEPROM_LENGHT-1 || aux == CREDENTIALS_LENGHT) return false; // we need to read the checksum
			last = (byte)EEPROM.read(n);
			WifiCredentialsSaver::_saved_pass[aux] = last;
			checksum ^= (byte)last;
			n++;
			aux++;
		} while (last != '\0');
		
		// the next character it's the checksum
		if ((byte)~checksum != (byte)EEPROM.read(n)) return false;
	}
	
	*pass = (const char*)WifiCredentialsSaver::_saved_pass;
	return true;
}

bool WifiCredentialsSaver::readCredentials(const char **ssid, const char **pass) {
	// we need to read first the SSID
	return (WifiCredentialsSaver::readSSID(ssid) && WifiCredentialsSaver::readPassword(pass));
}