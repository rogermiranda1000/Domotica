#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <SD.h>
#include "Secret.h" // save WIFI_NAME & WIFI_PASS here

#define DEBUG

#define MQTT_SERVER "192.168.1.229"
#define MQTT_PORT   1883
#define ID          "btn01"
#define GROUP       "btn"

#define SD_PIN      4

#define BUTTON_PIN  D2

WiFiClient espClient;
PubSubClient client(espClient);

void checkConnection() {
  while (!client.connected()) {
#ifdef DEBUG
    Serial.print("Conectando...");
#endif
    if (client.connect(ID)) {
#ifdef DEBUG
      Serial.println(" Conectado!");
#endif
      client.subscribe(GROUP);
    }
    else {
#ifdef DEBUG
      Serial.print("fallo, rc=");
      Serial.print(client.state());
      Serial.println(" intentado otra vez en 2 segundos...");
#endif
      delay(2000);
    }
  }
}

bool getDataFromSD(String *ssid, String *password) {
  if (!SD.begin(SD_PIN)) return false;
  
  File myFile = SD.open("send.txt");
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
  #ifdef DEBUG
    Serial.println("Unable to read SD data");
  #endif
      while (true) delay(9999);
    }
#endif

#ifdef DEBUG
  Serial.println("ssid: " + ssid);
  Serial.println("pass: " + password);
#endif
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

#ifdef DEBUG
    Serial.println("Botón pulsado.");
#endif
    client.publish("central", "btn01");
    
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
