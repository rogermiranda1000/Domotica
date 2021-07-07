#include <DomoticConnector.h>
#include "Secret.h" // #define WIFI_NAME & WIFI_PASS here

#define DEBUG

#define MQTT_SERVER "192.168.1.229"
#define MQTT_PORT   1883
#define GROUP       "btn"

#define BUTTON_PIN  D2
#define FILE_NAME   "credentials.txt"

DomoticConnector connector(MQTT_SERVER, MQTT_PORT, GROUP, FILE_NAME);

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  connector.setup();
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  connector.loop();
  
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
    connector.sendMessage("central", connector.getStringID() + " btn " + connector.getStringID()); // el botón ID notifica que se ha pulsado, y que realize la acción 'ID'
    
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
