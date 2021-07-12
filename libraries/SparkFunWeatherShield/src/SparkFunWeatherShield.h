#pragma once

/*  SparkFun Weather Shield Library
    By: Roger Miranda
    
    Use all the shield's sensors (wind, rain, light, pressure, temperature and humidity).
    Fully compatible with Arduino UNO Wifi */

// software I2C
#include <Arduino.h>
#include <SoftWire.h>
#include <AsyncDelay.h>

#include "My_SparkFunMPL3115A2.h"
#include "My_SparkFunHTU21D.h"
#include "SparkFunWind.h"
#include "SparkFunRain.h"

// shield pins
#define SHIELD_REFERENCE_3V3 A3
#define SHIELD_LIGHT A1
#define SHIELD_BATT A2 // voltage battery pin
#define SHIELD_LED_BLUE 7
#define SHIELD_LED_GREEN 8

#define SHIELD_SDA_PIN A4
#define SHIELD_SCL_PIN A5

#define MAX_I2C_BUFFER 8
#define ERROR_NOT_BEGIN 1000 // begin() not called

class WeatherShield {

public:
  WeatherShield::WeatherShield(uint8_t sda = SHIELD_SDA_PIN, uint8_t scl = SHIELD_SCL_PIN);
  WeatherShield::~WeatherShield();

  //Public Functions
  void WeatherShield::begin(bool use_leds = false); // start I2C & sensors
  void WeatherShield::loop(void);
  float WeatherShield::readHumidity(); // get sensor's humidity
  float WeatherShield::readTemperature(); // get sensor's temparature
  float WeatherShield::readPressure(); // get sensor's pressure
  float WeatherShield::getBatteryLevel(); //Returns the voltage of the raw pin based on the 3.3V rail
  float WeatherShield::getLightLevel(); //Returns the voltage of the light sensor based on the 3.3V rail
  float WeatherShield::getWindSpeed();
  float WeatherShield::getWindSpeedKm();
  int WeatherShield::getWindDirection();
  int WeatherShield::decodeWindDirection(unsigned int adc);
  float WeatherShield::getRain(); //Returns the rain in mm/s

  //Public Variables

private:
  //Private Variables
  SoftWire *_i2cPort;
  MPL3115A2 *_pressureSensor;
  HTU21D *_humiditySensor;
  Wind *_windSensor;
  Rain *_rainSensor;
  
  byte _swTxBuffer[MAX_I2C_BUFFER];
  byte _swRxBuffer[MAX_I2C_BUFFER];
  
  //Private Functions
};
