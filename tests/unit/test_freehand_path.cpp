#include "canvas/DrawTool.h"
#include "canvas/FreehandPath.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ── FreehandPath: Basic Construction ────────────────────────────

TEST_CASE("FreehandPath default construction", "[freehand]")
{
    FreehandPath path;
    REQUIRE(path.type() == CanvasObjectType::FreehandPath);
    REQUIRE(path.point_count() == 0);
    REQUIRE(path.stroke_width() == Catch::Approx(2.0));
    REQUIRE(path.stroke_color().r == 0);
    REQUIRE(path.stroke_color().g == 0);
    REQUIRE(path.stroke_color().b == 0);
    REQUIRE(path.stroke_color().a == 255);
    REQUIRE(path.smoothing_factor() == Catch::Approx(0.5));
}

// ── Point Management ────────────────────────────────────────────

TEST_CASE("FreehandPath add points and bounds", "[freehand]")
{
    FreehandPath path;
    path.add_point({10.0, 20.0});
    path.add_point({50.0, 80.0});
    path.add_point({30.0, 10.0});

    REQUIRE(path.point_count() == 3);

    const auto bounds = path.local_bounds();
    // Bounds should enclose all points, expanded by half stroke (1.0).
    REQUIRE(bounds.min_x == Catch::Approx(9.0));
    REQUIRE(bounds.min_y == Catch::Approx(9.0));
    REQUIRE(bounds.max_x == Catch::Approx(51.0));
    REQUIRE(bounds.max_y == Catch::Approx(81.0));
}

TEST_CASE("FreehandPath empty bounds", "[freehand]")
{
    FreehandPath path;
    const auto bounds = path.local_bounds();
    REQUIRE(bounds.min_x == Catch::Approx(0.0));
    REQUIRE(bounds.max_x == Catch::Approx(0.0));
}

// ── Stroke Properties ───────────────────────────────────────────

TEST_CASE("FreehandPath stroke properties", "[freehand]")
{
    FreehandPath path;
    path.set_stroke_color({255, 0, 0, 128});
    path.set_stroke_width(5.0);

    REQUIRE(path.stroke_color().r == 255);
    REQUIRE(path.stroke_color().a == 128);
    REQUIRE(path.stroke_width() == Catch::Approx(5.0));

    // Stroke width has a minimum of 0.5.
    path.set_stroke_width(0.1);
    REQUIRE(path.stroke_width() == Catch::Approx(0.5));
}

// ── Smoothing Factor ────────────────────────────────────────────

TEST_CASE("FreehandPath smoothing factor clamped", "[freehand]")
{
    FreehandPath path;
    path.set_smoothing_factor(1.5);
    REQUIRE(path.smoothing_factor() == Catch::Approx(1.0));

    path.set_smoothing_factor(-0.5);
    REQUIRE(path.smoothing_factor() == Catch::Approx(0.0));

    path.set_smoothing_factor(0.7);
    REQUIRE(path.smoothing_factor() == Catch::Approx(0.7));
}

// ── Chaikin Smoothing ───────────────────────────────────────────

TEST_CASE("FreehandPath Chaikin smoothing increases points", "[freehand]")
{
    FreehandPath path;
    path.add_point({0.0, 0.0});
    path.add_point({50.0, 100.0});
    path.add_point({100.0, 0.0});
    path.set_smoothing_factor(1.0); // Maximum smoothing (4 passes).

    const size_t original_count = path.point_count();
    path.smooth();

    // Chaikin subdivision adds points each pass. After smoothing,
    // the point count should be significantly larger.
    REQUIRE(path.point_count() > original_count);

    // The first and last points should be preserved.
    REQUIRE(path.points().front().x == Catch::Approx(0.0));
    REQUIRE(path.points().front().y == Catch::Approx(0.0));
    REQUIRE(path.points().back().x == Catch::Approx(100.0));
    REQUIRE(path.points().back().y == Catch::Approx(0.0));
}

