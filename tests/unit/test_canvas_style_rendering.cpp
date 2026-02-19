// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/StyleModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Stroke width clamped to positive", "[style][stroke]")
{
    StyleModel model;
    model.set_stroke_width(-1.0);
    REQUIRE(model.stroke_width() == 0.0);
    model.set_stroke_width(5.0);
    REQUIRE(model.stroke_width() == 5.0);
}

TEST_CASE("Dash and cap/join options", "[style][stroke]")
{
    StyleModel model;
    model.set_dash(DashPattern::kDotted);
    model.set_line_cap(LineCap::kSquare);
    model.set_line_join(LineJoin::kBevel);
    REQUIRE(model.dash() == DashPattern::kDotted);
    REQUIRE(model.line_cap() == LineCap::kSquare);
    REQUIRE(model.line_join() == LineJoin::kBevel);
}

TEST_CASE("Fill type and gradient stops", "[style][fill]")
{
    StyleModel model;
    model.set_fill_type(FillType::kLinearGradient);
    model.set_gradient_stops({"#FF0000", "#0000FF"});
    REQUIRE(model.fill_type() == FillType::kLinearGradient);
    REQUIRE(model.gradient_stops().size() == 2);
}

TEST_CASE("Opacity clamped 0-1", "[style][opacity]")
{
    StyleModel model;
    model.set_opacity(1.5);
    REQUIRE(model.opacity() == 1.0);
    model.set_opacity(-0.5);
    REQUIRE(model.opacity() == 0.0);
    model.set_opacity(0.7);
    REQUIRE(model.opacity() == 0.7);
}

TEST_CASE("Shadow properties", "[style][shadow]")
{
    StyleModel model;
    model.set_shadow({3.0, 3.0, 8.0, "#00000080", true});
    REQUIRE(model.shadow().enabled);
    REQUIRE(model.shadow().blur == 8.0);
}

TEST_CASE("Default shadow is disabled", "[style][shadow]")
{
    StyleModel model;
    REQUIRE_FALSE(model.shadow().enabled);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
