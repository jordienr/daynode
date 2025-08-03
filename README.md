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
2. **Connect** to the `DAYNODE_NOD1` WiFi network (or `DAYNODE_xxx` if device ID configured)
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
├── www/                  # Next.js web dashboard for viewing device events
└── README.md             # This file
```

## WiFi Reset Instructions

### Method 1: Hardware Reset (Recommended)

**Best for**: Reliable reset in any situation

1. **Locate** the BOOT button on your ESP32 board (usually labeled "BOOT" or "IO0")
2. **Hold** the BOOT button for **5+ seconds** while the device is powered on
3. **Watch** the Serial Monitor - you'll see reset confirmation messages
4. **Release** the button - the ESP32 will automatically restart in AP mode
5. **Done!** All WiFi credentials are cleared and device restarts fresh

**Serial Monitor Output:**

```
[LOG] Reset button pressed - hold for 5 seconds to reset WiFi
=================================
HARDWARE RESET TRIGGERED!
Clearing WiFi credentials...
=================================
Reset complete. Restarting device...
```

### Method 2: Development Toggle (For Developers)

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

### Method 3: Web Interface Reset (For End Users)

**Best for**: Clearing bad credentials or switching networks

1. **Connect** to the ESP32's WiFi hotspot (`DAYNODE_NOD1` or custom `DAYNODE_xxx`)
2. **Open** any website in your browser (captive portal will appear)
3. **Click** the red "🗑️ Reset WiFi Settings" button
4. **Confirm** the reset in the dialog
5. **Done!** All stored WiFi credentials are erased

Or navigate directly to: `http://192.168.4.1/reset-wifi`

### Method 4: Emergency Flash Reset

**Best for**: When web interface isn't accessible

1. **Flash erase**: Use Arduino IDE to upload any sketch (erases flash)
2. **Physical reset**: Hold BOOT button while powering on, then reflash

## Development Configuration

### Device Identification (secrets.h)

For device identification, use the `secrets.h` file:

1. **Copy the template**: `cp secrets.h.example secrets.h`
2. **Set your device ID**: Open `secrets.h` and uncomment/set `DEVICE_ID`
3. **File is gitignored**: Your device ID won't be committed to version control

**Example secrets.h configuration:**

```cpp
// Device Configuration
#define DEVICE_ID "001"  // Your unique device identifier

// API Configuration
#define API_TOKEN "your-supabase-anon-key"  // Supabase API token for authentication
```

**What it does:**

- **WiFi Hotspot**: Creates `DAYNODE_001` instead of `DAYNODE_NOD1`
- **API Logging**: Sends device events with ID and sensor data every 10 seconds
- **Authentication**: Uses API token for secure communication with Supabase
- **Unique Identity**: Each device gets its own identifier

**Benefits:**

- 🏷️ **Device Tracking**: Identify which device is connecting
- 🔄 **Easy Deployment**: Different IDs per device without code changes
- 📡 **Network Clarity**: Clear device identification in WiFi networks

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

- **Hold BOOT button for 5+ seconds** to trigger hardware reset (most reliable)
- Use web reset to clear old credentials
- Check WiFi password and network name
- Ensure WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)

**Can't access captive portal:**

- **Try hardware reset first** (BOOT button for 5+ seconds)
- Connect to `DAYNODE_NOD1` network (or your custom device network)
- Navigate to `192.168.4.1` manually
- Try different browser or clear browser cache

**Stuck in AP mode:**

- Check if `REMEMBER_WIFI` is set to `false` in development
- **Use hardware reset** (most reliable method)
- Use web interface to reset WiFi settings
- Verify your WiFi credentials are correct

**Hardware reset not working:**

- Ensure you're holding the correct button (BOOT/IO0, not EN)
- Hold for full 5+ seconds until you see serial output
- Check Serial Monitor for reset confirmation messages

## Network Information

- **ESP32 Hotspot**: `DAYNODE_NOD1` (default) or `DAYNODE_xxx` (with device ID)
- **AP IP Address**: `192.168.4.1`
- **Web Interface**: Automatic captive portal or manual navigation
- **API Endpoint**: `log-device-event` (POST every 10 seconds with JSON data)

## Web Dashboard

The `www/` folder contains a Next.js web application for viewing device events:

- **Real-time monitoring**: View events from all your Daynode devices
- **Device filtering**: Query events by specific device ID
- **Date range selection**: Filter events by time period with quick range buttons
- **Interactive charts**: Recharts line chart visualization of sensor data over time
- **Responsive design**: Works on desktop and mobile

### Quick Start

```bash
cd www
npm install
npm run dev
# Visit http://localhost:3000
```

See [www/README-DEVICE-DASHBOARD.md](www/README-DEVICE-DASHBOARD.md) for detailed setup instructions.
