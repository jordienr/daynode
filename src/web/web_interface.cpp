#include "web_interface.h"

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