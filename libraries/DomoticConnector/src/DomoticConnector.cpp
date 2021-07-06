#include "DomoticConnector.h"

char *_id;
char *_group;
WiFiClient _espClient;
PubSubClient _client(_espClient);

DomoticConnector::DomoticConnector(char *ip, short port, char *group, char *file_name) {
	WiFi.mode(WIFI_STA);
#if defined(WIFI_NAME) && defined(WIFI_PASS)
	// credentials hardcoded
	WiFi.begin(WIFI_NAME, WIFI_PASS);
#else
	// get credentials from SD
	String ssid, password;
	if (file_name != NULL && this->getDataFromSD(file_name, &ssid,&password)) WiFi.begin((const char*) ssid.c_str(), (const char*) password.c_str());
#endif
	this->_client.setServer(ip, port);

	// copy group identifier
	this->_group = (char*)malloc(sizeof(char)*(strlen(group)+1));
	strcpy(this->_group, group);
	this->_id = this->getID(group);
}

DomoticConnector::~DomoticConnector(void) {
	free(this->_id);
	free(this->_group);
}

char *DomoticConnector::getID(char *group) {
	return (char*)malloc(sizeof(char)*10); // TODO
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

void DomoticConnector::checkConnection() {
  while (WiFi.status() != WL_CONNECTED);
  while (!this->_client.connected()) {
    if (this->_client.connect(this->_id)) this->_client.subscribe(this->_group);
  }
}