#include "DomoticConnector.h"

byte _subscription;
char *_id;
char *_group;
WiFiClient *_espClient;
PubSubClient *_client;

// temporales (se destruyen al entrar a setup)
char *_sd_file_name;
char *_ip;
short _port;
MQTT_CALLBACK_SIGNATURE;

DomoticConnector::DomoticConnector(char *ip, short port, char *group, byte subscription, MQTT_CALLBACK_SIGNATURE, char *file_name) {
	this->_subscription = subscription;
	this->_port = port;
	this->callback = callback;
	
	this->_ip = NULL;
	this->_ip = (char*)malloc(sizeof(char)*(strlen(ip)+1));
	if (this->_ip != NULL) strcpy(this->_ip, ip);

	// copy group identifier
	this->_group = NULL;
	this->_group = (char*)malloc(sizeof(char)*(strlen(group)+1));
	if (this->_group != NULL) strcpy(this->_group, group);
	
	this->_id = this->generateID(group);
	
	if (file_name != NULL) {
		this->_sd_file_name = NULL;
		this->_sd_file_name = (char*)malloc(sizeof(char)*(strlen(file_name)+1));
		if (this->_sd_file_name != NULL) strcpy(this->_sd_file_name, file_name);
	}
	
	this->_espClient = new WiFiClient();
	this->_client = new PubSubClient(*this->_espClient);
}

void DomoticConnector::setup(void) {
	DEBUG_PRINTLN("Using id " + String(this->getID()));

	WiFi.mode(WIFI_STA);
#if defined(WIFI_NAME) && defined(WIFI_PASS)
	// credentials hardcoded
	DEBUG_PRINTLN("Wifi over #define: " + String(WIFI_NAME));
	WiFi.begin(WIFI_NAME, WIFI_PASS);
#else
	// get credentials from SD
	String ssid, password;
	if (this->_sd_file_name != NULL) {
		if (this->getDataFromSD(this->_sd_file_name, &ssid,&password)) {
			DEBUG_PRINTLN("Wifi over SD: " + ssid);
			WiFi.begin((const char*) ssid.c_str(), (const char*) password.c_str());
		}
		
		free(this->_sd_file_name);
		this->_sd_file_name = NULL;
	}
#endif
	if (this->_ip == NULL) return; // ya se ha llamado setup()
	this->_client->setServer(this->_ip, this->_port);
	this->_client->setCallback(this->callback);
	
	free(this->_ip);
	this->_ip = NULL;
}

DomoticConnector::~DomoticConnector(void) {
	free(this->_id);
	free(this->_group);
	
	delete this->_client;
	delete this->_espClient;
}

char *DomoticConnector::generateID(char *group) {
	byte mac[6];
	WiFi.macAddress(mac);

	char *id = (char*)malloc(sizeof(char)*(strlen(this->_group)+(2*6)+1));
	if (id == NULL) return id;
	
	byte n = 0, aux = 0;
	// copy group
	while (this->_group[n]) {
		id[n] = this->_group[n];
		n++;
	}
	// copy MAC
	while (aux < 6) {
		id[n] = mac[aux]>>4;
		id[n] += (id[n]<10 ? '0' : ('A'-10));

		n++;
		id[n] = mac[aux]&0x0F;
		id[n] += (id[n]<10 ? '0' : ('A'-10));

		n++;
		aux++;
	}
	id[n] = '\0';
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
		DEBUG_PRINTLN("Connected to MQTT");
		if (this->_subscription == ID_SUBSCRIPTION) this->_client->subscribe(this->_id);
		else if (this->_subscription == GROUP_SUBSCRIPTION) this->_client->subscribe(this->_group);
		return true;
	}
	return false;
}

void DomoticConnector::sendMessage(char *group, char *msg) {
	if (this->_client->connected()) this->_client->publish(group, msg);
}

void DomoticConnector::sendMessage(char *group, String msg) {
	this->sendMessage(group, msg.c_str());
}

bool DomoticConnector::loop(void) {
	if (!this->checkConnection()) return false;
	
	this->_client->loop();
	return true;
}