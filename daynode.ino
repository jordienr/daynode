// Daynode - Modular ESP32 WiFi Configuration
#include "src/core/config.h"
#include "src/web/web_interface.h"
#include "src/wifi/wifi_manager.h"
#include "src/network/network_service.h"
#include "src/temperature/temperature_service.h"

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(1000); // Give more time for serial to initialize

  // Setup hardware reset button
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

  LOG("Boot complete");
  LOG("Free heap: " + String(ESP.getFreeHeap()) + " bytes");
  LOG("Hardware reset: Hold BOOT button for 5+ seconds");

  // Initialize preferences
  prefs.begin("wifi", false);
  prefs.end();

  // Initialize temperature service
  LOG("About to initialize temperature service...");
  initTemperatureService();
  LOG("Temperature service initialization completed.");

  connectToStoredWiFi();
  if (WiFi.status() != WL_CONNECTED) {
    setupAP();
  }
}

void loop() {
  // Monitor hardware reset button
  bool buttonCurrentlyPressed = digitalRead(RESET_BUTTON_PIN) == LOW;
  
  if (buttonCurrentlyPressed && !resetButtonPressed) {
    // Button just pressed
    resetButtonPressed = true;
    resetButtonPressStart = millis();
    LOG("Reset button pressed - hold for 5 seconds to reset WiFi");
  } else if (!buttonCurrentlyPressed && resetButtonPressed) {
    // Button released
    resetButtonPressed = false;
    LOG("Reset button released");
  } else if (buttonCurrentlyPressed && resetButtonPressed) {
    // Button is being held
    unsigned long holdDuration = millis() - resetButtonPressStart;
    if (holdDuration >= RESET_HOLD_TIME) {
      // Button held for required time - trigger reset
      performHardwareReset();
      // performHardwareReset() calls ESP.restart(), so code won't reach here
    }
  }

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
    logDeviceEvent();
  }
}
