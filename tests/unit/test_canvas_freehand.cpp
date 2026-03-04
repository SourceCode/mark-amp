/**
 * @file test_canvas_freehand.cpp
 * @brief Phase 42: Tests for StrokeSmoother and StrokeStylePreset.
 */

#include "canvas/StrokeSmoother.h"
#include "canvas/StrokeStylePreset.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ═══════════════════════════════════════════════════════
// StrokePoint
// ═══════════════════════════════════════════════════════

TEST_CASE("StrokePoint - distance", "[canvas][freehand]")
{
    StrokePoint point_a;
    point_a.pos_x = 0.0;
    point_a.pos_y = 0.0;

    StrokePoint point_b;
    point_b.pos_x = 3.0;
    point_b.pos_y = 4.0;

    CHECK(point_a.distance_to(point_b) == 5.0);
}

TEST_CASE("StrokePoint - speed", "[canvas][freehand]")
{
    StrokePoint point_a;
    point_a.pos_x = 0.0;
    point_a.pos_y = 0.0;
    point_a.timestamp_ms = 0;

    StrokePoint point_b;
    point_b.pos_x = 10.0;
    point_b.pos_y = 0.0;
    point_b.timestamp_ms = 5;

    CHECK(point_a.speed_to(point_b) == 2.0); // 10px / 5ms
}

// ═══════════════════════════════════════════════════════
// StrokeSmoother
// ═══════════════════════════════════════════════════════

TEST_CASE("StrokeSmoother - empty state", "[canvas][freehand]")
{
    StrokeSmoother smoother;
    CHECK(smoother.raw_count() == 0);
    CHECK(smoother.smoothing_factor() == 0.5);
}

TEST_CASE("StrokeSmoother - add points", "[canvas][freehand]")
{
    StrokeSmoother smoother;
    smoother.add_point({0.0, 0.0, 1.0, 0});
    smoother.add_point({10.0, 10.0, 0.8, 10});
    CHECK(smoother.raw_count() == 2);
}

TEST_CASE("StrokeSmoother - smooth preserves endpoints", "[canvas][freehand]")
{
    StrokeSmoother smoother;
    smoother.add_point({0.0, 0.0, 1.0, 0});
    smoother.add_point({5.0, 5.0, 1.0, 5});
    smoother.add_point({10.0, 0.0, 1.0, 10});
    smoother.add_point({15.0, 5.0, 1.0, 15});

    auto smoothed = smoother.smooth();
    CHECK(smoothed.size() == 4);
    CHECK(smoothed[0].pos_x == 0.0);      // First preserved
    CHECK(smoothed.back().pos_x == 15.0); // Last preserved
}

TEST_CASE("StrokeSmoother - smoothing factor clamped", "[canvas][freehand]")
{
    StrokeSmoother smoother;
    smoother.set_smoothing_factor(2.0);
    CHECK(smoother.smoothing_factor() == 1.0);

    smoother.set_smoothing_factor(-1.0);
    CHECK(smoother.smoothing_factor() == 0.0);
}

TEST_CASE("StrokeSmoother - point reduction", "[canvas][freehand]")
{
    StrokeSmoother smoother;
    smoother.add_point({0.0, 0.0, 1.0, 0});
    smoother.add_point({1.0, 0.0, 1.0, 1}); // Too close
    smoother.add_point({2.0, 0.0, 1.0, 2}); // Too close
    smoother.add_point({50.0, 0.0, 1.0, 50});

    auto reduced = smoother.reduce(10.0);
    CHECK(reduced.size() < 4); // Should remove close points
}

TEST_CASE("StrokeSmoother - pressure width", "[canvas][freehand]")
{
    double full = StrokeSmoother::pressure_width(1.0, 4.0);
    CHECK(full == 4.0); // 0.3 + 0.7*1.0 = 1.0

    double half = StrokeSmoother::pressure_width(0.5, 4.0);
    CHECK(half > 2.0);
    CHECK(half < 4.0);

    double zero = StrokeSmoother::pressure_width(0.0, 4.0);
    CHECK(zero > 1.0); // Min 30%
}

TEST_CASE("StrokeSmoother - clear", "[canvas][freehand]")
{
    StrokeSmoother smoother;
    smoother.add_point({0.0, 0.0, 1.0, 0});
    smoother.clear();
    CHECK(smoother.raw_count() == 0);
}

// ═══════════════════════════════════════════════════════
// StrokeStyle
// ═══════════════════════════════════════════════════════

TEST_CASE("StrokeStyle - type name", "[canvas][freehand]")
{
    StrokeStyle style;
    style.type = StrokeStyleType::kPen;
    CHECK(style.type_name() == "pen");

    style.type = StrokeStyleType::kHighlighter;
    CHECK(style.type_name() == "highlighter");
}

// ═══════════════════════════════════════════════════════
// StrokeStylePreset
// ═══════════════════════════════════════════════════════

TEST_CASE("StrokeStylePreset - standard presets", "[canvas][freehand]")
{
    StrokeStylePreset presets;
    presets.register_standard_presets();

    CHECK(presets.preset_count() == 4);
    CHECK(presets.find_preset("pen") != nullptr);
    CHECK(presets.find_preset("marker") != nullptr);
    CHECK(presets.find_preset("highlighter") != nullptr);
    CHECK(presets.find_preset("eraser") != nullptr);
}

TEST_CASE("StrokeStylePreset - active preset", "[canvas][freehand]")
{
    StrokeStylePreset presets;
    presets.register_standard_presets();

    CHECK(presets.set_active("marker"));
    CHECK(presets.active_name() == "marker");
    CHECK(presets.active().width == 8.0);
}

TEST_CASE("StrokeStylePreset - preset not found", "[canvas][freehand]")
{
    StrokeStylePreset presets;
    CHECK_FALSE(presets.set_active("nonexistent"));
}
