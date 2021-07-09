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

unsigned int retraso = 1000, acumulado;

void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (int i=0;i<length;i++) mensaje += (char)payload[i];
  Serial.println("Retraso de " + mensaje + "ms.");
  retraso = mensaje.toInt();
}

void onReconnect(void) {
  connector->publishSelf("central", "?");
}

void setup() {
  Serial.begin(9600);

  Connector.setup(DEBUG, WIFI_NAME, WIFI_PASS);
  connector = new DomoticConnector(MQTT_SERVER, MQTT_PORT, GROUP, onReconnect, ID_SUBSCRIPTION, callback);
  
  weather.begin(); // enable I2C & sensors

  acumulado = millis();
}

// print readings every second
void loop() {
  connector->loop();
  if (Serial.available()) Connector.eepromUpdate(Serial.readString());

  if (millis() - acumulado < retraso) return;
  acumulado = millis();
  
  //Check Humidity Sensor
  float humidity = weather.readHumidity();

  if (humidity == ERROR_I2C_TIMEOUT) Serial.println("I2C error."); // humidty sensor failed to respond
  else {
    Serial.print("Humidity = ");
    Serial.print(humidity);
    Serial.println("%");
    connector->publishSelf("central", "humedad " + String(humidity));
    
    float temp = weather.readTemperature();
    Serial.print("Temp = ");
    Serial.print(temp, 2);
    Serial.println("C");
    connector->publishSelf("central", "temperatura " + String(temp));

    //Check Pressure Sensor
    float pressure = weather.readPressure();
    Serial.print("Pressure = ");
    Serial.print(pressure);
    Serial.println("Pa");
    connector->publishSelf("central", "presion " + String(pressure));

    // light and battery doesn't use I2C, but if the I2C it's not working it's very likely (due to internal connections) that light nor battery will work
    //Check light sensor
    float light_level = weather.getLightLevel();
    Serial.print("Light = ");
    Serial.print(light_level);
    Serial.println("%");
    connector->publishSelf("central", "luz " + String(light_level));
  
    //Check batt level
    float batt_lvl = weather.getBatteryLevel();
    Serial.print("Battery = ");
    Serial.print(batt_lvl);
    Serial.println("V");
    // TODO enviar?
  }
  
  float speed = weather.getWindSpeedKm();
  Serial.print("Speed = ");
  Serial.print(speed);
  Serial.println("Km/h");
  connector->publishSelf("central", "viento " + String(speed));
  
  int dir = weather.getWindDirection();
  Serial.print("Direction = ");
  Serial.println(dir);
  connector->publishSelf("central", "direccion " + String(dir));
  
  float rain = weather.getRain();
  Serial.print("Rain = ");
  Serial.print(rain);
  Serial.println("\"/s");
  connector->publishSelf("central", "agua " + String(rain));

  Serial.println();

  delay(1000);
}
