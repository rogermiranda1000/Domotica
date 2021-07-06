#ifndef _DomoticConnector_h_
#define _DomoticConnector_h_

#include "Arduino.h"
#include <WiFiNINA.h>
//#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SD.h>

#define SD_PIN D2

class DomoticConnector {
  public:
    DomoticConnector();
  private:
    byte _pin;
};

#endif
