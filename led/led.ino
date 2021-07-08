#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <SD.h>
String ssid = "ASUS";
String password = "8TEqumxs6K35";
const char* mqtt_server = "192.168.1.230";
const char* mqtt_server2 = "192.168.1.229";
bool dest = false;
WiFiClient espClient;
PubSubClient client(espClient);
File myFile;
const char* cliente = "LED01";
const int WHITEPIN = D1;
const int REDPIN = D3;
const int GREENPIN = D8;
const int BLUEPIN = D4;
int FADESPEED = 5; // make this higher to slow down
bool anim = false;
int proceso = 0;

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
    int num = msg[2].toInt();
    int pin = 0;
    if (msg[1] == "white") pin = WHITEPIN;
    else if (msg[1] == "red") pin = REDPIN;
    else if (msg[1] == "green") pin = GREENPIN;
    else if (msg[1] == "blue") pin = BLUEPIN;
    analogWrite(pin, num);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando...");
    if (client.connect(cliente)) {
      Serial.println(" Conectado!");
      client.subscribe(cliente);
      client.publish("central", (const char*) (String(cliente) + " led").c_str());
    } else {
      Serial.print("fallo, rc=");
      Serial.print(client.state());
      Serial.println(" intentado otra vez en 2 segundos...");
      cambiarDestino();
      delay(2000);
    }
  }
}

void cambiarDestino() {
  dest = !dest;
  if (dest) client.setServer(mqtt_server, 1883);
  else client.setServer(mqtt_server2, 1883);
}

void conectar() {
  if (!SD.begin(4)) {
    Serial.println("Error en la lectura de la MicroSD.");
  }
  myFile = SD.open("send.txt");
  if (myFile) {
    Serial.println("SD:");
    int x = 0;
    while (myFile.available()) {
      String msgo = myFile.readStringUntil('\n');
      Serial.println(msgo);
      if (x == 0) ssid = msgo;
      else if (x == 1) password = msgo;
      x++;
    }
    myFile.close();
  } else {
    Serial.println("Error en la lectura de la MicroSD.");
  }
  //ssid.remove(ssid.length() - 1);
  Serial.println("ssid: '" + ssid + "'");
  Serial.println("pass: '" + password + "'");
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin((const char*) ssid.c_str(), (const char*) password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  cambiarDestino();
  client.setCallback(callback);
}

void setup() {
  Serial.begin(9600);
  pinMode(WHITEPIN, OUTPUT);
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  digitalWrite(WHITEPIN, LOW);
  digitalWrite(REDPIN, LOW);
  digitalWrite(GREENPIN, LOW);
  digitalWrite(BLUEPIN, LOW);
  conectar();
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  if (anim) {
    if (++proceso == 1) executeAnimationM(REDPIN);
    else if (proceso == 2) executeAnimationL(BLUEPIN);
    else if (proceso == 3) executeAnimationM(GREENPIN);
    else if (proceso == 4) executeAnimationL(REDPIN);
    else if (proceso == 5) executeAnimationM(BLUEPIN);
    else if (proceso == 6) executeAnimationL(GREENPIN);
    else if (proceso == 7) proceso = 0;
  }
}

void executeAnimationM(int PIN) {
  for (int c = 0; c < 256; c++) {
    analogWrite(PIN, c);
    delay(FADESPEED);
  }
}

void executeAnimationL(int PIN) {
  for (int c = 255; c > 0; c--) {
    analogWrite(PIN, c);
    delay(FADESPEED);
  }
}
