#include "My_SparkFunMPL3115A2.h"

MPL3115A2::MPL3115A2(SoftWire *i2c) {
  this->_i2cPort = i2c;
}


//Returns the number of meters above sea level
//Returns -1 if no new data is available
float MPL3115A2::readAltitude()
{
	toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

	//Wait for PDR bit, indicates we have new pressure data
	int counter = 0;
	while( (IIC_Read(STATUS) & (1<<1)) == 0)
	{
		if(++counter > 600) return ERROR_I2C_TIMEOUT; //Error out after max of 512ms for a read
		delay(1);
	}

	// Read pressure registers
	this->_i2cPort->beginTransmission(MPL3115A2_ADDRESS);
	this->_i2cPort->write(OUT_P_MSB);  // Address of data to get
	this->_i2cPort->endTransmission(false); // Send data to I2C dev with option for a repeated start. THIS IS NECESSARY and not supported before Arduino V1.0.1!
	if (this->_i2cPort->requestFrom(MPL3115A2_ADDRESS, 3) != 3) return ERROR_I2C_TIMEOUT;

	byte msb, csb, lsb;
	msb = this->_i2cPort->read();
	csb = this->_i2cPort->read();
	lsb = this->_i2cPort->read();

	// The least significant bytes l_altitude and l_temp are 4-bit,
	// fractional values, so you must cast the calulation in (float),
	// shift the value over 4 spots to the right and divide by 16 (since 
	// there are 16 values in 4-bits). 
	float tempcsb = (lsb>>4)/16.0;

	float altitude = (float)( (msb << 8) | csb) + tempcsb;

	return(altitude);
}

//Returns the number of feet above sea level
float MPL3115A2::readAltitudeFt()
{
  return(readAltitude() * 3.28084);
}

//Reads the current pressure in hPa
//Unit must be set in barometric pressure mode
//Returns -1 if no new data is available
float MPL3115A2::readPressure()
{
	//Check PDR bit, if it's not set then toggle OST
	if(IIC_Read(STATUS) & (1<<2) == 0) toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

	//Wait for PDR bit, indicates we have new pressure data
	int counter = 0;
	while(IIC_Read(STATUS) & (1<<2) == 0)
	{
		if(++counter > 600) return ERROR_I2C_TIMEOUT; //Error out after max of 512ms for a read
		delay(1);
	}

	// Read pressure registers
	this->_i2cPort->beginTransmission(MPL3115A2_ADDRESS);
	this->_i2cPort->write(OUT_P_MSB);  // Address of data to get
	this->_i2cPort->endTransmission(false); // Send data to I2C dev with option for a repeated start. THIS IS NECESSARY and not supported before Arduino V1.0.1!
	if (this->_i2cPort->requestFrom(MPL3115A2_ADDRESS, 3) != 3) return ERROR_I2C_TIMEOUT;

	byte msb, csb, lsb;
	msb = this->_i2cPort->read();
	csb = this->_i2cPort->read();
	lsb = this->_i2cPort->read();
	
	toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

	// Pressure comes back as a left shifted 20 bit number
	long pressure_whole = (long)msb<<16 | (long)csb<<8 | (long)lsb;
	pressure_whole >>= 6; //Pressure is an 18 bit number with 2 bits of decimal. Get rid of decimal portion.

	lsb &= B00110000; //Bits 5/4 represent the fractional component
	lsb >>= 4; //Get it right aligned
	float pressure_decimal = (float)lsb/4.0; //Turn it into fraction

	float pressure = (float)pressure_whole + pressure_decimal;

	return (pressure/100);
}

float MPL3115A2::readTemp()
{
	if(IIC_Read(STATUS) & (1<<1) == 0) toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

	//Wait for TDR bit, indicates we have new temp data
	int counter = 0;
	while( (IIC_Read(STATUS) & (1<<1)) == 0)
	{
		if(++counter > 600) return ERROR_I2C_TIMEOUT; //Error out after max of 512ms for a read
		delay(1);
	}

	// Read temperature registers
	this->_i2cPort->beginTransmission(MPL3115A2_ADDRESS);
	this->_i2cPort->write(OUT_T_MSB);  // Address of data to get
	this->_i2cPort->endTransmission(false); // Send data to I2C dev with option for a repeated start. THIS IS NECESSARY and not supported before Arduino V1.0.1!
	if (this->_i2cPort->requestFrom(MPL3115A2_ADDRESS, 2) != 2) return ERROR_I2C_TIMEOUT;

	byte msb, lsb;
	msb = this->_i2cPort->read();
	lsb = this->_i2cPort->read();

	toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

    //Negative temperature fix by D.D.G.
	word foo = 0;
    bool negSign = false;

    //Check for 2s compliment
	if(msb > 0x7F)
	{
        foo = ~((msb << 8) + lsb) + 1;  //2’s complement
        msb = foo >> 8;
        lsb = foo & 0x00F0; 
        negSign = true;
	}

	// The least significant bytes l_altitude and l_temp are 4-bit,
	// fractional values, so you must cast the calulation in (float),
	// shift the value over 4 spots to the right and divide by 16 (since 
	// there are 16 values in 4-bits). 
	float templsb = (lsb>>4)/16.0; //temp, fraction of a degree

	float temperature = (float)(msb + templsb);

	if (negSign) temperature = 0 - temperature;
	
	return(temperature);
}

