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

volatile unsigned int retraso = 1000;
unsigned long acumulado;

void callback(char* topic, byte* payload, unsigned int length) {
  unsigned int value = 0;
  for (int i=0;i<length;i++) value = value*10 + ((char)payload[i])-'0'; // atoi
  
  Serial.println("Retraso de " + String(value) + "s");
  retraso = 1000*value;
}

void onReconnect(void) {
  connector->publishSelf("central", "?");
}

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
  connector = new DomoticConnector(MQTT_SERVER, MQTT_PORT, GROUP, onReconnect, ID_SUBSCRIPTION, callback);
  
  acumulado = millis();
}

void loop() {
  connector->loop();
  // Arduino Wifi 1010 no tiene EEPROM

  unsigned long current = millis();
  if (current - acumulado < retraso) return;
  acumulado = current;
  DEBUG_PRINTLN();
  
  float pressure = carrier.Pressure.readPressure() * 10;
  DEBUG_PRINT("Pressure = ");
  DEBUG_PRINT(pressure);
  DEBUG_PRINTLN(" hPa");
  connector->publishSelf("central", "presion " + String(pressure));
  
  float temperature = carrier.Env.readTemperature();
  DEBUG_PRINT("Temperature = ");
  DEBUG_PRINT(temperature);
  DEBUG_PRINTLN(" °C");
  connector->publishSelf("central", "temperatura " + String(temperature));
  
  float humidity = carrier.Env.readHumidity();
  DEBUG_PRINT("Humidity = ");
  DEBUG_PRINT(humidity);
  DEBUG_PRINTLN(" %");
  connector->publishSelf("central", "humedad " + String(humidity));
}
