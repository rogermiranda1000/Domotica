#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <SD.h>

String ssid = "";
String password = "";
char* mqtt_server = "192.168.1.10";
char* mqtt_server2 = "192.168.1.229";
bool dest = false;

WiFiClient espClient;
PubSubClient client(espClient);

File myFile;

const int btn = D9;

void callback(char* topic, byte* payload, unsigned int length) {}
 
 
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando...");
    if (client.connect("btn01")) {
      Serial.println(" Conectado!");
      client.subscribe("btn");
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

void setup() {
  pinMode(btn, INPUT);
  Serial.begin(9600);

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

  Serial.println("ssid: " + ssid);
  Serial.println("pass: " + password);
  cambiarDestino();
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  if(digitalRead(btn) == HIGH) {
      Serial.println("Botón pulsado.");
      client.publish("central", "btn01");
      while(digitalRead(btn) != LOW) delay(1);
  }
}
