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

// screen resolution
#define RESOLUTION_X 240
#define RESOLUTION_Y 240

// LEDs
#define NUMPIXELS 5
#define RGB(R,G,B) carrier.leds.Color(G,R,B)

// btn
#define NUMBTN            5
#define BOUNCE_GLITCH_MS  200

const char *const WIFI_NAME = SECRET_SSID;
const char *const WIFI_PASS = SECRET_PASS;

MKRIoTCarrier carrier;
DomoticConnector *connector;

volatile unsigned int retraso = 1000;
unsigned long acumulado, lastTick[NUMBTN];

void showScreen(byte *msg, unsigned int length);

void callback(char* topic, byte* payload, unsigned int length) {
  String cmd;

  uint16_t n = 0;
  while (n < length && (char)payload[n] != ' ') {
    cmd += (char)payload[n];
    n++;
  }
  n++; // skip ' '

  if (cmd == String("delay")) {
    unsigned int value = 0;
    // atoi
    while (n<length) {
      value = value*10 + ((char)payload[n])-'0';
      n++;
    }

    DEBUG_PRINT("Retraso de ");
    DEBUG_PRINT(String(value));
    DEBUG_PRINTLN("s");
    retraso = 1000*value;
  }
  else if (cmd == String("set")) {
    DEBUG_PRINTLN("Updating screen...");
    showScreen((char*)(payload+n), length-n); // espero que sizeof(char) == sizeof(byte)...
  }
}

void onReconnect(void) {
  connector->publishSelf("central", String(GROUP) + " ?");
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
  for (uint8_t n = 0; n < NUMBTN; n++) lastTick[n] = 0;
}

void loop() {
  connector->loop();
  // Arduino Wifi 1010 no tiene EEPROM
  
  carrier.Buttons.update();
  for (uint8_t btn = 0; btn < NUMBTN; btn++) {
    if (carrier.Buttons.onTouchDown((touchButtons)btn)) {
      if (millis() - lastTick[btn] <= BOUNCE_GLITCH_MS) continue;
      
      lastTick[btn] = millis();
      DEBUG_PRINTLN("Botón " + String(btn) + " pulsado");
      connector->publish("central", connector->getStringID() + " btn " + connector->getStringID() + String(btn)); // el botón ID notifica que se ha pulsado, y que realize la acción 'ID0'
    }
  }

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

  /*int r, g, b, c, light_level;
  carrier.Light.readColor(r, g, b);
  light_level = carrier.Light.calculateLux(r,g,b);
  DEBUG_PRINT("Light = ");
  DEBUG_PRINT(light_level);
  DEBUG_PRINTLN(" lumens/m^2");
  connector->publishSelf("central", "luzSI " + String(light_level));*/

  // show LEDs
  /*carrier.leds.setPixelColor(0, RGB(255,0,0));
  carrier.leds.setPixelColor(1, RGB(0,255,0));
  carrier.leds.setPixelColor(2, RGB(0,0,255));
  carrier.leds.setPixelColor(3, RGB(0,255,255));
  carrier.leds.setPixelColor(4, RGB(255,0,255));
  carrier.leds.show();*/
}

// ID/nombre, tipo de sensor, valor
void showScreen(char *msg, unsigned int length) {
  carrier.display.setCursor(50, 50);
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);
  for (uint16_t n = 0; n < length; n++) carrier.display.print(msg[n]);
  carrier.display.println();
}
