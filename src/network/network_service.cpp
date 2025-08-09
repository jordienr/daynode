#include "network_service.h"

void logDeviceEvent() {
  if (WiFi.status() != WL_CONNECTED) return;

  // Get device ID
  String deviceId;
  #ifdef DEVICE_ID
    deviceId = String(DEVICE_ID);
  #else
    deviceId = "NOD1";  // Default device ID
  #endif
  
  // Validate device ID
  if (deviceId.length() == 0) {
    ERR("Device ID is empty - skipping event logging");
    return;
  }
  
  // Get temperature reading
  float temperature = getTemperatureC();
  
  // Validate temperature
  if (temperature == DEVICE_DISCONNECTED_C) {
    ERR("Temperature sensor disconnected - skipping event logging");
    return;
  }
  
  // Check for valid temperature range (-55°C to +125°C for DS18B20)
  if (temperature < -55.0 || temperature > 125.0) {
    ERR("Temperature reading out of valid range: " + String(temperature, 2) + "°C - skipping event logging");
    return;
  }

  HTTPClient http;
  http.begin("https://unvvzrzcmtsnxounsywc.supabase.co/functions/v1/log-device-event");
  
  // Set headers
  http.addHeader("Content-Type", "application/json");
  
  #ifdef API_TOKEN
    http.addHeader("Authorization", "Bearer " + String(API_TOKEN));
    LOG("Using API token from secrets");
  #else
    LOG("No API token configured - request may fail");
  #endif
  
  // Build JSON payload
  String payload = "{";
  payload += "\"device_id\": \"" + deviceId + "\"";
  payload += ", \"temp_c\": " + String(temperature, 2);
  payload += "}";
  
  LOG("Temperature reading: " + String(temperature, 2) + "°C");
  LOG("Sending device event: " + payload);
  
  int code = http.POST(payload);
  Serial.println("Device Event: HTTP " + String(code));
  
  // Log response if available
  if (code > 0) {
    String response = http.getString();
    if (response.length() > 0) {
      LOG("Response: " + response);
    }
  }
  
  http.end();
}