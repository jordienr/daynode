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

## [August 3, 2025] - Hardware Reset Button Functionality

### Added

- **Hardware Reset Button Support** - BOOT button (GPIO0) monitoring for WiFi credential reset
- **5-Second Hold Detection** - Press and hold BOOT button for 5+ seconds triggers reset
- **Automatic Device Restart** - After clearing credentials, ESP32 automatically restarts
- **Serial Monitor Feedback** - Clear messages when button is pressed/held/reset triggered
- **Button State Tracking** - Global variables to monitor button press duration

### Technical Implementation

- **`src/core/config.h`** - Added `RESET_BUTTON_PIN` (GPIO0) and `RESET_HOLD_TIME` (5000ms) constants
- **`src/core/globals.cpp`** - Added button state tracking variables (`resetButtonPressed`, `resetButtonPressStart`)
- **`src/wifi/wifi_manager.h/.cpp`** - Added `performHardwareReset()` function with clear logging
- **`daynode.ino`** - Added button pinMode setup and monitoring logic in main loop

### Usage

1. **Hold BOOT button** on ESP32 board for 5+ seconds while powered on
2. **Watch Serial Monitor** for confirmation messages
3. **Release button** - device automatically clears WiFi credentials and restarts
4. **ESP32 starts fresh** in AP mode ready for new WiFi configuration

### Serial Output Example

```
[LOG] Reset button pressed - hold for 5 seconds to reset WiFi
=================================
HARDWARE RESET TRIGGERED!
Clearing WiFi credentials...
=================================
Reset complete. Restarting device...
```

### Benefits

- **Most Reliable**: Works even when web interface is inaccessible
- **No Tools Required**: Uses standard BOOT button present on all ESP32 dev boards
- **User-Friendly**: Clear feedback via Serial Monitor
- **Foolproof**: 5-second hold prevents accidental resets
- **Complete Reset**: Clears credentials and restarts device in one action

### Updated Documentation

- **README.md** - Added hardware reset as Method 1 (recommended approach)
- **Troubleshooting** - Updated all sections to mention hardware reset as primary solution
- **Method Reordering** - Hardware reset now primary method, others renumbered accordingly

## [August 3, 2025] - WiFi Network Name Update

### Changed

- **ESP32 Hotspot Name**: Updated from "ESP32_1" to "DAYNODE_NOD1"
- **Files Updated**: `src/core/globals.cpp` (apSSID constant), README.md (all network references)

### Benefits

- **Branded Network Name**: More descriptive and project-specific WiFi hotspot name
- **Easier Identification**: Users can easily identify the Daynode device among other WiFi networks

## [August 3, 2025] - Device Identification System (Simplified)

### Added

- **Device ID Configuration** - `secrets.h` file for unique device identification
- **`.gitignore` file** - Comprehensive gitignore including `secrets.h` and build files
- **`secrets.h.example`** - Simple template file showing DEVICE_ID configuration
- **Dynamic WiFi Hotspot Names** - Use `DEVICE_ID` to create unique WiFi names like `DAYNODE_001`
- **Device Tracking in API** - Append device ID to heartbeat URL for identification

### Technical Implementation

- **`secrets.h`** - Local device configuration file (gitignored)
- **`secrets.h.example`** - Simple template with DEVICE_ID example
- **`src/core/config.h`** - Includes secrets.h for project-wide access
- **`src/network/network_service.cpp`** - Adds device ID parameter to heartbeat URL
- **`src/wifi/wifi_manager.cpp`** - Creates WiFi hotspot name using DEVICE_ID

### Usage Example

```cpp
// In secrets.h
#define DEVICE_ID "001"  // Creates DAYNODE_001 WiFi hotspot
```

### Behavior

- **Without DEVICE_ID**: Uses default name `DAYNODE_NOD1`
- **With DEVICE_ID**: Uses `DAYNODE_` + device ID (e.g., `DAYNODE_001`)
- **API Tracking**: Heartbeat includes `&device=001` parameter for identification

### Benefits

- **Device Tracking**: Easy identification of multiple devices in network and API logs
- **Deployment Flexibility**: Different device IDs without code changes
- **Network Clarity**: Clear device identification in WiFi network lists
- **Simple Configuration**: Single configuration value covers all device identification needs

## [August 3, 2025] - API Logging and Device Events

