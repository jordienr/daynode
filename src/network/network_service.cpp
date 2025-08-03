#include "network_service.h"

void logDeviceEvent() {
  if (WiFi.status() != WL_CONNECTED) return;

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
  payload += "\"device_id\": \"";
  
  #ifdef DEVICE_ID
    payload += String(DEVICE_ID);
  #else
    payload += "NOD1";  // Default device ID
  #endif
  
  payload += "\", \"measure_1\": ";
  payload += String(random(0, 101));  // Random int 0-100
  payload += "}";
  
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