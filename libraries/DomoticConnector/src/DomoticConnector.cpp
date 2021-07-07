#include "DomoticConnector.h"

bool DomoticConnector::_debug_mode;

uint8_t _subscription;
char *_id;
char *_group;
WiFiClient *_espClient;
PubSubClient *_client;

void DomoticConnector::conditionalPrintln(const char *str) {
	if (DomoticConnector::_debug_mode) Serial.println(str);
}

void DomoticConnector::conditionalPrintln(String str) {
	if (DomoticConnector::_debug_mode) Serial.println((const char*) str.c_str());
}

DomoticConnector::DomoticConnector(const char *ip, uint16_t port, const char *group, uint8_t subscription, MQTT_CALLBACK_SIGNATURE) {
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
	this->_client->setCallback(callback);
}

void DomoticConnector::setup(bool debug_mode, const char *ssid, const char *password, char *file_name) {
	DomoticConnector::_debug_mode = debug_mode;
	WiFi.mode(WIFI_STA);

	if (ssid != NULL && password != NULL) {
		// credentials hardcoded
		DomoticConnector::conditionalPrintln("Wifi over constant: " + String(ssid));
		WiFi.begin(ssid, password);
	}
	else {
		// get credentials from SD
		String sd_ssid, sd_password;
		if (file_name != NULL && DomoticConnector::getDataFromSD(file_name, &sd_ssid,&sd_password)) {
			DomoticConnector::conditionalPrintln("Wifi over SD: " + sd_ssid);
			WiFi.begin((const char*) sd_ssid.c_str(), (const char*) sd_password.c_str());
		}
		else DomoticConnector::conditionalPrintln("Unable to get credentials from SD");
	}
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

bool DomoticConnector::getDataFromSD(char *file_name, String *ssid, String *password) {
	if (!SD.begin(SD_PIN)) return false;
	
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

bool DomoticConnector::loop(void) {
	if (!this->checkConnection()) return false;
	
	this->_client->loop();
	return true;
}