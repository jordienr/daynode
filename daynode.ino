#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <HTTPClient.h>

Preferences prefs;
WebServer server(80);
DNSServer dnsServer;

const char* apSSID = "ESP32_1";

bool shouldConnectToWiFi = false;
String selectedSSID;
String enteredPassword;

String networks[8];
int networkCount = 0;

#define LOG(msg) Serial.println(String("[LOG] ") + msg)
#define ERR(msg) Serial.println(String("[ERROR] ") + msg)

void showMainPage() {
  LOG("Serving main page");
  
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    body { font-family: sans-serif; padding: 1em; max-width: 600px; margin: 0 auto; }
    h2 { font-size: 1.5em; margin-bottom: 1em; }
    ul { list-style: none; padding: 0; }
    li { margin: 0.5em 0; }
    a { display: block; padding: 1em; background: #007bff; border-radius: 8px; text-decoration: none; color: white; font-size: 1.2em; text-align: center; }
    a:hover { background: #0056b3; }
    .form-container { margin-top: 1em; display: none; padding: 1em; border: 1px solid #ddd; border-radius: 8px; }
    input, button { font-size: 1.2em; padding: 1em; width: 100%; margin-top: 1em; box-sizing: border-box; }
    input { border: 1px solid #ddd; border-radius: 4px; }
    button { background: #28a745; color: white; border: none; border-radius: 4px; cursor: pointer; }
    button:hover { background: #218838; }
    .status { padding: 1em; margin: 1em 0; border-radius: 4px; display: none; }
    .status.success { background: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
    .status.error { background: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
    .back-btn { background: #6c757d; margin-bottom: 1em; }
    .back-btn:hover { background: #545b62; }
  </style>
</head>
<body>
  <div id="wifiList">
    <h2>Select WiFi Network</h2>
    <ul id="networksList">
)rawliteral";

  if (networkCount == 0) {
    html += "<li>No networks found. You can still enter SSID manually below.</li>";
  }

  // Add networks to HTML (limit to 3 to save memory)
  int count = min(networkCount, 3);
  for (int i = 0; i < count; i++) {
    String ssid = networks[i];
    // Escape quotes in SSID to prevent HTML issues
    ssid.replace("\"", "&quot;");
    html += "<li><a href='#' onclick=\"showForm('" + ssid + "')\">" + ssid + "</a></li>";
  }

  html += R"rawliteral(
    </ul>
  </div>

  <div id="passwordForm" class="form-container">
    <button class="back-btn" onclick="showWiFiList()">← Back to Networks</button>
    <h2 id="formTitle">Enter Password</h2>
    <form id="wifiForm" method="POST" action="/connect">
      <input type="hidden" name="ssid" id="ssidInput">
      <input type="password" name="password" id="passwordInput" placeholder="Enter WiFi password" autofocus>
      <button type="submit">Connect</button>
    </form>
  </div>

  <div id="status" class="status"></div>

  <script>
    function showForm(ssid) {
      document.getElementById('ssidInput').value = ssid;
      document.getElementById('formTitle').textContent = 'Enter password for ' + ssid;
      document.getElementById('wifiList').style.display = 'none';
      document.getElementById('passwordForm').style.display = 'block';
      document.getElementById('passwordInput').focus();
    }

    function showWiFiList() {
      document.getElementById('wifiList').style.display = 'block';
      document.getElementById('passwordForm').style.display = 'none';
      document.getElementById('status').style.display = 'none';
    }
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
  LOG("Main page sent");
}

void handleConnect() {
  LOG("Handling connect request");
  
  if (server.hasArg("ssid") && server.hasArg("password")) {
    selectedSSID = server.arg("ssid");
    enteredPassword = server.arg("password");
    
    LOG("Saving credentials: SSID=" + selectedSSID);
    
    prefs.begin("wifi", false);
    prefs.putString("ssid", selectedSSID);
    prefs.putString("pass", enteredPassword);
    prefs.end();
    
    shouldConnectToWiFi = true;
    
    String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>body{font-family:sans-serif;padding:2em;text-align:center;}h2{font-size:1.5em;}</style></head><body>";
    html += "<h2>Connecting to " + selectedSSID + "...</h2>";
    html += "<p>You can close this window.</p>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
    LOG("Credentials saved successfully");
  } else {
    server.send(400, "text/html", "<h1>Error: Missing SSID or password</h1>");
    ERR("Missing credentials in request");
  }
}

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

void sendHeartbeat() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin("https://unvvzrzcmtsnxounsywc.supabase.co/functions/v1/hello-world?hellofrom=esp32");
  int code = http.GET();
  Serial.println("Heartbeat: HTTP " + String(code));
  http.end();
}

unsigned long lastPing = 0;

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
