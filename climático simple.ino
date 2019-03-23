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

int retraso = 1000;
int pasado = 0;
char* ID = "CLIM01";

const int DHTPIN = 5;
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const int pResistor = A0;

void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (int i=0;i<length;i++) mensaje += (char)payload[i];
  Serial.println("Retraso de " + mensaje + " segundos.");
  retraso = mensaje.toInt();
  retraso *= 1000;
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

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(pResistor, INPUT);

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

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // Leemos la humedad relativa
  float h = dht.readHumidity();
  // Leemos la temperatura en grados centígrados (por defecto)
  float t = dht.readTemperature();
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(h) || isnan(t)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
  Serial.println(String(h) + "%");
  Serial.println(String(t) + "ºC");
  
  int value = analogRead(pResistor);
  double per = (double) value/1024;
  per *= 100;
  Serial.print(String(value) + " - ");
  Serial.print(per, 2);
  Serial.println("% luz");
  Serial.println("---");
  
  pasado += 1000;
  if(pasado >= retraso) {
    client.publish("central", (const char*) (String(ID) + " luz " + String(per)).c_str());
    client.publish("central", (const char*) (String(ID) + " temperatura " + String(t)).c_str());
    client.publish("central", (const char*) (String(ID) + " humedad " + String(h)).c_str());
    pasado = 0;
  }
  
  //client.publish("central", "MOV");

  delay(500);
}
