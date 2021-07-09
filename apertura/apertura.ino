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

bool closed;
void setup() {
  Serial.begin(9600);
  
  Connector.setup(true, WIFI_NAME, WIFI_PASS, SD_PIN, FILE_NAME);
  connector = new DomoticConnector(MQTT_SERVER, MQTT_PORT, GROUP);
  
  pinMode(APERTURA, INPUT);
  closed = digitalRead(APERTURA);
}

void loop() {
  connector->loop();
  if (Serial.available()) Connector.eepromUpdate(Serial.readString());

  if (closed) {
    if (!digitalRead(APERTURA)) {
      // se ha abierto
      Serial.println("Abierto");
      connector->publish("central", connector->getStringID() + " mag OPEN");
      closed = false;
    }
  }
  else if (digitalRead(APERTURA)) {
    // se ha cerrado
    Serial.println("Closed");
    closed = true;
  }
}
