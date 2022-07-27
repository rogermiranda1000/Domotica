#include "DomoticConnector.h"

bool DomoticConnector::_debug_mode;

void DomoticConnector::conditionalPrintln(const char *str) {
	if (DomoticConnector::_debug_mode) Serial.println(str);
}

void DomoticConnector::conditionalPrintln(String str) {
	if (DomoticConnector::_debug_mode) Serial.println(str);
}

void DomoticConnector::conditionalPrint(const char *str) {
	if (DomoticConnector::_debug_mode) Serial.print(str);
}

DomoticConnector::DomoticConnector(const char *ip, uint16_t port, const char *group, void (*on_reconnect)(void), uint8_t subscription, MQTT_CALLBACK_SIGNATURE) {
	this->_on_reconnect = on_reconnect;
	this->_subscription = subscription;

	// copy group identifier
	DomoticConnector::conditionalPrintln("New client on group " + String(group));
	this->_group = NULL;
	this->_group = (char*)malloc(sizeof(char)*(strlen(group)+1));
	if (this->_group != NULL) strcpy(this->_group, group);
	
	this->_id = this->generateID(group);
	DomoticConnector::conditionalPrintln("Using id " + String(this->_id));
	
	this->_espClient = new WiFiClient();
	this->_client = new PubSubClient(*this->_espClient);
	
	this->_client->setServer(ip, port);
	if (callback != NULL) this->_client->setCallback(callback);
}

void DomoticConnector::setup(bool debug_mode, bool enableWDT, const char *ssid, const char *password, byte sd_pin, char *file_name) {
	DomoticConnector::_debug_mode = debug_mode;
	
	WifiSaver.setup();
	
#ifdef ARDUINO_ESP8266_NODEMCU_ESP12E
	WiFi.mode(WIFI_STA);
#endif

	if (enableWDT) WatchdogTimer.enableWatchdogTimer();

	if (ssid != NULL && password != NULL) {
		// credentials hardcoded
		DomoticConnector::conditionalPrint("Wifi over constant: ");
		DomoticConnector::conditionalPrintln(ssid);
		
		WiFi.begin(ssid, password);
	}
	else {
		if (WifiSaver.readCredentials(&ssid, &password)) {
			// credentials in EEPROM
			DomoticConnector::conditionalPrint("Wifi over EEPROM: ");
			DomoticConnector::conditionalPrintln(ssid);
		
			WiFi.begin(ssid, password);
		}
		else {
			DomoticConnector::conditionalPrintln("EEPROM checksum is invalid");
			
			// get credentials from SD
			String sd_ssid, sd_password;
			if (file_name != NULL && DomoticConnector::getDataFromSD(sd_pin, file_name, &sd_ssid,&sd_password)) {
				DomoticConnector::conditionalPrint("Wifi over SD: ");
				DomoticConnector::conditionalPrintln(sd_ssid);
		
				WiFi.begin(sd_ssid.c_str(), sd_password.c_str());
			}
			else DomoticConnector::conditionalPrintln("Unable to get credentials from SD");
		}
	}
}

void DomoticConnector::setup(bool debug_mode, bool enableWDT, const char *ssid, const char *password) {
	DomoticConnector::setup(debug_mode, enableWDT, ssid, password, (byte)-1, NULL);
}

