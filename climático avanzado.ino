//#include <DHT.h>

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
char* ID = "CLIM02";

const int luz = D0;
const int agua = D1;
const int sensores = A0;

//#define DHTPIN D4
//#define DHTTYPE DHT11
//DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  pinMode(luz, OUTPUT);
  pinMode(agua, OUTPUT);
  pinMode(sensores, INPUT);
  //dht.begin();
  
  conectar();
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  
  /*float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.println("Humedad: " + String(h) + "%");
  Serial.println("Temperatura: " + String(t) + "ºC");*/
  
  //lectura nivel del agua
  digitalWrite(agua, HIGH);
  delay(400);
  int a = analogRead(sensores);
  Serial.println("Agua: " + String(a));
  delay(100);
  digitalWrite(agua, LOW);

  //lectura luz
  digitalWrite(luz, HIGH);
  delay(400);
  float l = analogRead(sensores);
  l /= (float) 1023;
  l *= (float) 100;
  Serial.println("Luz: " + String(l) + "%");
  delay(100);
  digitalWrite(luz, LOW);

  pasado += 1000;
  if(pasado >= retraso) {
    client.publish("central", (const char*) (String(ID) + " luz " + String(l)).c_str());
    client.publish("central", (const char*) (String(ID) + " agua " + String(a)).c_str());
    pasado = 0;
  }
}




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
