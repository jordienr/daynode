"use client";

import { useState } from "react";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
  ResponsiveContainer,
} from "recharts";

interface DeviceEvent {
  device_id: string;
  measure_1: number;
  created_at: string;
  [key: string]: any;
}

interface ApiResponse {
  events?: DeviceEvent[];
  count?: number;
  device_id?: string;
  from?: string;
  to?: string;
  error?: string;
  details?: string;
}

export default function DeviceEventsPage() {
  const [deviceId, setDeviceId] = useState("");
  const [fromDate, setFromDate] = useState("");
  const [toDate, setToDate] = useState("");
  const [events, setEvents] = useState<DeviceEvent[] | null>(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [selectedRange, setSelectedRange] = useState<string>("");

  // Set default dates (last 24 hours)
  const getDefaultDates = () => {
    const now = new Date();
    const yesterday = new Date(now.getTime() - 24 * 60 * 60 * 1000);

    return {
      from: yesterday.toISOString().slice(0, 16), // Format for datetime-local input
      to: now.toISOString().slice(0, 16),
    };
  };

  // Initialize with default dates
  useState(() => {
    const defaults = getDefaultDates();
    setFromDate(defaults.from);
    setToDate(defaults.to);
  });

  // Quick time range functions
  const setQuickRange = (range: string) => {
    const now = new Date();
    let from: Date;

    switch (range) {
      case "30min":
        from = new Date(now.getTime() - 30 * 60 * 1000);
        break;
      case "1hour":
        from = new Date(now.getTime() - 60 * 60 * 1000);
        break;
      case "1day":
        from = new Date(now.getTime() - 24 * 60 * 60 * 1000);
        break;
      default:
        return;
    }

    setFromDate(from.toISOString().slice(0, 16));
    setToDate(now.toISOString().slice(0, 16));
    setSelectedRange(range);
  };

  const fetchDeviceEvents = async () => {
    if (!deviceId.trim()) {
      setError("Please enter a device ID");
      return;
    }

    setLoading(true);
    setError(null);
    setEvents(null);

    try {
      const payload = {
        device_id: deviceId.trim(),
        from: fromDate || null,
        to: toDate || null,
      };

      console.log("Requesting events:", payload);

      const response = await fetch("/api/device-events", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(payload),
      });

      const data: ApiResponse = await response.json();

      if (!response.ok) {
        throw new Error(data.error || `HTTP ${response.status}`);
      }

      setEvents(data.events || []);
      console.log(
        `Retrieved ${data.count || 0} events for device ${data.device_id}`
      );
    } catch (err) {
      setError(err instanceof Error ? err.message : "Failed to fetch events");
      console.error("Error fetching events:", err);
    } finally {
      setLoading(false);
    }
  };

  const formatTimestamp = (created_at: string) => {
    return new Date(created_at).toLocaleString();
  };

  // Transform data for chart
  const getChartData = () => {
    if (!events || events.length === 0) return [];

    return events
      .map((event) => {
        const date = new Date(event.created_at);
        return {
          timestamp: date.toLocaleTimeString("en-US", {
            hour12: false,
            hour: "2-digit",
            minute: "2-digit",
            second: "2-digit",
          }),
          fullTimestamp: date.toLocaleString(),
          measure_1: event.measure_1,
          device_id: event.device_id,
          rawTimestamp: event.created_at,
        };
      })
      .reverse(); // Reverse to show chronological order (oldest to newest)
  };

  return (
    <div className="min-h-screen bg-gray-50 py-8">
      <div className="max-w-4xl mx-auto px-4">
        <div className="bg-white rounded-lg shadow-md p-6">
          <h1 className="text-3xl font-bold text-gray-900 mb-8">
            Daynode Device Events Dashboard
          </h1>

          {/* Query Form */}
          <div className="bg-gray-50 rounded-lg p-6 mb-6">
            <h2 className="text-xl font-semibold mb-4">Query Device Events</h2>

            {/* Quick Time Range Buttons */}
            <div className="mb-4">
              <label className="block text-sm font-medium text-gray-700 mb-2">
                Quick Time Ranges
              </label>
              <div className="flex flex-wrap gap-2">
                {[
                  { value: "30min", label: "Last 30 mins" },
                  { value: "1hour", label: "Last 1 hour" },
                  { value: "1day", label: "Last 1 day" },
                ].map((range) => (
                  <button
                    key={range.value}
                    onClick={() => setQuickRange(range.value)}
                    className={`px-3 py-1 text-sm rounded-md border transition-colors ${
                      selectedRange === range.value
                        ? "bg-blue-600 text-white border-blue-600"
                        : "bg-white text-gray-700 border-gray-300 hover:bg-gray-50"
                    }`}
                  >
                    {range.label}
                  </button>
                ))}
                <button
                  onClick={() => {
                    setSelectedRange("");
                    setFromDate("");
                    setToDate("");
                  }}
                  className="px-3 py-1 text-sm rounded-md border bg-white text-gray-700 border-gray-300 hover:bg-gray-50"
                >
                  Clear
                </button>
              </div>
            </div>

            <div className="grid grid-cols-1 md:grid-cols-3 gap-4 mb-4">
              <div>
                <label
                  htmlFor="deviceId"
                  className="block text-sm font-medium text-gray-700 mb-2"
                >
                  Device ID
                </label>
                <input
                  type="text"
                  id="deviceId"
                  value={deviceId}
                  onChange={(e) => setDeviceId(e.target.value)}
                  placeholder="e.g., 001"
                  className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500"
                />
              </div>

              <div>
                <label
                  htmlFor="fromDate"
                  className="block text-sm font-medium text-gray-700 mb-2"
                >
                  From Date & Time
                </label>
                <input
                  type="datetime-local"
                  id="fromDate"
                  value={fromDate}
                  onChange={(e) => {
                    setFromDate(e.target.value);
                    setSelectedRange(""); // Clear quick range selection
                  }}
                  className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500"
                />
              </div>

              <div>
                <label
                  htmlFor="toDate"
                  className="block text-sm font-medium text-gray-700 mb-2"
                >
                  To Date & Time
                </label>
                <input
                  type="datetime-local"
                  id="toDate"
                  value={toDate}
                  onChange={(e) => {
                    setToDate(e.target.value);
                    setSelectedRange(""); // Clear quick range selection
                  }}
                  className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500"
                />
              </div>
            </div>

            <button
              onClick={fetchDeviceEvents}
              disabled={loading}
              className="bg-blue-600 hover:bg-blue-700 disabled:bg-blue-400 text-white font-medium py-2 px-4 rounded-md transition-colors"
            >
              {loading ? "Loading..." : "Fetch Events"}
            </button>
          </div>

          {/* Error Display */}
          {error && (
            <div className="bg-red-50 border-l-4 border-red-400 p-4 mb-6">
              <div className="flex">
                <div className="flex-shrink-0">
                  <svg
                    className="h-5 w-5 text-red-400"
                    viewBox="0 0 20 20"
                    fill="currentColor"
                  >
                    <path
                      fillRule="evenodd"
                      d="M10 18a8 8 0 100-16 8 8 0 000 16zM8.707 7.293a1 1 0 00-1.414 1.414L8.586 10l-1.293 1.293a1 1 0 101.414 1.414L10 11.414l1.293 1.293a1 1 0 001.414-1.414L11.414 10l1.293-1.293a1 1 0 00-1.414-1.414L10 8.586 8.707 7.293z"
                      clipRule="evenodd"
                    />
                  </svg>
                </div>
                <div className="ml-3">
                  <h3 className="text-sm font-medium text-red-800">Error</h3>
                  <div className="mt-2 text-sm text-red-700">{error}</div>
                </div>
              </div>
            </div>
          )}

          {/* Results Display */}
          {events !== null && (
            <div>
              <h2 className="text-xl font-semibold mb-4">
                Events for Device: {deviceId} ({events.length} events)
              </h2>

              {events.length === 0 ? (
                <div className="bg-yellow-50 border-l-4 border-yellow-400 p-4">
                  <p className="text-yellow-700">
                    No events found for the specified criteria.
                  </p>
                </div>
              ) : (
                <>
                  {/* Line Chart */}
                  {events.length > 0 && (
                    <div className="mb-6">
                      <h3 className="text-lg font-medium mb-4">
                        Measure 1 Over Time
                      </h3>
                      <div className="bg-gray-50 p-4 rounded-lg">
                        <ResponsiveContainer width="100%" height={400}>
                          <LineChart
                            data={getChartData()}
                            margin={{
                              top: 5,
                              right: 30,
                              left: 20,
                              bottom: 5,
                            }}
                          >
                            <CartesianGrid strokeDasharray="3 3" />
                            <XAxis
                              dataKey="timestamp"
                              tick={{ fontSize: 12 }}
                              angle={-45}
                              textAnchor="end"
                              height={60}
                            />
                            <YAxis
                              label={{
                                value: "Measure 1",
                                angle: -90,
                                position: "insideLeft",
                              }}
                            />
                            <Tooltip
                              labelFormatter={(value, payload) => {
                                if (payload && payload[0]) {
                                  return `Time: ${payload[0].payload.fullTimestamp}`;
                                }
                                return `Time: ${value}`;
                              }}
                              formatter={(value, name) => [
                                `${value}`,
                                "Measure 1",
                              ]}
                              labelStyle={{ color: "#374151" }}
                              contentStyle={{
                                backgroundColor: "white",
                                border: "1px solid #e5e7eb",
                                borderRadius: "6px",
                                boxShadow: "0 4px 6px -1px rgba(0, 0, 0, 0.1)",
                              }}
                            />
                            <Legend />
                            <Line
                              type="monotone"
                              dataKey="measure_1"
                              stroke="#3b82f6"
                              strokeWidth={2}
                              dot={{ fill: "#3b82f6", strokeWidth: 2, r: 4 }}
                              activeDot={{
                                r: 6,
                                stroke: "#3b82f6",
                                strokeWidth: 2,
                              }}
                              name="Measure 1"
                            />
                          </LineChart>
                        </ResponsiveContainer>
                      </div>
                    </div>
                  )}

                  {/* Events Table */}
                  <div className="overflow-x-auto mb-6">
                    <table className="min-w-full bg-white border border-gray-300">
                      <thead className="bg-gray-50">
                        <tr>
                          <th className="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                            Timestamp
                          </th>
                          <th className="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                            Device ID
                          </th>
                          <th className="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                            Measure 1
                          </th>
                        </tr>
                      </thead>
                      <tbody className="bg-white divide-y divide-gray-200">
                        {events.slice(0, 10).map((event, index) => (
                          <tr key={index} className="hover:bg-gray-50">
                            <td className="px-6 py-4 whitespace-nowrap text-sm text-gray-900">
                              {formatTimestamp(event.created_at)}
                            </td>
                            <td className="px-6 py-4 whitespace-nowrap text-sm text-gray-900">
                              {event.device_id}
                            </td>
                            <td className="px-6 py-4 whitespace-nowrap text-sm text-gray-900">
                              {event.measure_1}
                            </td>
                          </tr>
                        ))}
                      </tbody>
                    </table>
                    {events.length > 10 && (
                      <p className="text-sm text-gray-500 mt-2">
                        Showing first 10 of {events.length} events in table. See
                        raw data below for all events.
                      </p>
                    )}
                  </div>
                </>
              )}
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
