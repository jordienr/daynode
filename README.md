# Daynode - ESP32 WiFi Configuration

A modular ESP32 WiFi provisioning system with captive portal and easy credential management.

## Features

- **Captive Portal**: Automatic WiFi setup when connecting to ESP32 hotspot
- **Persistent Storage**: Remembers WiFi credentials across reboots
- **Web Interface**: User-friendly WiFi configuration page
- **Modular Code**: Clean separation of concerns across multiple files
- **Development Tools**: Easy reset and testing capabilities

## Quick Start

1. **Upload** the code to your ESP32
2. **Connect** to the device WiFi network
3. **Configure** your WiFi credentials through the captive portal
4. **Done!** ESP32 will connect to your WiFi automatically

## Project Structure

```
daynode/
├── daynode.ino           # Main Arduino file
├── src/                  # Modular source code
│   ├── core/             # Configuration and globals
│   ├── web/              # Web interface and UI
│   ├── wifi/             # WiFi management and AP setup
│   └── network/          # Network services and HTTP communication
└── README.md             # This file
```

## WiFi Reset Instructions

### Method 1: Development Toggle (For Developers)

**Best for**: Testing and development

1. **Open** `src/core/config.h`
2. **Change** the line:
   ```cpp
   #define REMEMBER_WIFI false  // Set to false for testing
   ```
3. **Upload** the code to ESP32
4. **Result**: WiFi credentials won't be saved, ESP32 starts fresh each boot

To restore normal operation:

```cpp
#define REMEMBER_WIFI true   // Normal operation
```

### Method 2: Web Interface Reset (For End Users)

**Best for**: Clearing bad credentials or switching networks

1. **Connect** to the ESP32's WiFi hotspot (`ESP32_1`)
2. **Open** any website in your browser (captive portal will appear)
3. **Click** the red "🗑️ Reset WiFi Settings" button
4. **Confirm** the reset in the dialog
5. **Done!** All stored WiFi credentials are erased

Or navigate directly to: `http://192.168.4.1/reset-wifi`

### Method 3: Hardware Reset (Emergency)

**Best for**: When web interface isn't accessible

1. **Flash erase**: Use Arduino IDE to upload any sketch (erases flash)
2. **Physical reset**: Hold BOOT button while powering on, then reflash

## Development Configuration

### Testing Mode

```cpp
// In src/core/config.h
#define REMEMBER_WIFI false  // Session-only WiFi (no persistence)
```

### Production Mode

```cpp
// In src/core/config.h
#define REMEMBER_WIFI true   // Save WiFi credentials permanently
```

## Usage Scenarios

### Normal Operation

1. ESP32 boots → tries stored WiFi
2. If connection fails → starts AP mode
3. User configures WiFi → credentials saved
4. ESP32 reboots → connects automatically

### Development/Testing

1. Set `REMEMBER_WIFI false`
2. ESP32 always starts in AP mode
3. Configure WiFi for testing
4. Power cycle → starts fresh (no saved credentials)

## Troubleshooting

**ESP32 won't connect to my WiFi:**

- Use web reset to clear old credentials
- Check WiFi password and network name
- Ensure WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)

**Can't access captive portal:**

- Connect to `ESP32_1` network
- Navigate to `192.168.4.1` manually
- Try different browser or clear browser cache

**Stuck in AP mode:**

- Check if `REMEMBER_WIFI` is set to `false` in development
- Use web interface to reset WiFi settings
- Verify your WiFi credentials are correct

## Network Information

- **ESP32 Hotspot**: `ESP32_1`
- **AP IP Address**: `192.168.4.1`
- **Web Interface**: Automatic captive portal or manual navigation
- **Heartbeat URL**: Configurable in `src/network/network_service.cpp`