bool DomoticConnector::eepromUpdate(String str) {
	uint8_t n = 0, len = str.length()-1, checkLen = strlen(EEPROM_SET_SSID);
	str.remove(len,1); // remove '\n'
	const char *strPtr = str.c_str();
	while (n < len && n < checkLen) {
		if (strPtr[n] != EEPROM_SET_SSID[n]) break;
		n++;
	}
	if (n == checkLen) {
		// es SSID
		strPtr += n;
		if (WifiSaver.safeSetSSID(strPtr)) {
			DomoticConnector::conditionalPrint("New SSID value: ");
			DomoticConnector::conditionalPrintln(strPtr);
		}
		else DomoticConnector::conditionalPrintln("Error while saving the new SSID");
		return true;
	}
	
	n = 0;
	checkLen = strlen(EEPROM_SET_PASSWORD);
	while (n < len && n < checkLen) {
		if (strPtr[n] != EEPROM_SET_PASSWORD[n]) break;
		n++;
	}
	if (n == checkLen) {
		// es password
		strPtr += n;
		if (WifiSaver.safeSetPassword(strPtr)) DomoticConnector::conditionalPrintln("New password value setted");
		else DomoticConnector::conditionalPrintln("Error while saving the new password. Keep in mind that you need to set the SSID first.");
		return true;
	}
	
	n = 0;
	checkLen = strlen(EEPROM_DELETE);
	while (n < len && n < checkLen) {
		if (strPtr[n] != EEPROM_DELETE[n]) break;
		n++;
	}
	if (n == checkLen) {
		// es delete
		DomoticConnector::conditionalPrintln("Removing EEPROM data...");
		WifiSaver.emptyEEPROM();
		return true;
	}
	
	return false;
}

DomoticConnector::~DomoticConnector(void) {
	free(this->_id);
	free(this->_group);
	
	delete this->_client;
	delete this->_espClient;
}

char *DomoticConnector::generateID(const char *group) {
	char *id = NULL;
	id = (char*)malloc(sizeof(char)*(strlen(this->_group)+(2*WL_MAC_ADDR_LENGTH)+1));
	if (id == NULL) return id;
	
	uint8_t n = 0, aux = 0;
	// copy group
	while (this->_group[n]) {
		id[n] = this->_group[n];
		n++;
	}
	
	// get MAC
	uint8_t mac[WL_MAC_ADDR_LENGTH];
	WiFi.macAddress(mac);
	
	// copy MAC
	while (aux < WL_MAC_ADDR_LENGTH) {
		id[n] = mac[aux]>>4;
		id[n] += (id[n]<10 ? '0' : ('A'-10));

		n++;
		id[n] = mac[aux]&0x0F;
		id[n] += (id[n]<10 ? '0' : ('A'-10));

		n++;
		aux++;
	}
	id[n] = '\0';
	return id;
}

char *DomoticConnector::getID() {
	return this->_id;
}

String DomoticConnector::getStringID() {
	return String(this->getID());
}

bool DomoticConnector::getDataFromSD(byte sd_pin, char *file_name, String *ssid, String *password) {
	if (!SD.begin(sd_pin)) return false;
	
	File myFile = SD.open(file_name);
	if (myFile == NULL) return false;
	
	byte x = 0;
	while (myFile.available()) {
		String tmp = myFile.readStringUntil('\n');
		if(x==0) *ssid = tmp;
		else if(x==1) *password = tmp;

		x++;
	}
	myFile.close();
	return true;
}

bool DomoticConnector::checkConnection() {
	if (WiFi.status() != WL_CONNECTED) return false;
	if (this->_client->connected()) return true;
	
	// not connected
	if (this->_client->connect(this->_id)) {
		// conected; re-subscribe (if needed)
		DomoticConnector::conditionalPrintln("Connected to MQTT");
		if (this->_subscription == ID_SUBSCRIPTION) this->_client->subscribe(this->_id);
		else if (this->_subscription == GROUP_SUBSCRIPTION) this->_client->subscribe(this->_group);
		
		if (this->_on_reconnect != NULL) this->_on_reconnect(); // notify
		
		return true;
	}
	return false;
}

void DomoticConnector::publish(const char *group, const char *msg) {
	if (this->_client->connected()) this->_client->publish(group, msg);
}

void DomoticConnector::publish(const char *group, String msg) {
	this->publish(group, (const char*)msg.c_str());
}

void DomoticConnector::publishSelf(const char *group, String msg) {
	this->publish(group, (const char*)(this->getStringID() + " " + msg).c_str());
}

void DomoticConnector::publishSelf(const char *group, const char *msg) {
	this->publishSelf(group, String(msg));
}

bool DomoticConnector::loop(void) {
	if (!this->checkConnection()) return false;
	
	WatchdogTimer.loop(); // reset the WDT
	this->_client->loop();
	return true;
}