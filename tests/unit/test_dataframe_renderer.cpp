/// @file test_dataframe_renderer.cpp
/// @brief V4 Phase 32 – DataFrameEngine tests.

#include "core/EventBus.h"
#include "ui/DataFrameRenderer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

// ============================================================================
// Test fixture helper
// ============================================================================

struct DFFixture
{
    EventBus event_bus;
    DataFrameEngine engine{event_bus};
};

static const std::string kSampleHtml =
    "<table>"
    "<thead><tr><th>Name</th><th>Age</th><th>City</th></tr></thead>"
    "<tbody>"
    "<tr><td>Alice</td><td>30</td><td>NYC</td></tr>"
    "<tr><td>Bob</td><td>25</td><td>LA</td></tr>"
    "<tr><td>Charlie</td><td>35</td><td>Chicago</td></tr>"
    "</tbody>"
    "</table>";

// ============================================================================
// HTML parsing
// ============================================================================

TEST_CASE("DataFrameEngine parses HTML table columns", "[phase32][dataframe]")
{
    DFFixture ctx;
    ctx.engine.load_from_html(kSampleHtml);

    REQUIRE(ctx.engine.data().column_count() == 3);
    REQUIRE(ctx.engine.data().columns[0].name == "Name");
    REQUIRE(ctx.engine.data().columns[1].name == "Age");
    REQUIRE(ctx.engine.data().columns[2].name == "City");
}

TEST_CASE("DataFrameEngine parses HTML table rows", "[phase32][dataframe]")
{
    DFFixture ctx;
    ctx.engine.load_from_html(kSampleHtml);

    REQUIRE(ctx.engine.data().row_count() == 3);
    REQUIRE(ctx.engine.data().rows[0][0] == "Alice");
    REQUIRE(ctx.engine.data().rows[1][1] == "25");
    REQUIRE(ctx.engine.data().rows[2][2] == "Chicago");
}

// ============================================================================
// JSON parsing
// ============================================================================

TEST_CASE("DataFrameEngine parses JSON table", "[phase32][dataframe]")
{
    DFFixture ctx;
    std::string json = R"({"columns":["X","Y"],"data":[["1","10"],["2","20"]]})";
    ctx.engine.load_from_json(json);

    REQUIRE(ctx.engine.data().column_count() == 2);
    REQUIRE(ctx.engine.data().row_count() == 2);
    REQUIRE(ctx.engine.data().rows[0][0] == "1");
    REQUIRE(ctx.engine.data().rows[1][1] == "20");
}

// ============================================================================
// Sorting
// ============================================================================

TEST_CASE("DataFrameEngine sorts by numeric column ascending", "[phase32][dataframe]")
{
    DFFixture ctx;
    ctx.engine.load_from_html(kSampleHtml);

    ctx.engine.sort_by_column(1, true); // Sort by Age ascending.
    REQUIRE(ctx.engine.data().rows[0][1] == "25");
    REQUIRE(ctx.engine.data().rows[1][1] == "30");
    REQUIRE(ctx.engine.data().rows[2][1] == "35");
}

TEST_CASE("DataFrameEngine sorts by string column descending", "[phase32][dataframe]")
{
    DFFixture ctx;
    ctx.engine.load_from_html(kSampleHtml);

    ctx.engine.sort_by_column(0, false); // Sort by Name descending.
    REQUIRE(ctx.engine.data().rows[0][0] == "Charlie");
    REQUIRE(ctx.engine.data().rows[1][0] == "Bob");
    REQUIRE(ctx.engine.data().rows[2][0] == "Alice");
}

// ============================================================================
// Filtering
// ============================================================================

TEST_CASE("DataFrameEngine filters rows matching text", "[phase32][dataframe]")
{
    DFFixture ctx;
    ctx.engine.load_from_html(kSampleHtml);

    ctx.engine.filter("NYC");
    REQUIRE(ctx.engine.is_filtered());
    REQUIRE(ctx.engine.filtered_rows().size() == 1);
    REQUIRE(ctx.engine.filtered_rows()[0][0] == "Alice");
}

TEST_CASE("DataFrameEngine clears filter", "[phase32][dataframe]")
{
    DFFixture ctx;
    ctx.engine.load_from_html(kSampleHtml);

    ctx.engine.filter("NYC");
    ctx.engine.clear_filter();
    REQUIRE_FALSE(ctx.engine.is_filtered());
}

// ============================================================================
// CSV export
// ============================================================================

TEST_CASE("DataFrameEngine exports CSV with headers", "[phase32][dataframe]")
{
    DFFixture ctx;
    ctx.engine.load_from_html(kSampleHtml);

    auto csv = ctx.engine.export_csv();
    REQUIRE(csv.find("Name,Age,City") != std::string::npos);
    REQUIRE(csv.find("Alice,30,NYC") != std::string::npos);
    REQUIRE(csv.find("Bob,25,LA") != std::string::npos);
}

TEST_CASE("DataFrameEngine exports filtered CSV", "[phase32][dataframe]")
{
    DFFixture ctx;
    ctx.engine.load_from_html(kSampleHtml);
    ctx.engine.filter("Alice");

    auto csv = ctx.engine.export_csv();
    REQUIRE(csv.find("Alice") != std::string::npos);
    REQUIRE(csv.find("Bob") == std::string::npos);
}

// ============================================================================
// Chart data
// ============================================================================

TEST_CASE("DataFrameEngine manages chart data", "[phase32][dataframe]")
{
    DFFixture ctx;
    REQUIRE_FALSE(ctx.engine.has_chart());

    ChartData chart;
    chart.format = "png";
    chart.data = "base64data";
    chart.width = 800;
    chart.height = 600;
    ctx.engine.load_chart(chart);

    REQUIRE(ctx.engine.has_chart());
    REQUIRE(ctx.engine.chart().format == "png");
    REQUIRE(ctx.engine.chart().width == 800);
}
