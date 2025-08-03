#include "wifi_manager.h"
#include "../web/web_interface.h"

void setupAP() {
  LOG("Starting AP mode...");
  
  // Enable Station + AP mode to allow scanning while serving AP
  WiFi.mode(WIFI_AP_STA);
  
  // Perform a quick scan for nearby networks before starting AP
  LOG("Scanning for WiFi networks...");
  int n = WiFi.scanNetworks();
  networkCount = min(n, 8);
  for (int i = 0; i < networkCount; i++) {
    networks[i] = WiFi.SSID(i);
    LOG("Found SSID: " + networks[i]);
  }

  // Switch to AP-only mode so client devices know the network has no Internet
  WiFi.mode(WIFI_AP);

  // Now start the SoftAP
  WiFi.softAP(apSSID);
  Serial.println("AP IP: " + WiFi.softAPIP().toString());

  LOG("Starting DNS redirector");
  dnsServer.start(53, "*", WiFi.softAPIP());
  LOG("DNS server started");

  // Main page route
  server.on("/", showMainPage);
  server.on("/connect", HTTP_POST, handleConnect);
  
  // Test route
  server.on("/test", []() {
    LOG("Test route hit");
    server.send(200, "text/plain", "ESP32 Server Working!");
  });

  // Captive portal handlers
  server.on("/generate_204", []() {
    LOG("Handled /generate_204");
    server.send(200, "text/html", "<html><body>Redirecting...<script>location.href='/'</script></body></html>");
  });

  server.on("/hotspot-detect.html", []() {
    LOG("Handled /hotspot-detect.html");
    server.send(200, "text/html", "<html><body><h2>Captive Portal Active</h2><script>location.href='/'</script></body></html>");
  });

  server.on("/ncsi.txt", []() {
    LOG("Handled /ncsi.txt");
    server.send(200, "text/plain", "Microsoft NCSI");
  });

  server.on("/connecttest.txt", []() {
    LOG("Handled /connecttest.txt");
    server.send(200, "text/plain", "");
  });

  server.on("/fwlink", []() {
    LOG("Handled /fwlink");
    server.send(200, "text/html", "<html><body><script>location.href='/'</script></body></html>");
  });

  // Additional captive portal handlers for better compatibility
  server.on("/library/test/success.html", []() {
    LOG("Handled /library/test/success.html");
    server.send(200, "text/html", "<html><body><script>location.href='/'</script></body></html>");
  });

  server.on("/success.txt", []() {
    LOG("Handled /success.txt");
    server.send(200, "text/plain", "success");
  });

  server.onNotFound([]() {
    LOG("NotFound triggered, sending /");
    showMainPage();
  });

  server.begin();
  LOG("Web server started");
}

void connectToStoredWiFi() {
  LOG("Attempting to connect to stored WiFi...");
  prefs.begin("wifi", true);
  String ssid = prefs.getString("ssid", "");
  String pass = prefs.getString("pass", "");
  prefs.end();

  if (ssid == "") {
    LOG("No stored SSID found");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());

  Serial.print("Connecting to ");
  Serial.println(ssid);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
    LOG("WiFi connected successfully");
  } else {
    Serial.println("\nFailed to connect.");
    ERR("Failed to connect to WiFi. Falling back to AP");
    setupAP();  // fallback
  }
}