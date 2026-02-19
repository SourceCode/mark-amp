// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/ShapeModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Default shape is rectangle", "[shape][type]")
{
    ShapeModel model;
    REQUIRE(model.type() == ShapeType::kRectangle);
}

TEST_CASE("Corner radius clamped to half of min dimension", "[shape][geometry]")
{
    ShapeModel model;
    model.set_bounds({0, 0, 100, 60});
    model.set_corner_radius(50.0);
    REQUIRE(model.corner_radius() == 30.0); // clamped to 60/2
}

TEST_CASE("Constrain proportions makes square", "[shape][geometry]")
{
    ShapeModel model;
    model.set_bounds({0, 0, 200, 100});
    model.constrain_proportions();
    REQUIRE(model.bounds().width == 100.0);
    REQUIRE(model.bounds().height == 100.0);
}

TEST_CASE("Style inheritance via commit", "[shape][style]")
{
    ShapeModel model;
    model.set_style({"#FF0000", "#00FF00", 3.0, 5.0, 0.8});
    model.commit_style_as_default();
    REQUIRE(model.default_style().fill_color == "#FF0000");
    REQUIRE(model.default_style().stroke_width == 3.0);
}

TEST_CASE("Reset default style", "[shape][style]")
{
    ShapeModel model;
    model.set_style({"#FF0000", "#00FF00", 3.0, 5.0, 0.8});
    model.commit_style_as_default();
    model.reset_default_style();
    REQUIRE(model.default_style().fill_color == "#FFFFFF");
}

TEST_CASE("Constrained bounds", "[shape][constrained]")
{
    ShapeModel model;
    model.set_bounds({10, 20, 200, 100});
    model.set_constrained(true);
    const auto cb = model.constrained_bounds();
    REQUIRE(cb.width == 100.0);
    REQUIRE(cb.height == 100.0);
    REQUIRE(cb.x == 10.0);
}

TEST_CASE("Non-constrained bounds unchanged", "[shape][constrained]")
{
    ShapeModel model;
    model.set_bounds({10, 20, 200, 100});
    const auto cb = model.constrained_bounds();
    REQUIRE(cb.width == 200.0);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
