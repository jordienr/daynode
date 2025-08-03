#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "../core/config.h"

// Function declarations
void setupAP();
void connectToStoredWiFi();
void clearStoredWiFi();
void performHardwareReset();

#endif // WIFI_MANAGER_H