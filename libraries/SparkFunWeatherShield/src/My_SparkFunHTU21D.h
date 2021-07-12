#pragma once

/*  HTU21D Humidity Sensor Library-Based Library
    Original: Nathan Seidle
    Updated: Roger Miranda
    
    Get humidity and temperature from the HTU21D sensor. */
    
#include <Arduino.h>
#include <SoftWire.h>
#include <AsyncDelay.h>


#define MAX_WAIT 100
#define DELAY_INTERVAL 10
#define MAX_COUNTER (MAX_WAIT/DELAY_INTERVAL)

#define HTU21D_ADDRESS 0x40  //Unshifted 7-bit I2C address for the sensor

#define ERROR_I2C_TIMEOUT 	998
#define ERROR_BAD_CRC		    999

#define TRIGGER_TEMP_MEASURE_HOLD  0xE3
#define TRIGGER_HUMD_MEASURE_HOLD  0xE5
#define TRIGGER_TEMP_MEASURE_NOHOLD  0xF3
#define TRIGGER_HUMD_MEASURE_NOHOLD  0xF5
#define WRITE_USER_REG  0xE6
#define READ_USER_REG  0xE7
#define SOFT_RESET  0xFE

#define USER_REGISTER_RESOLUTION_MASK 0x81
#define USER_REGISTER_RESOLUTION_RH12_TEMP14 0x00
#define USER_REGISTER_RESOLUTION_RH8_TEMP12 0x01
#define USER_REGISTER_RESOLUTION_RH10_TEMP13 0x80
#define USER_REGISTER_RESOLUTION_RH11_TEMP11 0x81

#define USER_REGISTER_END_OF_BATTERY 0x40
#define USER_REGISTER_HEATER_ENABLED 0x04
#define USER_REGISTER_DISABLE_OTP_RELOAD 0x02

//CRC POLYNOMIAL = 0x0131 = x^8 + x^5 + x^4 + 1 : http://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
#define SHIFTED_DIVISOR 0x988000 //This is the 0x0131 polynomial shifted to farthest left of three bytes


class HTU21D {
public:
  /* public variables */
  
  /* public functions */
  HTU21D(SoftWire *i2c);
  
  // given a command, reads a given 2-byte value with CRC from the HTU21D
  float readHumidity(void);
  float readTemperature(void);
  void setResolution(byte resBits);

  byte readUserRegister(void);
  void writeUserRegister(byte val);

private:
  /* private variables */
  SoftWire *_i2cPort;
  
  /* private functions */
  byte checkCRC(uint16_t message_from_sensor, uint8_t check_value_from_sensor);
  uint16_t readValue(byte cmd);
};