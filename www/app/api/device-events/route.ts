import { NextRequest, NextResponse } from "next/server";
import { createClient } from "@supabase/supabase-js";

const supabaseUrl = process.env.SUPABASE_URL!;
const supabaseServiceKey = process.env.SUPABASE_SERVICE_KEY!;

const supabase = createClient(supabaseUrl, supabaseServiceKey);

export async function POST(request: NextRequest) {
  try {
    const body = await request.json();
    const { device_id, from, to } = body;

    // Validate required fields
    if (!device_id) {
      return NextResponse.json(
        { error: "device_id is required" },
        { status: 400 }
      );
    }

    console.log("Fetching device events:", { device_id, from, to });

    // Build the query
    let query = supabase
      .from("events")
      .select("*")
      .eq("device_id", device_id)
      .order("created_at", { ascending: false });

    // Add date range filters if provided
    if (from) {
      query = query.gte("created_at", from);
    }
    if (to) {
      query = query.lte("created_at", to);
    }

    // Execute the query
    const { data: events, error } = await query;

    if (error) {
      console.error("Supabase query error:", error);
      return NextResponse.json(
        {
          error: "Database query failed",
          details: error.message,
        },
        { status: 500 }
      );
    }

    console.log(
      `Retrieved ${events?.length || 0} events for device ${device_id}`
    );

    return NextResponse.json({
      events: events || [],
      count: events?.length || 0,
      device_id,
      from,
      to,
    });
  } catch (error) {
    console.error("API error:", error);
    return NextResponse.json(
      {
        error: "Internal server error",
        details: error instanceof Error ? error.message : "Unknown error",
      },
      { status: 500 }
    );
  }
}
