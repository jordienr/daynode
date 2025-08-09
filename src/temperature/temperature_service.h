#ifndef TEMPERATURE_SERVICE_H
#define TEMPERATURE_SERVICE_H

#include "../core/config.h"

// Include required libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#define TEMPERATURE_SENSOR_AVAILABLE

// DS18B20 Configuration
#define ONE_WIRE_BUS 4  // GPIO4 for DS18B20 data line
#define TEMPERATURE_PRECISION 12  // 12-bit precision for DS18B20
#define DEVICE_DISCONNECTED_C -127.0  // Error value when sensor disconnected

// Function declarations
void initTemperatureService();
float getTemperatureC();
bool isTemperatureSensorConnected();

#endif // TEMPERATURE_SERVICE_H
