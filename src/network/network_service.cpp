#include "network_service.h"

void sendHeartbeat() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  
  // Build heartbeat URL with device ID if available
  String heartbeatURL = "https://unvvzrzcmtsnxounsywc.supabase.co/functions/v1/hello-world?hellofrom=esp32";
  #ifdef DEVICE_ID
    heartbeatURL += "&device=" + String(DEVICE_ID);
  #endif
  
  http.begin(heartbeatURL);
  int code = http.GET();
  Serial.println("Heartbeat: HTTP " + String(code));
  http.end();
}