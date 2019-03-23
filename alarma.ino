#include <Keypad.h>
#include "FPS_GT511C3.h"
#include "TM1637.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SD.h>String ssid = "";
String password = "";
char* mqtt_server = "192.168.1.230";
char* mqtt_server2 = "192.168.1.229";
bool dest = false;
WiFiClient espClient;
PubSubClient client(espClient);
File myFile;
char* ID = "ALAR01";

int pasado = 0;
bool activado = false;

const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 12, 5, 4};
byte colPins[COLS] = {3, 2, 1, 0};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

FPS_GT511C3 fps(10, 11);

#define CLK 8
#define DIO 7
TM1637 tm1637(CLK,DIO);

const int buzzer = D6;

char password[4] = {' ', ' ', ' ', ' '};
bool alarma = false;
bool anadiendo = false;
//a = alarma; d = desactivar alarma; h = huella; m = añadir huella; n = añadir huella (password correcto)
char modo = ' ';
int tiempo = 0;

void setup(){
  Serial.begin(9600);
  //while(!Serial.available()) delay(1);
  
  fps.Open();
  
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);
  
  pinMode(buzzer, OUTPUT);

  conectar();
}
  
void loop(){
  if (!client.connected()) reconnect();
  client.loop();

  if(activado) {
    pasado++;
    if(pasado%100<=50) tone(buzzer, 1000);
    else noTone(buzzer);
  }
  
  char customKey = customKeypad.getKey();
  
  if (customKey) {
    //Serial.println(customKey);
    if(customKey=='*') {
      reset();
      modo = 'h';
      fps.SetLED(true);
    }
    else if(customKey=='#') reset();
    else if(customKey=='A') {
      reset();
      modo = 'a';
      tiempo = 0;
      client.publish("central", (const char*) (String(ID) + " alarma act").c_str());
      for(int x = 0; x<4; x++) {
        tone(buzzer, 1000);
        delay(400);
        noTone(buzzer);
        delay(400);
      }
    }
    else if(customKey=='B') {
      reset();
      modo = 'd';
    }
    else if(customKey=='C');
    else if(customKey=='D') {
      reset();
      modo = 'm';
      fps.SetLED(true);
    }
    else {
      if(modo == 'd' || modo == 'm') {
        for(int x = 0; x!=3; x++) password[x] = password[x+1];
        password[3] = customKey;
        
        //Debug
        Serial.print("Password: ");
        String pass = "";
        for(int x = 0; x!=4; x++) {
          pass+=String(password[x]);
          tm1637.display(x,(int8_t) password[x]);
        }
        Serial.println(pass);

        if(password[x]!=' ') {
          String aci = "deact";
          if(modo=='m') aci = "enr";
          client.publish("central", (const char*) (String(ID) + " alarma "+aci+" "+pass).c_str());
        }
      }
    }
  }

  if(modo == 'a');
  else if(modo == 'h') {
    if (fps.IsPressFinger())
    {
      fps.CaptureFinger(false);
      int id = fps.Identify1_N();
      if (id <200) {
        //Huella válida
        client.publish("central", (const char*) (String(ID) + " alarma deact").c_str());
        modo = ' ';
      }
    }
  }
  else if(modo == 'n') {
    anadiendo = false;
    Enroll();
  }
}

void reset() {
  for(int x = 0; x!=4; x++) {
    password[x] = ' ';
    tm1637.display(x,' ');
  }
  if(modo=='m' || modo=='h') {
    fps.SetLED(false);
  }
}

void Enroll()
{
  if(anadiendo == true) return;
  anadiendo = true;
  // find open enroll id
  int enrollid = 0;
  bool usedid = true;
  while (usedid == true)
  {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid==true) enrollid++;
  }
  fps.EnrollStart(enrollid);

  // enroll
  Serial.print("Press finger to Enroll #");
  for(int x = 0; x!=4; x++) tm1637.display(x,(int8_t) 1);
  Serial.println(enrollid);
  while(fps.IsPressFinger() == false) delay(100);
  bool bret = fps.CaptureFinger(true);
  int iret = 0;
  if (bret != false)
  {
    Serial.println("Remove finger");
    for(int x = 0; x!=4; x++) tm1637.display(x,(int8_t) 0);
    fps.Enroll1(); 
    while(fps.IsPressFinger() == true) delay(100);
    Serial.println("Press same finger again");
    for(int x = 0; x!=4; x++) tm1637.display(x,(int8_t) 2);
    while(fps.IsPressFinger() == false) delay(100);
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      Serial.println("Remove finger");
      for(int x = 0; x!=4; x++) tm1637.display(x,(int8_t) 0);
      fps.Enroll2();
      while(fps.IsPressFinger() == true) delay(100);
      Serial.println("Press same finger yet again");
      for(int x = 0; x!=4; x++) tm1637.display(x,(int8_t) 3);
      while(fps.IsPressFinger() == false) delay(100);
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
        Serial.println("Remove finger");
        for(int x = 0; x!=4; x++) tm1637.display(x,(int8_t) 0);
        iret = fps.Enroll3();
        if (iret == 0)
        {
          Serial.println("Enrolling Successful");
        }
        else
        {
          Serial.print("Enrolling Failed with error code:");
          Serial.println(iret);
          for(int x = 0; x!=4; x++) tm1637.display(x,(int8_t) 9);
        }
      }
      else Serial.println("Failed to capture third finger");
      for(int x = 0; x!=4; x++) tm1637.display(x,(int8_t) 9);
    }
    else Serial.println("Failed to capture second finger");
    for(int x = 0; x!=4; x++) tm1637.display(x,(int8_t) 9);
  }
  else Serial.println("Failed to capture first finger");
  for(int x = 0; x!=4; x++) tm1637.display(x,(int8_t) 9);
}

void activar() {
  activado = true;
}

void desactivar() {
  activado = false;
  for(int x = 0; x<3; x++) {
    tone(buzzer, 1000);
    delay(200);
    noTone(buzzer);
    delay(200);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (int i=0;i<length;i++) mensaje += (char)payload[i];
  if(mensaje=="act") activar();
  else if(mensaje=="deact") desactivar();
  else if(mensaje=="enr") Enroll();
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