### Changed

- **API Endpoint**: Changed from `hello-world` GET request to `log-device-event` POST request
- **Request Interval**: Increased from 5 seconds to 10 seconds between API calls
- **Function Name**: Renamed `sendHeartbeat()` to `logDeviceEvent()` for clarity

### Added

- **API Token Authentication** - `API_TOKEN` in secrets.h for Supabase authentication
- **JSON Payload** - POST requests with structured data: `{"device_id": "xxx", "measure_1": random_int}`
- **Random Sensor Data** - `measure_1` field contains random integer between 0-100
- **Enhanced Logging** - Better logging of requests, responses, and authentication status

### Technical Implementation

- **`secrets.h.example`** - Added `API_TOKEN` configuration example
- **`secrets.h`** - Added API token (gitignored for security)
- **`src/network/network_service.h`** - Updated function declaration to `logDeviceEvent()`
- **`src/network/network_service.cpp`** - Complete rewrite for POST with JSON payload and authentication
- **`daynode.ino`** - Updated interval and function call

### API Request Details

```bash
POST https://unvvzrzcmtsnxounsywc.supabase.co/functions/v1/log-device-event
Headers:
  - Authorization: Bearer {API_TOKEN}
  - Content-Type: application/json
Body:
  {"device_id": "001", "measure_1": 42}
```

### Benefits

- **Structured Data**: JSON payload allows for complex sensor data
- **Authentication**: Secure API calls with bearer token
- **Device Tracking**: Clear device identification in server logs
- **Sensor Simulation**: Random data simulates real sensor readings
- **Reduced Load**: 10-second intervals reduce server load while maintaining connectivity

## [August 3, 2025] - Web Dashboard for Device Events

### Added

- **Next.js Web Dashboard** - Complete web application in `www/` folder for viewing device events
- **Device Events API Route** - `/api/device-events` endpoint for querying Supabase edge function
- **Responsive UI Components** - Device ID input, date range picker, and events display
- **Real-time Data Visualization** - Table view and raw JSON display of device events
- **Environment Configuration** - Support for environment variables in Next.js app

### Technical Implementation

- **`www/app/page.tsx`** - Main dashboard component with form and results display
- **`www/app/api/device-events/route.ts`** - API route that calls Supabase `get-device-events` function
- **`www/README-DEVICE-DASHBOARD.md`** - Complete setup and usage documentation
- **Tailwind CSS v4** - Modern styling with responsive design

### Features

- **Device ID Search**: Query events for specific device identifiers
- **Date Range Filtering**: Filter events by start and end datetime
- **Default Time Range**: Automatically sets last 24 hours as default query
- **Error Handling**: User-friendly error messages for API failures
- **Loading States**: Visual feedback during data fetching
- **Multiple Display Formats**: Both table view (first 10 events) and raw JSON (all events)

### API Integration

- **Endpoint**: POST `/api/device-events`
- **Payload**: `{"device_id": "001", "from": "2025-01-06T10:00", "to": "2025-01-06T11:00"}`
- **Authentication**: Uses same Supabase anon key as ESP32 devices
- **Response**: Array of device events with timestamps and sensor data

### Usage Example

```bash
cd www
npm install
npm run dev
# Visit http://localhost:3000
# Enter device ID "001" and click "Fetch Events"
```

### Benefits

- **Centralized Monitoring**: View all device events in one place
- **Historical Analysis**: Query events by date range for trend analysis
- **Developer Friendly**: Easy setup with clear documentation
- **Production Ready**: Built with Next.js 15 and TypeScript for reliability

## [August 3, 2025] - Direct Database Access for Web Dashboard

### Changed

- **API Implementation**: Switched from Supabase edge function to direct database access using Supabase client
- **Environment Variables**: Updated to use `SUPABASE_URL` and `SUPABASE_SERVICE_KEY` instead of anon key
- **Query Performance**: Direct database queries provide better performance and more control

### Added

- **@supabase/supabase-js**: Added Supabase client library dependency
- **Enhanced Response Format**: API now returns metadata including count and query parameters
- **Better Error Handling**: More detailed error messages from direct database queries

### Technical Implementation

- **`www/app/api/device-events/route.ts`** - Replaced fetch to edge function with direct Supabase client queries
- **Database Queries** - Direct queries to `events` table with proper filtering and ordering
- **Service Key Authentication** - Uses service key for backend database access (more secure and powerful)

