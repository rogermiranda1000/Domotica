#include <SparkFunWeatherShield.h>
#include <DomoticConnector.h>
#include "secrets.h" // place here #define SECRET_SSID & SECRET_PASS

#define DEBUG 0
#if (DEBUG==1)
  #define DEBUG_PRINT(str) Serial.print(str)
  #define DEBUG_PRINTLN(str) Serial.println(str)
#else
  #define DEBUG_PRINT(str)
  #define DEBUG_PRINTLN(str)
#endif

#define MQTT_SERVER "192.168.1.79"
#define MQTT_PORT   1883
#define GROUP       "met"

const char *const WIFI_NAME = SECRET_SSID;
const char *const WIFI_PASS = SECRET_PASS;

DomoticConnector *connector;
WeatherShield weather;

volatile unsigned int retraso = 1000;
unsigned long acumulado;

void callback(char* topic, byte* payload, unsigned int length) {
  unsigned int value = 0;
  for (int i=0;i<length;i++) value = value*10 + ((char)payload[i])-'0'; // atoi

  DEBUG_PRINT("Retraso de ");
  DEBUG_PRINT(value);
  DEBUG_PRINTLN("s");
  retraso = 1000*value;
}

void onReconnect(void) {
  connector->publishSelf("central", "?");
}

void setup() {
  Serial.begin(9600);

  Connector.setup(DEBUG, true, WIFI_NAME, WIFI_PASS);
  connector = new DomoticConnector(MQTT_SERVER, MQTT_PORT, GROUP, onReconnect, ID_SUBSCRIPTION, callback);
  
  weather.begin(); // enable I2C & sensors

  acumulado = millis();
}

// print readings every second
void loop() {
  connector->loop();
  if (Serial.available()) Connector.eepromUpdate(Serial.readString());
  
  weather.loop();

  unsigned long current = millis();
  if (current - acumulado < retraso) return;
  acumulado = current;
  DEBUG_PRINTLN();
  
  //Check Humidity Sensor
  float humidity = weather.readHumidity();

  if (humidity == ERROR_I2C_TIMEOUT) Serial.println("I2C error."); // humidty sensor failed to respond
  else {
    DEBUG_PRINT("Humidity = ");
    DEBUG_PRINT(humidity);
    DEBUG_PRINTLN("%");
    connector->publishSelf("central", "humedad " + String(humidity));
    
    float temp = weather.readTemperature();
    DEBUG_PRINT("Temp = ");
    DEBUG_PRINT(temp, 2);
    DEBUG_PRINTLN("C");
    connector->publishSelf("central", "temperatura " + String(temp));

    //Check Pressure Sensor
    float pressure = weather.readPressure();
    DEBUG_PRINT("Pressure = ");
    DEBUG_PRINT(pressure);
    DEBUG_PRINTLN("hPa");
    connector->publishSelf("central", "presion " + String(pressure));

    // light and battery doesn't use I2C, but if the I2C it's not working it's very likely (due to internal connections) that light nor battery will work
    //Check light sensor
    float light_level = weather.getLightLevel();
    DEBUG_PRINT("Light = ");
    DEBUG_PRINT(light_level);
    DEBUG_PRINTLN("%");
    connector->publishSelf("central", "luz " + String(light_level));
  
    //Check batt level
    float batt_lvl = weather.getBatteryLevel();
    DEBUG_PRINT("Battery = ");
    DEBUG_PRINT(batt_lvl);
    DEBUG_PRINTLN("V");
    // TODO enviar?
  }
  
  unsigned int dir = weather.getWindDirection();
  if (weather.decodeWindDirection(dir) == -1) Serial.println("Unconnected external sensors.");
  else {
    DEBUG_PRINT("Direction = ");
    DEBUG_PRINTLN(dir);
    connector->publishSelf("central", "direccion " + String(dir));
    
    float speed = weather.getWindSpeedKm();
    DEBUG_PRINT("Speed = ");
    DEBUG_PRINT(speed);
    DEBUG_PRINTLN("Km/h");
    connector->publishSelf("central", "viento " + String(speed));
  }
  
  float rain = weather.getRain();
  DEBUG_PRINT("Rain = ");
  DEBUG_PRINT(rain);
  DEBUG_PRINTLN("\"/s");
  connector->publishSelf("central", "agua " + String(rain));
}
