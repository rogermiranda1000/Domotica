#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>

#define DEBUG 1
#if (DEBUG==1)
  #define DEBUG_PRINT(str) Serial.print(str)
  #define DEBUG_PRINTLN(str) Serial.println(str)
#else
  #define DEBUG_PRINT(str)
  #define DEBUG_PRINTLN(str)
#endif

bool temp = true, pressure = true;
void setup() {
#if (DEBUG == 1)
  Serial.begin(9600);
#endif

  if (!HTS.begin()) {
    temp = false;
    DEBUG_PRINTLN("Failed to initialize humidity temperature sensor!");
  }
  if (!BARO.begin()) {
    pressure = false;
    DEBUG_PRINTLN("Failed to initialize pressure sensor!");
  }
}

void loop() {
  if (temp) {
    float temperature = HTS.readTemperature();
    float humidity    = HTS.readHumidity();
    
    DEBUG_PRINT("Temperature = ");
    DEBUG_PRINT(temperature);
    DEBUG_PRINTLN(" °C");
  
    DEBUG_PRINT("Humidity    = ");
    DEBUG_PRINT(humidity);
    DEBUG_PRINTLN(" %");
  }

  if (pressure) {
    float pressure = BARO.readPressure()*10; // pressure in kPa, but expecting hPa
    
    DEBUG_PRINT("Pressure = ");
    DEBUG_PRINT(pressure);
    DEBUG_PRINTLN("hPa");
  }

  // print an empty line
  DEBUG_PRINTLN();

  // wait 1 second to print again
  delay(1000);
}
