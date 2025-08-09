#include "temperature_service.h"

// OneWire and Dallas Temperature objects
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Global state
bool temperatureSensorInitialized = false;
bool temperatureSensorConnected = false;

void initTemperatureService() {
  LOG("Initializing DS18B20 temperature service...");
  LOG("OneWire bus pin: GPIO" + String(ONE_WIRE_BUS));
  
  // Initialize the DS18B20 sensor
  sensors.begin();
  
  // Set precision to 12-bit (0.0625°C resolution)
  sensors.setResolution(TEMPERATURE_PRECISION);
  
  // Check if sensor is connected
  sensors.requestTemperatures();
  delay(100); // Give sensor time to respond
  
  float testTemp = sensors.getTempCByIndex(0);
  
  if (testTemp != DEVICE_DISCONNECTED_C) {
    temperatureSensorConnected = true;
    temperatureSensorInitialized = true;
    LOG("DS18B20 temperature sensor initialized successfully");
    LOG("Sensor precision: " + String(TEMPERATURE_PRECISION) + "-bit");
    LOG("Initial temperature reading: " + String(testTemp, 2) + "°C");
  } else {
    temperatureSensorConnected = false;
    temperatureSensorInitialized = true;
    ERR("DS18B20 temperature sensor not detected!");
    ERR("Check wiring: Data pin to GPIO" + String(ONE_WIRE_BUS) + ", VCC to 3.3V, GND to GND");
    ERR("Ensure 4.7kΩ pull-up resistor is connected between data and VCC");
  }
}

float getTemperatureC() {
  if (!temperatureSensorInitialized) {
    ERR("Temperature service not initialized. Call initTemperatureService() first.");
    return DEVICE_DISCONNECTED_C;
  }
  
  if (!temperatureSensorConnected) {
    ERR("DS18B20 sensor not connected");
    return DEVICE_DISCONNECTED_C;
  }
  
  LOG("Requesting temperature reading from DS18B20...");
  
  // Request temperature conversion
  sensors.requestTemperatures();
  
  // Get temperature in Celsius
  float temperatureC = sensors.getTempCByIndex(0);
  
  // Validate reading
  if (temperatureC == DEVICE_DISCONNECTED_C) {
    ERR("Failed to read temperature - sensor disconnected");
    temperatureSensorConnected = false;
    return DEVICE_DISCONNECTED_C;
  }
  
  // Check for valid temperature range (-55°C to +125°C for DS18B20)
  if (temperatureC < -55.0 || temperatureC > 125.0) {
    ERR("Temperature reading out of valid range: " + String(temperatureC, 2) + "°C");
    return DEVICE_DISCONNECTED_C;
  }
  
  LOG("Temperature reading: " + String(temperatureC, 2) + "°C");
  return temperatureC;
}

bool isTemperatureSensorConnected() {
  if (!temperatureSensorInitialized) {
    return false;
  }
  
  // Perform a quick connectivity check
  sensors.requestTemperatures();
  delay(50); // Short delay for quick check
  float testTemp = sensors.getTempCByIndex(0);
  
  bool connected = (testTemp != DEVICE_DISCONNECTED_C);
  temperatureSensorConnected = connected;
  
  return connected;
}
