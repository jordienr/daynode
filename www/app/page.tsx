"use client";

import { useState, useEffect, useRef } from "react";
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
  temp_c: number;
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
  const [autoRefresh, setAutoRefresh] = useState(false);
  const [countdown, setCountdown] = useState(5);

  const autoRefreshIntervalRef = useRef<NodeJS.Timeout | null>(null);
  const countdownIntervalRef = useRef<NodeJS.Timeout | null>(null);

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

  // Auto-refresh functionality
  useEffect(() => {
    if (autoRefresh && deviceId.trim()) {
      // Start the 5-second refresh cycle
      autoRefreshIntervalRef.current = setInterval(() => {
        fetchDeviceEvents();
        setCountdown(5); // Reset countdown
      }, 5000);

      // Start the countdown timer (updates every second)
      countdownIntervalRef.current = setInterval(() => {
        setCountdown((prev) => {
          if (prev <= 1) {
            return 5; // Reset to 5 when reaching 0
          }
          return prev - 1;
        });
      }, 1000);
    } else {
      // Clear intervals when auto-refresh is disabled
      if (autoRefreshIntervalRef.current) {
        clearInterval(autoRefreshIntervalRef.current);
        autoRefreshIntervalRef.current = null;
      }
      if (countdownIntervalRef.current) {
        clearInterval(countdownIntervalRef.current);
        countdownIntervalRef.current = null;
      }
      setCountdown(5); // Reset countdown
    }

    // Cleanup function
    return () => {
      if (autoRefreshIntervalRef.current) {
        clearInterval(autoRefreshIntervalRef.current);
      }
      if (countdownIntervalRef.current) {
        clearInterval(countdownIntervalRef.current);
      }
    };
  }, [autoRefresh, deviceId]);

  // Cleanup intervals on component unmount
  useEffect(() => {
    return () => {
      if (autoRefreshIntervalRef.current) {
        clearInterval(autoRefreshIntervalRef.current);
      }
      if (countdownIntervalRef.current) {
        clearInterval(countdownIntervalRef.current);
      }
    };
  }, []);

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

  // Toggle auto-refresh functionality
  const toggleAutoRefresh = () => {
    setAutoRefresh(!autoRefresh);
    if (!autoRefresh) {
      setCountdown(5); // Reset countdown when enabling
    }
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
          temp_c: event.temp_c,
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

            <div className="flex flex-wrap gap-3">
              <button
                onClick={fetchDeviceEvents}
                disabled={loading}
                className="bg-blue-600 hover:bg-blue-700 disabled:bg-blue-400 text-white font-medium py-2 px-4 rounded-md transition-colors"
              >
                {loading ? "Loading..." : "Fetch Events"}
              </button>

              <button
                onClick={toggleAutoRefresh}
                disabled={!deviceId.trim() || loading}
                className={`font-medium py-2 px-4 rounded-md transition-colors border ${
                  autoRefresh
                    ? "bg-green-600 hover:bg-green-700 text-white border-green-600"
                    : "bg-white hover:bg-gray-50 text-gray-700 border-gray-300"
                } disabled:opacity-50 disabled:cursor-not-allowed`}
              >
                {autoRefresh ? "Stop Auto-refresh" : "Start Auto-refresh"}
              </button>

              {autoRefresh && (
                <div className="flex items-center px-3 py-2 bg-gray-100 rounded-md">
                  <div className="flex items-center space-x-2">
                    <div className="w-2 h-2 bg-green-500 rounded-full animate-pulse"></div>
                    <span className="text-sm text-gray-700">
                      Next refresh in {countdown}s
                    </span>
                  </div>
                </div>
              )}
            </div>
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
                        Temperature Over Time
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
                                value: "Temperature (°C)",
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
                                "Temperature (°C)",
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
                              dataKey="temp_c"
                              stroke="#3b82f6"
                              strokeWidth={2}
                              dot={{ fill: "#3b82f6", strokeWidth: 2, r: 4 }}
                              activeDot={{
                                r: 6,
                                stroke: "#3b82f6",
                                strokeWidth: 2,
                              }}
                              name="Temperature (°C)"
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
                            Temperature (°C)
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
                              {event.temp_c}
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
