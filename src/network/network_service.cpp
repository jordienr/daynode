#include "network_service.h"

void sendHeartbeat() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin("https://unvvzrzcmtsnxounsywc.supabase.co/functions/v1/hello-world?hellofrom=esp32");
  int code = http.GET();
  Serial.println("Heartbeat: HTTP " + String(code));
  http.end();
}