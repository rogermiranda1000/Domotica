#include "SparkFunRain.h"

Rain *Rain::instance;

Rain::Rain(void) {
	Rain::instance = this;
}

//Set board's pins
void Rain::begin(uint8_t port) {
	pinMode(port, INPUT_PULLUP); // input from wind meters rain gauge sensor
	attachInterrupt(digitalPinToInterrupt(port), Rain::rainIRQ, FALLING);
	
	this->_queue_ptr_write = 0;
	this->_queue_ptr_read = 0;
	this->_queue_length = 0;
}

void Rain::update(void) {
	// check if rain expired
	while (this->_queue_length > 0 && millis() - this->_queue[this->_queue_ptr_read] > TIME_INTERVAL*1000) {
		this->_queue_ptr_read++;
		if (this->_queue_ptr_read == QUEUE_SIZE) this->_queue_ptr_read = 0;
		this->_queue_length--;
	}
}

//Returns the rain in mm/h
float Rain::getRain() {
	return this->_queue_length * (0.2794 * 3600 / TIME_INTERVAL); //Each dump is 0.011" of water; the queue lasts TIME_INTERVAL seconds
}

uint32_t Rain::getLastRain(void) {
	if (this->_queue_length == 0) return millis()-50; // fuerza que el intervalo sea superior a 10
	return this->_queue[(this->_queue_ptr_write > 0) ? (this->_queue_ptr_write - 1) : (QUEUE_SIZE - 1)]; // obten el último tiempo escrito
}

// Count rain gauge bucket tips as they occur
// Activated by the magnet and reed switch in the rain gauge, attached to input D2
void Rain::rainIRQ() {
	unsigned long raintime = millis(); // grab current time
	unsigned long raininterval = raintime - Rain::instance->getLastRain(); // calculate interval between this and last event

	if (raininterval > 10) // ignore switch-bounce glitches less than 10mS after initial edge
	{
		Rain::instance->_queue[Rain::instance->_queue_ptr_write++] = raintime;
		if (Rain::instance->_queue_ptr_write == QUEUE_SIZE) Rain::instance->_queue_ptr_write = 0;
		Rain::instance->_queue_length++;
	}
}