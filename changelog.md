# Changelog

All notable changes to the DayNode project will be documented in this file.

## [August 3, 2025] - Major Code Refactoring

### Added

- **config.h** - Central configuration file containing all includes, constants, and global variable declarations
- **globals.cpp** - Implementation file for all global variables (Preferences, WebServer, DNSServer, etc.)
- **web_interface.h** - Header file for web interface functionality
- **web_interface.cpp** - Implementation of HTML generation and web page serving functions (`showMainPage()`, `handleConnect()`)
- **wifi_manager.h** - Header file for WiFi management functionality
- **wifi_manager.cpp** - Implementation of WiFi connection logic, AP setup, and captive portal handlers (`setupAP()`, `connectToStoredWiFi()`)
- **network_service.h** - Header file for network communication functionality
- **network_service.cpp** - Implementation of heartbeat and network communication functions (`sendHeartbeat()`)

### Changed

- **daynode.ino** - Completely refactored from a monolithic 304-line file to a clean main file containing only `setup()` and `loop()` functions with proper module includes
- Improved code organization by separating concerns into logical modules
- Enhanced maintainability by splitting large file into focused, smaller components

### Technical Details

- Split original 304-line monolithic file into 9 modular files
- Maintained all original functionality while improving code structure
- Used proper header guards and extern declarations for global variables
- No breaking changes to existing functionality - all WiFi configuration, captive portal, and heartbeat features preserved
- Improved compilation organization with clear separation of concerns

### Benefits

- **Maintainability**: Each module has a single responsibility
- **Readability**: Code is now organized into logical sections
- **Extensibility**: New features can be added to appropriate modules without cluttering the main file
- **Debugging**: Issues can be isolated to specific modules more easily

## [August 3, 2025] - Folder Structure Reorganization

### Changed

- **Project Structure** - Reorganized all source files into logical folder hierarchy
- **Include Paths** - Updated all `#include` statements to reflect new folder structure

### New Folder Structure

```
daynode/
├── daynode.ino           # Main Arduino file (must stay in root)
├── README.md             # Documentation
├── changelog.md          # This file
└── src/                  # All source code organized by functionality
    ├── core/             # Core configuration and global variables
    │   ├── config.h      # Central configuration with includes and declarations
    │   └── globals.cpp   # Global variable definitions
    ├── web/              # Web interface and UI components
    │   ├── web_interface.h
    │   └── web_interface.cpp
    ├── wifi/             # WiFi management and connection handling
    │   ├── wifi_manager.h
    │   └── wifi_manager.cpp
    └── network/          # Network services and HTTP communication
        ├── network_service.h
        └── network_service.cpp
```

### Separation of Concerns

- **`src/core/`** - Configuration, includes, and global state management
- **`src/web/`** - HTML generation, web page serving, and UI logic
- **`src/wifi/`** - WiFi connections, AP setup, captive portal handlers
- **`src/network/`** - HTTP requests, heartbeat functionality, external communication

### Benefits of New Structure

- **Clear Separation**: UI concerns separated from networking and WiFi management
- **Scalability**: Easy to add new modules without cluttering specific areas
- **Developer Experience**: TypeScript-like folder organization familiar to web developers
- **Maintainability**: Related code grouped together, easier to locate and modify
- **Code Navigation**: Logical hierarchy makes it easier to understand project structure
