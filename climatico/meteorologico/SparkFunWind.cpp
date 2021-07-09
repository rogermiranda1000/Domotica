#include "SparkFunWind.h"

Wind *Wind::_instance;

Wind::Wind(uint8_t portSpeed, uint8_t portDirection) {
  Wind::_instance = this;
  
  this->_portSpeed = portSpeed;
  this->_portDirection = portDirection;
	
	this->_lastWindCheck = 0;
	this->_lastWindIRQ = 0;
	this->_windClicks = 0;
}

//Set board's pins
void Wind::begin() {
	pinMode(this->_portSpeed, INPUT_PULLUP);
	pinMode(this->_portDirection, INPUT);
	attachInterrupt(digitalPinToInterrupt(this->_portSpeed), Wind::wspeedIRQ, FALLING);
}

//Returns the speed in MPH
float Wind::getWindSpeed() {
	float deltaTime = millis() - this->_lastWindCheck;

	deltaTime /= 1000.0; //Covert to seconds

	float windSpeed = (float)this->_windClicks / deltaTime;

	this->_windClicks = 0; //Reset and start watching for new wind
	this->_lastWindCheck = millis();

	windSpeed *= 1.492;

	return(windSpeed);
}

// Activated by the magnet in the anemometer (2 ticks per rotation)
static void Wind::wspeedIRQ() {
	if (millis() - Wind::_instance->_lastWindIRQ > 10) // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
	{
		Wind::_instance->_lastWindIRQ = millis(); //Grab the current time
		Wind::_instance->_windClicks++; //There is 1.492MPH for each click per second.
	}
}

// read the wind direction sensor, return heading in degrees
unsigned int Wind::getWindDirection() {
	return averageAnalogRead(this->_portDirection); // get the current reading from the sensor
}

int Wind::decodeWindDirection(unsigned int adc) {
 // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
  // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
  // Note that these are not in compass degree order! See Weather Meters datasheet for more information.

  if (adc < 380) return (113);
  if (adc < 393) return (68);
  if (adc < 414) return (90);
  if (adc < 456) return (158);
  if (adc < 508) return (135);
  if (adc < 551) return (203);
  if (adc < 615) return (180);
  if (adc < 680) return (23);
  if (adc < 746) return (45);
  if (adc < 801) return (248);
  if (adc < 833) return (225);
  if (adc < 878) return (338);
  if (adc < 913) return (0);
  if (adc < 940) return (293);
  if (adc < 967) return (315);
  if (adc < 990) return (270);
  
  return (-1); // error, disconnected?
}

//Takes an average of readings on a given pin
//Returns the average
int Wind::averageAnalogRead(uint8_t pinToRead) {
	byte numberOfReadings = 8;
	unsigned int runningValue = 0;

	for(int x = 0 ; x < numberOfReadings ; x++) runningValue += analogRead(pinToRead);
	runningValue /= numberOfReadings;

	return(runningValue);
}
