// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/CameraModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Zoom clamped to range", "[camera][zoom]")
{
    CameraModel model;
    model.set_zoom(0.01);
    REQUIRE(model.zoom() == 0.1);
    model.set_zoom(15.0);
    REQUIRE(model.zoom() == 10.0);
}

TEST_CASE("Zoom at cursor anchor", "[camera][zoom]")
{
    CameraModel model;
    model.zoom_at(0.5, 200.0, 150.0);
    REQUIRE(model.anchor_x() == 200.0);
    REQUIRE(model.anchor_y() == 150.0);
    REQUIRE(model.zoom() == 1.5);
}

TEST_CASE("Zoom presets", "[camera][preset]")
{
    CameraModel model;
    model.apply_preset(ZoomPreset::kFiftyPercent);
    REQUIRE(model.zoom() == 0.5);
    model.apply_preset(ZoomPreset::kTwoHundredPercent);
    REQUIRE(model.zoom() == 2.0);
}

TEST_CASE("Pan velocity clamped", "[camera][pan]")
{
    CameraModel model;
    model.set_pan_velocity(0.01);
    REQUIRE(model.pan_velocity() == 0.1);
    model.set_pan_velocity(10.0);
    REQUIRE(model.pan_velocity() == 5.0);
}

TEST_CASE("Boundary enforcement", "[camera][bounds]")
{
    CameraModel model;
    model.set_bounds(-100.0, -100.0, 100.0, 100.0);
    model.set_pan(500.0, -500.0);
    REQUIRE(model.is_out_of_bounds());
    model.enforce_bounds();
    REQUIRE(model.pan_x() == 100.0);
    REQUIRE(model.pan_y() == -100.0);
    REQUIRE_FALSE(model.is_out_of_bounds());
}

TEST_CASE("Recenter recovery", "[camera][recovery]")
{
    CameraModel model;
    model.set_bounds(-200.0, -200.0, 200.0, 200.0);
    model.set_pan(9999.0, 9999.0);
    model.recenter();
    REQUIRE(model.pan_x() == 0.0);
    REQUIRE(model.pan_y() == 0.0);
    REQUIRE(model.zoom() == 1.0);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