TEST_CASE("FreehandPath no smoothing with factor 0", "[freehand]")
{
    FreehandPath path;
    path.add_point({0.0, 0.0});
    path.add_point({50.0, 100.0});
    path.add_point({100.0, 0.0});
    path.set_smoothing_factor(0.0);

    const size_t original_count = path.point_count();
    path.smooth();

    REQUIRE(path.point_count() == original_count);
}

// ── RDP Simplification ──────────────────────────────────────────

TEST_CASE("FreehandPath RDP simplification removes collinear points", "[freehand]")
{
    FreehandPath path;
    // A straight line from (0,0) to (100,0) with intermediate points.
    for (int idx = 0; idx <= 10; ++idx)
    {
        path.add_point({static_cast<double>(idx * 10), 0.0});
    }

    REQUIRE(path.point_count() == 11);
    path.simplify(1.0);

    // All intermediate points are collinear, so only start+end should remain.
    REQUIRE(path.point_count() == 2);
    REQUIRE(path.points().front().x == Catch::Approx(0.0));
    REQUIRE(path.points().back().x == Catch::Approx(100.0));
}

TEST_CASE("FreehandPath RDP keeps significant deviations", "[freehand]")
{
    FreehandPath path;
    path.add_point({0.0, 0.0});
    path.add_point({50.0, 50.0}); // Significant deviation from straight line.
    path.add_point({100.0, 0.0});

    path.simplify(1.0);

    // The middle point deviates significantly, so all 3 should be kept.
    REQUIRE(path.point_count() == 3);
}

// ── Clone ───────────────────────────────────────────────────────

TEST_CASE("FreehandPath clone", "[freehand]")
{
    FreehandPath path;
    path.add_point({10.0, 20.0});
    path.add_point({30.0, 40.0});
    path.set_stroke_color({255, 0, 0, 200});
    path.set_stroke_width(3.0);
    path.set_name("test_path");

    auto cloned = path.clone();
    const auto& copy = static_cast<const FreehandPath&>(*cloned);

    REQUIRE(copy.point_count() == 2);
    REQUIRE(copy.stroke_color().r == 255);
    REQUIRE(copy.stroke_width() == Catch::Approx(3.0));
    REQUIRE(copy.name() == "test_path");
    // Clone has a new ID.
    REQUIRE(copy.id() != path.id());
}

// ── JSON Serialization ──────────────────────────────────────────

TEST_CASE("FreehandPath to_json", "[freehand]")
{
    FreehandPath path;
    path.add_point({10.0, 20.0});
    path.add_point({30.0, 40.0});

    const auto json = path.to_json();
    REQUIRE(json.find("FreehandPath") != std::string::npos);
    REQUIRE(json.find("points") != std::string::npos);
    REQUIRE(json.find("stroke_width") != std::string::npos);
}

// ── DrawTool ────────────────────────────────────────────────────

TEST_CASE("DrawTool initial state", "[freehand]")
{
    DrawTool tool;
    REQUIRE(tool.tool_mode() == ToolMode::Draw);
    REQUIRE(tool.cursor_name() == "crosshair");
    REQUIRE(tool.is_drawing() == false);
    REQUIRE(tool.current_path() == nullptr);
    REQUIRE(tool.pen_width() == Catch::Approx(2.0));
    REQUIRE(tool.pen_smoothing() == Catch::Approx(0.5));
}

TEST_CASE("DrawTool pen configuration", "[freehand]")
{
    DrawTool tool;
    tool.set_pen_color({0, 128, 255, 255});
    tool.set_pen_width(4.0);
    tool.set_pen_smoothing(0.8);

    REQUIRE(tool.pen_color().g == 128);
    REQUIRE(tool.pen_width() == Catch::Approx(4.0));
    REQUIRE(tool.pen_smoothing() == Catch::Approx(0.8));
}
