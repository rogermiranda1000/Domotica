#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <SD.h>
#include <DHT.h>
String ssid = "";
String password = "";
char* mqtt_server = "192.168.1.230";
char* mqtt_server2 = "192.168.1.229";
bool dest = false;
WiFiClient espClient;
PubSubClient client(espClient);
File myFile;
const int pinA = 5;
void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (int i = 0; i < length; i++) mensaje += (char)payload[i];
}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando...");
    if (client.connect("MAG01")) {
      Serial.println(" Conectado!");
      client.subscribe("MAG01");
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
  ssid.remove(ssid.length() - 1);
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
  pinMode(pinA, INPUT);
  conectar();
}
void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  if (digitalRead(pinA)) {
    while (digitalRead(pinA)) delay(1);
    Serial.println("Abierto");
    client.publish("central", "MAG01 mag OPEN");
  }
  delay(10);
}
