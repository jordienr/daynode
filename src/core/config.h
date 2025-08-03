#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <HTTPClient.h>

// Global objects
extern Preferences prefs;
extern WebServer server;
extern DNSServer dnsServer;

// Configuration constants
extern const char* apSSID;

// Development toggles
#define REMEMBER_WIFI true  // Set to false to ignore stored WiFi credentials

// Global state variables
extern bool shouldConnectToWiFi;
extern String selectedSSID;
extern String enteredPassword;
extern String networks[8];
extern int networkCount;
extern unsigned long lastPing;

// Logging macros
#define LOG(msg) Serial.println(String("[LOG] ") + msg)
#define ERR(msg) Serial.println(String("[ERROR] ") + msg)

#endif // CONFIG_H