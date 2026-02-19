// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/FreehandModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Stroke recording lifecycle", "[freehand][stroke]")
{
    FreehandModel model;
    model.begin_stroke();
    model.add_point({10.0, 20.0, 1.0, 0.0});
    model.add_point({15.0, 25.0, 1.0, 10.0});
    model.add_point({20.0, 30.0, 1.0, 20.0});
    model.end_stroke();
    REQUIRE(model.point_count() == 3);
}

TEST_CASE("Points not added when not recording", "[freehand][stroke]")
{
    FreehandModel model;
    model.add_point({10.0, 20.0, 1.0, 0.0});
    REQUIRE(model.point_count() == 0);
}

TEST_CASE("Smoothing clamps to 0-1", "[freehand][smooth]")
{
    FreehandModel model;
    model.set_smoothing(1.5);
    REQUIRE(model.smoothing() == 1.0);
    model.set_smoothing(-0.5);
    REQUIRE(model.smoothing() == 0.0);
}

TEST_CASE("Smoothed points differ from raw", "[freehand][smooth]")
{
    FreehandModel model;
    model.set_smoothing(0.8);
    model.begin_stroke();
    model.add_point({0.0, 0.0, 1.0, 0.0});
    model.add_point({10.0, 100.0, 1.0, 10.0}); // outlier
    model.add_point({20.0, 0.0, 1.0, 20.0});
    model.add_point({30.0, 0.0, 1.0, 30.0});
    model.end_stroke();

    const auto smoothed = model.smoothed_points();
    REQUIRE(smoothed[1].y < 100.0); // outlier was smoothed down
}

TEST_CASE("Pressure-based width scaling", "[freehand][pressure]")
{
    FreehandModel model;
    model.set_pressure_support(true);
    model.begin_stroke();
    model.add_point({0.0, 0.0, 0.5, 0.0});
    model.end_stroke();

    REQUIRE(model.width_at(0, 4.0) == 2.0); // 4.0 * 0.5
}

TEST_CASE("No pressure gives base width", "[freehand][pressure]")
{
    FreehandModel model;
    model.set_pressure_support(false);
    model.begin_stroke();
    model.add_point({0.0, 0.0, 0.5, 0.0});
    model.end_stroke();

    REQUIRE(model.width_at(0, 4.0) == 4.0); // pressure ignored
}

TEST_CASE("Stroke preset and eraser mode", "[freehand][preset]")
{
    FreehandModel model;
    model.set_preset(StrokePreset::kHighlighter);
    REQUIRE(model.preset() == StrokePreset::kHighlighter);
    model.set_eraser_mode(EraserMode::kSegment);
    REQUIRE(model.eraser_mode() == EraserMode::kSegment);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
