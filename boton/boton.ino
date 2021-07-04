#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SD.h>
#include "Secret.h" // #define WIFI_NAME & WIFI_PASS here

#define DEBUG
// big brain; cortesia de https://stackoverflow.com/a/5586469/9178470
#ifdef  DEBUG
  #define DEBUG_PRINT(str) Serial.print(str)
  #define DEBUG_PRINTLN(str) Serial.println(str)
#else
  #define DEBUG_PRINT(str)
  #define DEBUG_PRINTLN(str)
#endif

#define MQTT_SERVER "192.168.1.229"
#define MQTT_PORT   1883
#define ID          "btn01"
#define GROUP       "btn"

#define SD_PIN      D8
#define FILE_NAME   "credentials.txt"

#define BUTTON_PIN  D2

WiFiClient espClient;
PubSubClient client(espClient);

void checkConnection() {
  while (!client.connected()) {
    DEBUG_PRINT("Conectando...");
    if (client.connect(ID)) {
      DEBUG_PRINTLN(" Conectado!");
      client.subscribe(GROUP);
    }
    else {
      DEBUG_PRINT("fallo, rc=");
      DEBUG_PRINT(client.state());
      DEBUG_PRINTLN(" intentado otra vez en 2 segundos...");
      delay(2000);
    }
  }
}

bool getDataFromSD(String *ssid, String *password) {
  if (!SD.begin(SD_PIN)) return false;
  
  File myFile = SD.open(FILE_NAME);
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

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
#ifdef DEBUG
  Serial.begin(9600);
#endif

  String ssid, password;
#if defined(WIFI_NAME) && defined(WIFI_PASS)
  ssid = String(WIFI_NAME);
  password = String(WIFI_PASS);
#else
  // no #define; get data from SD
  if(!getDataFromSD(&ssid, &password)) {
    DEBUG_PRINTLN("Unable to read SD data");
    while (true) delay(9999);
  }
#endif

  DEBUG_PRINT("Connecting to " + ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin((const char*) ssid.c_str(), (const char*) password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUG_PRINT(".");
  }
  DEBUG_PRINTLN("");
  client.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  checkConnection();
  client.loop();

  static bool pressed = false;
  if(!pressed) {
    // apretar el botón?
    if (digitalRead(BUTTON_PIN) == HIGH) return; // not pressed
    
    // falsa pulsación?
    unsigned long inicial = millis();
    while (millis()-inicial <= 17) {
      if(digitalRead(BUTTON_PIN) == HIGH) return;
    }

    DEBUG_PRINTLN("Botón pulsado.");
    client.publish("central", ID " btn " ID); // el botón ID notifica que se ha pulsado, y que realize la acción 'ID'
    
    pressed = true;
  }
  else {
    // soltar el botón?
    if (digitalRead(BUTTON_PIN) == LOW) return; // still pressed
    
    // falso release?
    unsigned long inicial = millis();
    while (millis()-inicial <= 17) {
      if(digitalRead(BUTTON_PIN) == LOW) return;
    }
    
    pressed = false;
  }
}
