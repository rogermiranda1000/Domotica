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

void turnOff(void) {
  analogWrite(WHITEPIN, 0);
  analogWrite(REDPIN, 0);
  analogWrite(GREENPIN, 0);
  analogWrite(BLUEPIN, 0);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (int i = 0; i < length; i++) mensaje += (char)payload[i];
  
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
  }
  else if (msg[0] == "animS") anim = false;
  else if (msg[0] == "off") {
    anim = false;
    turnOff();
  }
  else if (msg[0] == "on") {
    anim = false;
    analogWrite(WHITEPIN, 255);
    analogWrite(REDPIN, 255);
    analogWrite(GREENPIN, 255);
    analogWrite(BLUEPIN, 255);
  }
  else if (msg[0] == "delay") {
    FADESPEED = msg[1].toInt();
  }
  else if (msg[0] == "set") {
    anim = false;
    
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

uint8_t c;
void executeAnimationM(byte pin) {
  analogWrite(pin, c);
  delay(FADESPEED);
  c++;
}

void executeAnimationL(byte pin) {
  analogWrite(pin, c);
  delay(FADESPEED);
  c--;
}

void setup() {
  Serial.begin(9600);
  
  Connector.setup(true, WIFI_NAME, WIFI_PASS, SD_PIN, FILE_NAME);
  connector = new DomoticConnector(MQTT_SERVER, MQTT_PORT, GROUP, onReconnect, ID_SUBSCRIPTION, callback);
  
  pinMode(WHITEPIN, OUTPUT);
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  
  turnOff();
}

void loop() {
  connector->loop();
  if (Serial.available()) Connector.eepromUpdate(Serial.readString());
  
  if (anim) {
    switch (proceso) {
      case 0:
        analogWrite(REDPIN, 0);
        analogWrite(GREENPIN, 0);
        analogWrite(BLUEPIN, 255);
        proceso++;
        break;
      
      case 1:
        c = 0;
        proceso++;
        break;

      case 2:
        executeAnimationM(REDPIN);
        if (c == 0) proceso++; // overflow (256 times done)
        break;
        
      case 3:
        c = 255;
        proceso++;
        break;

      case 4:
        executeAnimationL(BLUEPIN);
        if (c == 255) proceso++; // overflow (256 times done)
        break;
        
      case 5:
        c = 0;
        proceso++;
        break;

      case 6:
        executeAnimationM(GREENPIN);
        if (c == 0) proceso++; // overflow (256 times done)
        break;

      case 7:
        c = 255;
        proceso++;
        break;
        
      case 8:
        executeAnimationL(REDPIN);
        if (c == 255) proceso++; // overflow (256 times done)
        break;

      case 9:
        c = 0;
        proceso++;
        break;
        
      case 10:
        executeAnimationM(BLUEPIN);
        if (c == 0) proceso++; // overflow (256 times done)
        break;

      case 11:
        c = 255;
        proceso++;
        break;
        
      case 12:
        executeAnimationL(GREENPIN);
        if (c == 255) proceso = 0; // overflow (256 times done)
    }
  }
}
