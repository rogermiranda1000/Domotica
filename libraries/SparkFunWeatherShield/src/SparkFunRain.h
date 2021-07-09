#pragma once

/*  SparkFun Rain Sensor Library
    By: Nathan Seidle
    Updated: Roger Miranda
    
    Get the current rain. */
    
#include <Arduino.h>

#define RAIN 2

class Rain {
public:
  Rain::Rain(uint8_t port = RAIN);
  
  /* public variables */
  
  /* public functions */
  void Rain::begin(); //Set board's pins
  float Rain::getRain(); //Returns the rain in inches/second

private:
  /* private variables */
  uint8_t _port;
  volatile unsigned long _rainLast;
  volatile unsigned long _initTime;
  volatile float _rainIn;
  static Rain *Rain::instance; //Instance used for interrupt; only the newest object will work
  
  /* private functions */
  static void Rain::rainIRQ();
};