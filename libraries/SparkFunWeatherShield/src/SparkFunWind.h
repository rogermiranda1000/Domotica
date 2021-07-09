#pragma one

/*  SparkFun Wind Sensor Library
    By: Nathan Seidle
    Updated: Roger Miranda
    
    Get speed and direction of the wind. */
    
#include <Arduino.h>

#define WSPEED 3
#define WDIR A0

class Wind {
public:
  Wind::Wind(uint8_t portSpeed = WSPEED, uint8_t portDirection = WDIR);
  
  /* public variables */
  
  /* public functions */
  void Wind::begin(); //Set board's pins
  float Wind::getWindSpeed(); //Returns the speed in MPH
  unsigned int Wind::getWindDirection();
  int Wind::decodeWindDirection(unsigned int adc);

private:
  /* private variables */
  uint8_t _portSpeed;
  uint8_t _portDirection;
  long _lastWindCheck;
  volatile long _lastWindIRQ;
  volatile uint8_t _windClicks;
  static Wind *_instance; //Instance used for interrupt; only the newest object will work
  
  /* private functions */
  static void Wind::wspeedIRQ();
  int Wind::averageAnalogRead(uint8_t pinToRead);
};
