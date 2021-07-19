#include <Arduino_MKRIoTCarrier.h>
#include <DomoticConnector.h>
#include "secrets.h" // place here #define SECRET_SSID & SECRET_PASS

#define DEBUG 1
#if (DEBUG==1)
  #define DEBUG_PRINT(str) Serial.print(str)
  #define DEBUG_PRINTLN(str) Serial.println(str)
#else
  #define DEBUG_PRINT(str)
  #define DEBUG_PRINTLN(str)
#endif

#define MQTT_SERVER "192.168.1.79"
#define MQTT_PORT   1883
#define GROUP       "info"

#define SD_PIN      SD_CS
#define FILE_NAME   "credentials.txt"

const char *const WIFI_NAME = SECRET_SSID;
const char *const WIFI_PASS = SECRET_PASS;

MKRIoTCarrier carrier;
DomoticConnector *connector;

void setup() {
#if DEBUG
  Serial.begin(9600);
  while (!Serial) delay(500);
#endif

  CARRIER_CASE = false;
  if (!carrier.begin()) {
    Serial.println("Failed to initialize!");
    while (true) delay(99999);
  }

  Connector.setup(DEBUG, WIFI_NAME, WIFI_PASS, SD_PIN, FILE_NAME);
  connector = new DomoticConnector(MQTT_SERVER, MQTT_PORT, GROUP);
}

void loop() {
  connector->loop();
  // Arduino Wifi 1010 no tiene EEPROM
}
