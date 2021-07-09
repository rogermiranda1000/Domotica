#include <SparkFunWeatherShield.h>
#include <DomoticConnector.h>

#define DEBUG 0
#if (DEBUG==1)
  #define DEBUG_PRINT(str) Serial.print(str)
  #define DEBUG_PRINTLN(str) Serial.println(str)
#else
  #define DEBUG_PRINT(str)
  #define DEBUG_PRINTLN(str)
#endif

#define MQTT_SERVER "192.168.1.229"
#define MQTT_PORT   1883
#define GROUP       "met"

const char *const WIFI_NAME = NULL;//"...";
const char *const WIFI_PASS = NULL;//"...";

DomoticConnector *connector;
WeatherShield weather;

void setup() {
  Serial.begin(9600);

  Connector.setup(DEBUG, WIFI_NAME, WIFI_PASS);
  connector = new DomoticConnector(MQTT_SERVER, MQTT_PORT, GROUP);
  
  weather.begin(); // enable I2C & sensors
}

// print readings every second
void loop() {
  connector->loop();
  if (Serial.available()) Connector.eepromUpdate(Serial.readString());
  
  //Check Humidity Sensor
  float humidity = weather.readHumidity();

  if (humidity == ERROR_I2C_TIMEOUT) Serial.println("I2C error."); // humidty sensor failed to respond
  else {
    Serial.print("Humidity = ");
    Serial.print(humidity);
    Serial.println("%");
    
    float temp = weather.readTemperature();
    Serial.print("Temp = ");
    Serial.print(temp, 2);
    Serial.println("C");

    //Check Pressure Sensor
    float pressure = weather.readPressure();
    Serial.print("Pressure = ");
    Serial.print(pressure);
    Serial.println("Pa");

    // light and battery doesn't use I2C, but if the I2C it's not working it's very likely (due to internal connections) that light nor battery will work
    //Check light sensor
    float light_level = weather.getLightLevel();
    Serial.print("Light = ");
    Serial.print(light_level);
    Serial.println("V");
  
    //Check batt level
    float batt_lvl = weather.getBatteryLevel();
    Serial.print("Battery = ");
    Serial.print(batt_lvl);
    Serial.println("V");
  }
  
  float speed = weather.getWindSpeed();
  Serial.print("Speed = ");
  Serial.print(speed);
  Serial.println("MPH");
  
  int dir = weather.getWindDirection();
  Serial.print("Direction = ");
  Serial.println(dir);
  
  float rain = weather.getRain();
  Serial.print("Rain = ");
  Serial.print(rain);
  Serial.println("\"/s");

  Serial.println();

  delay(1000);
}
