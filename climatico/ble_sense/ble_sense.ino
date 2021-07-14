#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>
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
#define GROUP       "clim"

const char *const WIFI_NAME = SECRET_SSID;
const char *const WIFI_PASS = SECRET_PASS;

DomoticConnector *connector;

bool temp = true, pressure = true;

volatile unsigned int retraso = 1000;
unsigned long acumulado;

void callback(char* topic, byte* payload, unsigned int length);
void onReconnect(void);

void setup() {
#if (DEBUG == 1)
  Serial.begin(9600);
#endif

  Connector.setup(DEBUG, WIFI_NAME, WIFI_PASS);
  connector = new DomoticConnector(MQTT_SERVER, MQTT_PORT, GROUP, onReconnect, ID_SUBSCRIPTION, callback);

  if (!HTS.begin()) {
    temp = false;
    DEBUG_PRINTLN("Failed to initialize humidity temperature sensor!");
  }
  if (!BARO.begin()) {
    pressure = false;
    DEBUG_PRINTLN("Failed to initialize pressure sensor!");
  }
  
  acumulado = millis();
}

void callback(char* topic, byte* payload, unsigned int length) {
  unsigned int value = 0;
  for (int i=0;i<length;i++) value = value*10 + ((char)payload[i])-'0'; // atoi
  
  Serial.println("Retraso de " + String(value) + "s");
  retraso = 1000*value;
}

void onReconnect(void) {
  connector->publishSelf("central", "?");
}

void loop() {
  connector->loop();
  // el Arduino BLE no tiene EEPROM
  
  unsigned long current = millis();
  if (current - acumulado < retraso) return;
  acumulado = current;
  DEBUG_PRINTLN();
  
  if (temp) {
    float temperature = HTS.readTemperature();
    float humidity    = HTS.readHumidity();
    
    DEBUG_PRINT("Temperature = ");
    DEBUG_PRINT(temperature);
    DEBUG_PRINTLN("C");
    connector->publishSelf("central", "temperatura " + String(temperature));
  
    DEBUG_PRINT("Humidity    = ");
    DEBUG_PRINT(humidity);
    DEBUG_PRINTLN("%");
    connector->publishSelf("central", "humedad " + String(humidity));
  }

  if (pressure) {
    float pressure = BARO.readPressure()*10; // pressure in kPa, but expecting hPa
    
    DEBUG_PRINT("Pressure = ");
    DEBUG_PRINT(pressure);
    DEBUG_PRINTLN("hPa");
    connector->publishSelf("central", "presion " + String(pressure));
  }
}
