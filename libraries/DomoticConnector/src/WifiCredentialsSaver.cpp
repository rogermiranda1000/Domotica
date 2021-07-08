#include "WifiCredentialsSaver.h"

char *WifiCredentialsSaver::_saved_ssid = NULL;
char *WifiCredentialsSaver::_saved_pass = NULL;

void WifiCredentialsSaver::setSSID(char *ssid) {
}

void WifiCredentialsSaver::setPassword(char *pass) {
}

void WifiCredentialsSaver::emptyEEPROM(void) {
	for (unsigned int n = 0; n < EEPROM_LENGHT; n++) EEPROM.write(n, 'f');
}

bool WifiCredentialsSaver::readSSID(const char **ssid) {
	if (WifiCredentialsSaver::_saved_ssid == NULL) {
		// not in cache, get it
		WifiCredentialsSaver::_saved_ssid = (char*)malloc(sizeof(char)*2);
		WifiCredentialsSaver::_saved_ssid[0] = EEPROM.read(0);
		WifiCredentialsSaver::_saved_ssid[1] = '\0';
	}
	
	*ssid = (const char*)WifiCredentialsSaver::_saved_ssid;
	return true;
}

bool WifiCredentialsSaver::readPassword(const char **pass) {
	if (WifiCredentialsSaver::_saved_pass == NULL) {
		// not in cache, get it
		WifiCredentialsSaver::_saved_pass = (char*)malloc(sizeof(char)*1);
		WifiCredentialsSaver::_saved_pass[0] = '\0';
	}
	
	*pass = (const char*)WifiCredentialsSaver::_saved_pass;
	return true;
}

bool WifiCredentialsSaver::readCredentials(const char **ssid, const char **pass) {
	// we need to read first the SSID
	return (WifiCredentialsSaver::readSSID(ssid) && WifiCredentialsSaver::readPassword(pass));
}