//Give me temperature in fahrenheit!
float MPL3115A2::readTempF()
{
  return((readTemp() * 9.0)/ 5.0 + 32.0); // Convert celsius to fahrenheit
}

//Sets the mode to Barometer
//CTRL_REG1, ALT bit
void MPL3115A2::setModeBarometer()
{
  byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
  tempSetting &= ~(1<<7); //Clear ALT bit
  IIC_Write(CTRL_REG1, tempSetting);
}

//Sets the mode to Altimeter
//CTRL_REG1, ALT bit
void MPL3115A2::setModeAltimeter()
{
  byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
  tempSetting |= (1<<7); //Set ALT bit
  IIC_Write(CTRL_REG1, tempSetting);
}

//Puts the sensor in standby mode
//This is needed so that we can modify the major control registers
void MPL3115A2::setModeStandby()
{
  byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
  tempSetting &= ~(1<<0); //Clear SBYB bit for Standby mode
  IIC_Write(CTRL_REG1, tempSetting);
}

//Puts the sensor in active mode
//This is needed so that we can modify the major control registers
void MPL3115A2::setModeActive()
{
  byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
  tempSetting |= (1<<0); //Set SBYB bit for Active mode
  IIC_Write(CTRL_REG1, tempSetting);
}

//Call with a rate from 0 to 7. See page 33 for table of ratios.
//Sets the over sample rate. Datasheet calls for 128 but you can set it 
//from 1 to 128 samples. The higher the oversample rate the greater
//the time between data samples.
void MPL3115A2::setOversampleRate(byte sampleRate)
{
  if(sampleRate > 7) sampleRate = 7; //OS cannot be larger than 0b.0111
  sampleRate <<= 3; //Align it for the CTRL_REG1 register
  
  byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
  tempSetting &= B11000111; //Clear out old OS bits
  tempSetting |= sampleRate; //Mask in new OS bits
  IIC_Write(CTRL_REG1, tempSetting);
}

//Enables the pressure and temp measurement event flags so that we can
//test against them. This is recommended in datasheet during setup.
void MPL3115A2::enableEventFlags()
{
  IIC_Write(PT_DATA_CFG, 0x07); // Enable all three pressure and temp event flags 
}

//Clears then sets the OST bit which causes the sensor to immediately take another reading
//Needed to sample faster than 1Hz
void MPL3115A2::toggleOneShot(void)
{
  byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
  tempSetting &= ~(1<<1); //Clear OST bit
  IIC_Write(CTRL_REG1, tempSetting);

  tempSetting = IIC_Read(CTRL_REG1); //Read current settings to be safe
  tempSetting |= (1<<1); //Set OST bit
  IIC_Write(CTRL_REG1, tempSetting);
}


// These are the two I2C functions in this sketch.
byte MPL3115A2::IIC_Read(byte regAddr)
{
  // This function reads one byte over IIC
  this->_i2cPort->beginTransmission(MPL3115A2_ADDRESS);
  this->_i2cPort->write(regAddr);  // Address of CTRL_REG1
  this->_i2cPort->endTransmission(false); // Send data to I2C dev with option for a repeated start. THIS IS NECESSARY and not supported before Arduino V1.0.1!
  this->_i2cPort->requestFrom(MPL3115A2_ADDRESS, 1); // Request the data...
  return this->_i2cPort->read();
}

void MPL3115A2::IIC_Write(byte regAddr, byte value)
{
  // This function writes one byto over IIC
  this->_i2cPort->beginTransmission(MPL3115A2_ADDRESS);
  this->_i2cPort->write(regAddr);
  this->_i2cPort->write(value);
  this->_i2cPort->endTransmission(true);
}
