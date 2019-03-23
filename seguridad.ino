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
char* ID = "SEG01";

const int fuego = D0;
const int MQ2 = D1;
const int MQ5 = D2;
const int MQ7 = D3;
const int buzzer = D5;
const int MQ135 = A0;

bool alarma = false;
int tiempo = 0;

void setup() {
  Serial.begin(9600);
  pinMode(MQ2, OUTPUT);
  pinMode(MQ5, OUTPUT);
  pinMode(MQ7, OUTPUT);
  pinMode(MQ135, OUTPUT);
  
  conectar();
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  if(digitalRead(MQ2)==1 || digitalRead(MQ5)==1 || digitalRead(MQ7)==1) client.publish("central", (const char*) (String(ID) + " gas 1").c_str());
  if(digitalRead(fuego)==1) client.publish("central", (const char*) (String(ID) + " fuego").c_str());
  
  int gas = analogRead(MQ135);
  if(gas>=5) client.publish("central", (const char*) (String(ID) + " gas " + String(gas)).c_str());

  if(alarma) {
    tiempo++;
    if((tiempo%100)<=50) tone(buzzer, 1000);
    else noTone(buzzer);
  }
}




void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (int i=0;i<length;i++) mensaje += (char)payload[i];
  if(mensaje=="act") alarma = true;
  else if(mensaje=="deact") alarma = false;
}
 
 
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando...");
    if (client.connect(ID)) {
      Serial.println(" Conectado!");
      client.subscribe(ID);
      client.publish("central", (const char*) (String(ID) + " ?").c_str());
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
  if(dest) client.setServer(mqtt_server, 1883);
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
      if(x==0) ssid = msgo;
      else if(x==1) password = msgo;

      x++;
    }
    myFile.close();
  } else {
    Serial.println("Error en la lectura de la MicroSD.");
  }

  ssid.remove(ssid.length()-1);
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
