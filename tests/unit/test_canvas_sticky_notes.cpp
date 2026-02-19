// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/StickyNoteModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Quick create with immediate edit", "[sticky][create]")
{
    StickyNoteModel model;
    model.set_editing(true);
    REQUIRE(model.is_editing());
    model.set_text("Brainstorm idea");
    model.set_editing(false);
    REQUIRE(model.text() == "Brainstorm idea");
}

TEST_CASE("Color presets map to hex", "[sticky][preset]")
{
    REQUIRE(StickyNoteModel::color_for(StickyColorPreset::kYellow) == "#FFF9C4");
    REQUIRE(StickyNoteModel::color_for(StickyColorPreset::kPink) == "#F8BBD0");
    REQUIRE(StickyNoteModel::color_for(StickyColorPreset::kBlue) == "#BBDEFB");
}

TEST_CASE("Size presets map to dimensions", "[sticky][preset]")
{
    const auto [small_w, small_h] = StickyNoteModel::dimensions_for(StickySizePreset::kSmall);
    const auto [large_w, large_h] = StickyNoteModel::dimensions_for(StickySizePreset::kLarge);
    REQUIRE(small_w == 100.0);
    REQUIRE(large_w == 200.0);
}

TEST_CASE("Overflow detection", "[sticky][overflow]")
{
    StickyNoteModel model;
    model.set_max_chars(10);
    model.set_text("Short");
    REQUIRE_FALSE(model.is_overflowing());
    model.set_text("This is a very long note text");
    REQUIRE(model.is_overflowing());
}

TEST_CASE("Clustering suggestions", "[sticky][cluster]")
{
    StickyNoteModel model;
    model.set_suggestions({{"c1", {"s1", "s2"}, "proximity"}});
    REQUIRE(model.suggestions().size() == 1);
    REQUIRE(model.suggestions()[0].reason == "proximity");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
