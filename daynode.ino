// DayNode - Modular ESP32 WiFi Configuration
#include "src/core/config.h"
#include "src/web/web_interface.h"
#include "src/wifi/wifi_manager.h"
#include "src/network/network_service.h"

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(1000); // Give more time for serial to initialize

  LOG("Boot complete");
  LOG("Free heap: " + String(ESP.getFreeHeap()) + " bytes");

  // Initialize preferences
  prefs.begin("wifi", false);
  prefs.end();

  connectToStoredWiFi();
  if (WiFi.status() != WL_CONNECTED) {
    setupAP();
  }
}

void loop() {
  if (WiFi.getMode() == WIFI_AP) {
    // Process DNS requests more frequently
    dnsServer.processNextRequest();
    server.handleClient();
    
    // Add small delay to prevent watchdog issues
    delay(10);
  }

  if (shouldConnectToWiFi) {
    shouldConnectToWiFi = false;
    connectToStoredWiFi();
  }

  if (WiFi.status() == WL_CONNECTED && millis() - lastPing > 5000) {
    lastPing = millis();
    sendHeartbeat();
  }
}
