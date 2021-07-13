#include "SparkFunWeatherShield.h"

WeatherShield::WeatherShield(uint8_t sda, uint8_t scl) {
  this->_i2cPort = new SoftWire(sda, scl);
  this->_pressureSensor = new MPL3115A2(this->_i2cPort);
  this->_humiditySensor = new HTU21D(this->_i2cPort);
  this->_windSensor = new Wind();
  this->_rainSensor = new Rain();
}

WeatherShield::~WeatherShield() {
  delete this->_pressureSensor;
  delete this->_humiditySensor;
  delete this->_i2cPort;
  delete this->_windSensor;
  delete this->_rainSensor;
}

// start I2C & sensors
void WeatherShield::begin(bool use_leds) {
  // analog inputs
  pinMode(SHIELD_REFERENCE_3V3, INPUT);
  pinMode(SHIELD_LIGHT, INPUT);
  pinMode(SHIELD_BATT, INPUT);
  
  if (use_leds) {
    // board LEDs
    pinMode(SHIELD_LED_BLUE, OUTPUT);
    pinMode(SHIELD_LED_GREEN, OUTPUT);
  }
  
  this->_windSensor->begin();
  this->_rainSensor->begin();
  
  // I2C
  this->_i2cPort->setTxBuffer(this->_swTxBuffer, MAX_I2C_BUFFER);
  this->_i2cPort->setRxBuffer(this->_swRxBuffer, MAX_I2C_BUFFER);
  this->_i2cPort->setDelay_us(4);
  this->_i2cPort->setTimeout(1000);
  this->_i2cPort->begin();
  
  // pressure sensor
  this->_pressureSensor->setModeAltimeter();
  this->_pressureSensor->setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  this->_pressureSensor->setOversampleRate(7); // Set Oversample to the recommended 128
  this->_pressureSensor->enableEventFlags(); // Enable all three pressure and temp event flags
}

void WeatherShield::loop(void) {
	this->_rainSensor->update();
}

// get sensor's humidity
float WeatherShield::readHumidity() {
  if (this->_i2cPort == NULL) return ERROR_NOT_BEGIN;
  return this->_humiditySensor->readHumidity();
}

// get sensor's temparature
float WeatherShield::readTemperature() {
  if (this->_i2cPort == NULL) return ERROR_NOT_BEGIN;
  return this->_humiditySensor->readTemperature();
}

// get sensor's pressure
float WeatherShield::readPressure() {
  if (this->_i2cPort == NULL) return ERROR_NOT_BEGIN;
  return this->_pressureSensor->readPressure();
}

float WeatherShield::getWindSpeed() {
  return this->_windSensor->getWindSpeed();
}

float WeatherShield::getWindSpeedKm() {
	return this->getWindSpeed()*1.609344;
}

int WeatherShield::getWindDirection() {
  return this->_windSensor->getWindDirection();
}

int WeatherShield::decodeWindDirection(unsigned int adc) {
  return this->_windSensor->decodeWindDirection(adc);
}

float WeatherShield::getRain() {
  return this->_rainSensor->getRain();
}

//Returns the voltage of the light sensor based on the 3.3V rail
float WeatherShield::getLightLevel() {
  float operatingVoltage = analogRead(SHIELD_REFERENCE_3V3);

  float lightSensor = analogRead(SHIELD_LIGHT);

  return 100*(lightSensor/operatingVoltage);
}

//Returns the voltage of the raw pin based on the 3.3V rail
//Battery level is connected to the RAW pin on Arduino and is fed through two 5% resistors:
//3.9K on the high side (R1), and 1K on the low side (R2)
float WeatherShield::getBatteryLevel() {
  float operatingVoltage = analogRead(SHIELD_REFERENCE_3V3);

  float rawVoltage = analogRead(SHIELD_BATT);

  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V

  rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin

  rawVoltage *= 4.90; //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage

  return rawVoltage;
}
