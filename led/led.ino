#include <DomoticConnector.h>

const char *const WIFI_NAME = NULL;//"...";
const char *const WIFI_PASS = NULL;//"...";

#define MQTT_SERVER "192.168.1.229"
#define MQTT_PORT   1883
#define GROUP       "led"

#define SD_PIN      D2
#define FILE_NAME   "credentials.txt"

#define WHITEPIN    D1
#define REDPIN      D3
#define GREENPIN    D8
#define BLUEPIN     D4

DomoticConnector *connector;
volatile bool anim = false;
uint8_t proceso = 0;
volatile int FADESPEED = 5; // make this higher to slow down

void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (int i = 0; i < length; i++) mensaje += (char)payload[i];
  //Serial.println(mensaje);
  String msg[3] = {"", "", ""};
  int msg_val = 0;
  int pos;
  msg[0] = mensaje;
  while ((pos = mensaje.indexOf (' ')) != -1) {
    msg[msg_val] = mensaje.substring(0, pos);
    msg_val++;
    mensaje = mensaje.substring (pos + 1, mensaje.length());
  }
  msg[msg_val] = mensaje;
  Serial.println(msg[0] + "; " + msg[1] + "; " + msg[2]);
  if (msg[0] == "anim") {
    anim = true;
    proceso = 0;
    digitalWrite(REDPIN, LOW);
    digitalWrite(GREENPIN, LOW);
    digitalWrite(BLUEPIN, HIGH);
  }
  else if (msg[0] == "animS") anim = false;
  else if (msg[0] == "off") {
    digitalWrite(WHITEPIN, LOW);
    digitalWrite(REDPIN, LOW);
    digitalWrite(GREENPIN, LOW);
    digitalWrite(BLUEPIN, LOW);
  }
  else if (msg[0] == "on") {
    digitalWrite(WHITEPIN, HIGH);
    digitalWrite(REDPIN, HIGH);
    digitalWrite(GREENPIN, HIGH);
    digitalWrite(BLUEPIN, HIGH);
  }
  else if (msg[0] == "delay") {
    FADESPEED = msg[1].toInt();
  }
  else if (msg[0] == "set") {
    byte pin;
    if (msg[1] == "white") pin = WHITEPIN;
    else if (msg[1] == "red") pin = REDPIN;
    else if (msg[1] == "green") pin = GREENPIN;
    else if (msg[1] == "blue") pin = BLUEPIN;
    else return;
    
    analogWrite(pin, (unsigned byte)msg[2].toInt());
  }
}

void onReconnect(void) {
  connector->publish("central", connector->getStringID() + " led");
}

void executeAnimationM(int PIN) {
  for (short c = 0; c < 256; c++) {
    analogWrite(PIN, c);
    delay(FADESPEED);
  }
}

void executeAnimationL(int PIN) {
  for (short c = 255; c > 0; c--) {
    analogWrite(PIN, c);
    delay(FADESPEED);
  }
}

void setup() {
  Serial.begin(9600);
  
  Connector.setup(true, WIFI_NAME, WIFI_PASS, SD_PIN, FILE_NAME);
  connector = new DomoticConnector(MQTT_SERVER, MQTT_PORT, GROUP, onReconnect, ID_SUBSCRIPTION, callback);
  
  pinMode(WHITEPIN, OUTPUT);
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  
  digitalWrite(WHITEPIN, LOW);
  digitalWrite(REDPIN, LOW);
  digitalWrite(GREENPIN, LOW);
  digitalWrite(BLUEPIN, LOW);
}

void loop() {
  connector->loop();
  if (Serial.available()) Connector.eepromUpdate(Serial.readString());
  
  if (!anim) {
    switch (proceso) {
      case 0:
        executeAnimationM(REDPIN);
        break;
        
      case 1:
        executeAnimationL(BLUEPIN);
        break;
        
      case 2:
        executeAnimationM(GREENPIN);
        break;
        
      case 3:
        executeAnimationL(REDPIN);
        break;
        
      case 4:
        executeAnimationM(BLUEPIN);
        break;
        
      case 5:
        executeAnimationL(GREENPIN);
    }
    
    proceso++;
    if (proceso == 6) proceso = 0;
  }
}
