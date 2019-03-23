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

const int movPin = D3;
bool alarma = false;

void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (int i=0;i<length;i++) mensaje += (char)payload[i];
  if(mensaje=="1") alarma = true;
  else if(mensaje=="0") alarma = false;
}
 
 
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando...");
    if (client.connect("MOV01")) {
      Serial.println(" Conectado!");
      client.subscribe("MOV");
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
  pinMode(movPin, INPUT);
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
  
  if(alarma) {
    if(digitalRead(movPin) == HIGH) {
      Serial.println("Movimiento");
      client.publish("central", "MOV01 movimiento MOV");
      while(digitalRead(movPin) != LOW) delay(1);
      Serial.println("Movimiento cesado");
    }
  }
}
