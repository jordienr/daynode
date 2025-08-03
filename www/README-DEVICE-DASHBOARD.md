# Daynode Device Events Dashboard

A Next.js web application for viewing device events from Daynode ESP32 devices.

## Features

- **Device ID Search**: Query events for specific device IDs
- **Date Range Filtering**: Filter events by date and time range
- **Real-time Data**: View device events with sensor measurements
- **Interactive Charts**: Recharts line chart visualization of sensor data over time
- **Responsive UI**: Works on desktop and mobile devices

## Setup

1. **Install dependencies:**

   ```bash
   npm install
   ```

2. **Configure environment:**
   Create `.env.local` file:

   ```
   SUPABASE_URL=https://unvvzrzcmtsnxounsywc.supabase.co
   SUPABASE_SERVICE_KEY=your-supabase-service-key
   ```

3. **Run development server:**

   ```bash
   npm run dev
   ```

4. **Open in browser:**
   Visit [http://localhost:3000](http://localhost:3000)

## Usage

1. **Enter Device ID**: Input the device identifier (e.g., "001")
2. **Set Date Range**: Choose start and end dates (defaults to last 24 hours) or use quick range buttons
3. **Fetch Events**: Click "Fetch Events" to retrieve data
4. **View Results**: See events in table format, interactive line chart, and raw JSON

## API Endpoints

- **POST /api/device-events**: Retrieves device events directly from Supabase database
  - Request body: `{ device_id: string, from?: string, to?: string }`
  - Returns: Object with events array, count, and query metadata
  - Uses Supabase client with service key for direct database access

## Example API Response

```json
{
  "events": [
    {
      "device_id": "001",
      "measure_1": 42,
      "created_at": "2025-01-06T10:30:00Z"
    }
  ],
  "count": 1,
  "device_id": "001",
  "from": "2025-01-06T10:00:00",
  "to": "2025-01-06T11:00:00"
}
```

## Integration with Daynode ESP32

This dashboard displays data from Daynode ESP32 devices that send events every 10 seconds to the Supabase edge function `log-device-event`.

## Dependencies

- **Next.js 15**: React framework with App Router
- **TypeScript**: Type safety
- **Tailwind CSS v4**: Styling
- **React 19**: UI components
- **@supabase/supabase-js**: Supabase client for database access
- **Recharts**: Interactive chart library for data visualization
