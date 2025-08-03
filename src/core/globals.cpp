#include "config.h"

// Global objects
Preferences prefs;
WebServer server(80);
DNSServer dnsServer;

// Configuration constants
const char* apSSID = "DAYNODE_NOD1";

// Global state variables
bool shouldConnectToWiFi = false;
String selectedSSID;
String enteredPassword;
String networks[8];
int networkCount = 0;
unsigned long lastPing = 0;