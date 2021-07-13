#include <Arduino_HTS221.h>

#define DEBUG 1
#if (DEBUG==1)
  #define DEBUG_PRINT(str) Serial.print(str)
  #define DEBUG_PRINTLN(str) Serial.println(str)
#else
  #define DEBUG_PRINT(str)
  #define DEBUG_PRINTLN(str)
#endif

void setup() {
#if (DEBUG == 1)
  Serial.begin(9600);
#endif

  if (!HTS.begin()) DEBUG_PRINTLN("Failed to initialize humidity temperature sensor!");
}

void loop() {
  float temperature = HTS.readTemperature();
  float humidity    = HTS.readHumidity();

  // print each of the sensor values
  DEBUG_PRINT("Temperature = ");
  DEBUG_PRINT(temperature);
  DEBUG_PRINTLN(" °C");

  DEBUG_PRINT("Humidity    = ");
  DEBUG_PRINT(humidity);
  DEBUG_PRINTLN(" %");

  // print an empty line
  DEBUG_PRINTLN();

  // wait 1 second to print again
  delay(1000);
}
