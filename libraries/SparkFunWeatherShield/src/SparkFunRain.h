#pragma once

/*  SparkFun Rain Sensor Library
    By: Nathan Seidle
    Updated: Roger Miranda
    
    Get the current rain. */
    
#include <Arduino.h>

#define RAIN 2
#define QUEUE_SIZE 900 // idealmente habria que guardar unas 30 gotas/segundo; con cola de 900 hay que enviar los datos cada medio segundo para que no haya overflow
#define TIME_INTERVAL 30 // cada 30 segundos la cola se "reinicia"

class Rain {
public:
	Rain::Rain(void);
	
	/* public variables */
	
	/* public functions */
	void Rain::begin(uint8_t port = RAIN); //Set board's pins
	void Rain::update(void);
	float Rain::getRain(); //Returns the rain in mm/h

private:
  /* private variables */
  uint32_t _queue[QUEUE_SIZE]; // FIFO
  volatile uint16_t _queue_ptr_write;
  uint16_t _queue_ptr_read;
  volatile uint16_t _queue_length;
  static Rain *Rain::instance; //Instance used for interrupt; only the newest object will work
  
  /* private functions */
  uint32_t Rain::getLastRain(void);
  static void Rain::rainIRQ();
};