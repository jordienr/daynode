# Changelog

All notable changes to the Daynode project will be documented in this file.

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

## [August 3, 2025] - WiFi Reset Functionality

### Added

- **WiFi Reset Function** - `clearStoredWiFi()` function to erase stored WiFi credentials from flash memory
- **Reset Web Endpoint** - `/reset-wifi` route accessible via web interface
- **Reset UI Button** - Red "🗑️ Reset WiFi Settings" button on main web page with confirmation dialog

### Technical Implementation

- **`src/wifi/wifi_manager.h`** - Added `clearStoredWiFi()` function declaration
- **`src/wifi/wifi_manager.cpp`** - Implemented `clearStoredWiFi()` using `prefs.clear()` to erase "wifi" namespace
- **`src/wifi/wifi_manager.cpp`** - Added `/reset-wifi` web route with user-friendly confirmation page
- **`src/web/web_interface.cpp`** - Added reset button to main interface with JavaScript confirmation dialog

### Usage

- **Via Web**: Connect to ESP32 WiFi hotspot and click "🗑️ Reset WiFi Settings" button
- **Via URL**: Navigate to `http://192.168.4.1/reset-wifi` when connected to ESP32 AP
- **Effect**: Clears all stored WiFi credentials; ESP32 will start in AP mode on next boot

### Benefits

- **Easy Recovery**: Simple way to clear bad WiFi credentials without reflashing firmware
- **User-Friendly**: Web-based interface with confirmation dialog prevents accidental resets
- **Debugging**: Useful for testing and development when switching between different WiFi networks

## [August 3, 2025] - Development Toggle for WiFi Memory

### Added

- **REMEMBER_WIFI Development Toggle** - Global constant in `src/core/config.h` for easy testing
- **Conditional WiFi Storage** - WiFi credentials only saved/loaded when `REMEMBER_WIFI` is `true`
- **Session-Only Mode** - When disabled, WiFi works for current session but doesn't persist

### Technical Implementation

- **`src/core/config.h`** - Added `#define REMEMBER_WIFI true` compile-time constant
- **`src/wifi/wifi_manager.cpp`** - `connectToStoredWiFi()` respects the toggle flag
- **`src/web/web_interface.cpp`** - `handleConnect()` only saves credentials when flag is enabled

### Usage for Development

```cpp
// In src/core/config.h
#define REMEMBER_WIFI true   // Normal operation - save WiFi credentials
#define REMEMBER_WIFI false  // Testing mode - session only, no persistence
```

### Benefits

- **Fast Testing**: Simply toggle `false` to prevent WiFi persistence during development
- **No Manual Reset**: No need to use web interface or clear flash memory
- **Clean Builds**: Each compilation with `REMEMBER_WIFI false` starts fresh
- **Production Ready**: Set to `true` for normal operation with credential persistence

### Behavior

- **When `true`**: Normal operation - save and remember WiFi credentials across reboots
- **When `false`**: Session-only WiFi - connect works but ESP32 forgets on next boot

## [August 3, 2025] - Comprehensive README Documentation

### Changed

- **README.md** - Complete rewrite with comprehensive documentation and reset instructions

### Added to README

- **Project overview** - Clear description of features and capabilities
- **Project structure** - Visual representation of modular folder organization
- **Quick start guide** - Step-by-step setup instructions for new users
- **WiFi reset instructions** - Three different methods with use case guidance:
  - Development toggle (`REMEMBER_WIFI false/true`)
  - Web interface reset (captive portal button)
  - Hardware reset (emergency method)
- **Development configuration** - Testing vs production mode examples
- **Usage scenarios** - Normal operation vs development workflows
- **Troubleshooting section** - Common issues and solutions
- **Network information** - IP addresses, hotspot names, and technical details

### Benefits

- **User-friendly**: Clear instructions for both developers and end users
- **Self-documenting**: Complete project documentation in one place
- **Problem-solving**: Troubleshooting guide reduces support needs
- **Development guidance**: Clear distinction between testing and production modes

## [August 3, 2025] - Project Name Correction

### Changed

- **Project Name**: Fixed capitalization from "DayNode" to "Daynode" across all documentation
- **Files Updated**: README.md, changelog.md, daynode.ino header comment
