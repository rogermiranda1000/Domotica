#include <DomoticConnector.h>

const char *const WIFI_NAME = NULL;//"...";
const char *const WIFI_PASS = NULL;//"...";

#define MQTT_SERVER "192.168.1.229"
#define MQTT_PORT   1883
#define GROUP       "mag"

#define SD_PIN      D8
#define FILE_NAME   "credentials.txt"

#define APERTURA    5

DomoticConnector *connector;

void setup() {
  Serial.begin(9600);
  
  Connector.setup(true, WIFI_NAME, WIFI_PASS, SD_PIN, FILE_NAME);
  connector = new DomoticConnector(MQTT_SERVER, MQTT_PORT, GROUP);
  
  pinMode(APERTURA, INPUT);
}

void loop() {
  connector->loop();
  
  if (digitalRead(APERTURA)) {
    while (digitalRead(APERTURA)) delay(1);
    Serial.println("Abierto");
    connector->publish("central", connector->getStringID() + "mag OPEN");
  }
  delay(10);
}
