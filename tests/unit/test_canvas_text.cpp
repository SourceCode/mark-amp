// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/CanvasTextModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Edit lifecycle: begin, modify, commit", "[text][edit]")
{
    CanvasTextModel model;
    model.set_text("Hello");
    model.begin_edit();
    REQUIRE(model.is_editing());
    model.set_text("Hello World");
    model.commit_edit();
    REQUIRE_FALSE(model.is_editing());
    REQUIRE(model.text() == "Hello World");
}

TEST_CASE("Cancel edit restores original text", "[text][edit]")
{
    CanvasTextModel model;
    model.set_text("Original");
    model.begin_edit();
    model.set_text("Modified");
    model.cancel_edit();
    REQUIRE(model.text() == "Original");
    REQUIRE_FALSE(model.is_editing());
}

TEST_CASE("Typography controls", "[text][typography]")
{
    CanvasTextModel model;
    model.set_typography({"Roboto", 24.0, 700.0, "#FF0000", TextAlign::kCenter, 1.6});
    REQUIRE(model.typography().font_family == "Roboto");
    REQUIRE(model.typography().font_size == 24.0);
    REQUIRE(model.typography().align == TextAlign::kCenter);
}

TEST_CASE("Auto-size vs fixed-box mode", "[text][size]")
{
    CanvasTextModel model;
    REQUIRE(model.size_mode() == TextSizeMode::kAutoSize);
    model.set_size_mode(TextSizeMode::kFixedBox);
    REQUIRE(model.size_mode() == TextSizeMode::kFixedBox);
}

TEST_CASE("Fixed dimensions clamped to positive", "[text][size]")
{
    CanvasTextModel model;
    model.set_fixed_width(-10.0);
    REQUIRE(model.fixed_width() == 1.0);
    model.set_fixed_height(0.0);
    REQUIRE(model.fixed_height() == 1.0);
}

TEST_CASE("Preset applies typography", "[text][preset]")
{
    CanvasTextModel model;
    model.apply_preset(TextPreset::kHeading);
    REQUIRE(model.active_preset() == TextPreset::kHeading);
    REQUIRE(model.typography().font_size == 32.0);
    REQUIRE(model.typography().font_weight == 700.0);
}

TEST_CASE("Preset typography mappings", "[text][preset]")
{
    const auto heading = CanvasTextModel::typography_for(TextPreset::kHeading);
    const auto body = CanvasTextModel::typography_for(TextPreset::kBody);
    REQUIRE(heading.font_size > body.font_size);
    REQUIRE(heading.font_weight > body.font_weight);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
