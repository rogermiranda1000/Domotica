#include "SparkFunRain.h"

Rain *Rain::instance;

Rain::Rain(uint8_t port) {
  this->_initTime = millis();
  this->_port = port;
  Rain::instance = this;
}

//Set board's pins
void Rain::begin() {
	pinMode(this->_port, INPUT_PULLUP); // input from wind meters rain gauge sensor
	attachInterrupt(digitalPinToInterrupt(this->_port), Rain::rainIRQ, FALLING);
}

//Returns the rain in inches/second
float Rain::getRain() {
  return this->_rainIn / (millis() - this->_initTime);
}

// Count rain gauge bucket tips as they occur
// Activated by the magnet and reed switch in the rain gauge, attached to input D2
void Rain::rainIRQ() {
  unsigned long raintime = millis(); // grab current time
	unsigned long raininterval = raintime - Rain::instance->_rainLast; // calculate interval between this and last event

	if (raininterval > 10) // ignore switch-bounce glitches less than 10mS after initial edge
	{
		Rain::instance->_rainIn += 0.011; //Each dump is 0.011" of water

		Rain::instance->_rainLast = raintime; // set up for next event
	}
}