### API Changes

- **Request Format**: Unchanged - still accepts `{ device_id, from, to }`
- **Response Format**: Enhanced with metadata:
  ```json
  {
    "events": [...],
    "count": 5,
    "device_id": "001",
    "from": "2025-01-06T10:00",
    "to": "2025-01-06T11:00"
  }
  ```

### Benefits

- **Better Performance**: Direct database access eliminates edge function overhead
- **More Control**: Full Supabase query capabilities (filtering, ordering, pagination)
- **Enhanced Security**: Service key provides backend-only database access
- **Improved Debugging**: Direct error messages from database queries
- **Future Scalability**: Foundation for more complex queries and aggregations

## [August 3, 2025] - Database Field Fix and Quick Time Range UI

### Fixed

- **Database Field**: Changed from `events.timestamp` to `events.created_at` to match actual database schema
- **API Queries**: Updated all database queries to use correct `created_at` field
- **Frontend Display**: Updated UI to display timestamps using `created_at` field

### Added

- **Quick Time Range Buttons** - One-click time range selection for common periods
- **Time Range Options**: "Last 30 mins", "Last 1 hour", "Last 1 day"
- **Clear Button**: Reset time range selection and manual date inputs
- **Visual Feedback**: Active quick range button highlighting

### Enhanced User Experience

- **Faster Queries**: Quick buttons eliminate manual date/time input for common use cases
- **Smart Interaction**: Manual date changes clear quick range selection automatically
- **Intuitive UI**: Clear visual indication of selected time range

### Technical Implementation

- **Quick Range Logic**: JavaScript time calculations for precise time ranges
- **State Management**: Added `selectedRange` state to track active quick selection
- **Event Handlers**: Manual date inputs clear quick range selection for consistency
- **Responsive Design**: Button layout adapts to different screen sizes

### UI Changes

- **Time Range Buttons**: Added above device ID and date inputs
- **Button States**: Active/inactive styling with hover effects
- **Grid Layout**: Maintained responsive 3-column layout for inputs
- **Clear Functionality**: One-click reset for all time-related inputs

### Benefits

- **Improved UX**: Common time ranges accessible with single click
- **Reduced Errors**: Pre-calculated time ranges eliminate manual date input mistakes
- **Faster Workflow**: Quick access to recent data for monitoring and debugging

## [August 3, 2025] - Interactive Data Visualization with Recharts

### Added

- **Recharts Line Chart** - Interactive line chart to visualize sensor data over time
- **Chart Data Transformation** - Function to convert device events into chart-friendly format
- **Enhanced Tooltips** - Interactive tooltips showing full timestamp and measurement values
- **Responsive Chart Design** - Chart adapts to different screen sizes with ResponsiveContainer

### Technical Implementation

- **Recharts Library** - Added recharts dependency for chart functionality
- **Chart Component** - LineChart with XAxis (time), YAxis (measure_1), grid, and legend
- **Data Processing** - Transform events data with proper timestamp formatting for X-axis
- **Conditional Rendering** - Chart only displays when events data is available

### Chart Features

- **Time-based X-axis** - Shows timestamps in HH:MM:SS format (24-hour)
- **Measure 1 Y-axis** - Displays sensor values with automatic scaling
- **Interactive Elements** - Hover tooltips, clickable legend, zoom functionality
- **Visual Design** - Blue line with dots, grid lines, and professional styling
- **Chronological Order** - Data displayed from oldest to newest for trend analysis

### UI Layout Updates

- **Chart Placement** - Positioned between data table and raw JSON display
- **Section Headers** - Clear "Measure 1 Over Time" heading
- **Consistent Styling** - Matches existing design with gray background container
- **Responsive Height** - Fixed 400px height for optimal viewing

### Data Visualization Benefits

- **Trend Analysis** - Easy identification of patterns and trends in sensor data
- **Time Correlation** - Visual correlation between time and measurement values
- **Data Quality Assessment** - Quick visual check for anomalies or outliers
- **User Experience** - More engaging and intuitive data presentation
- **Professional Appearance** - Dashboard now suitable for presentations and reports

### Dependencies Added

- **recharts**: ^2.x - React chart library for creating responsive, interactive charts
