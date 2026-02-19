// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/CanvasColorModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Palette management", "[color][palette]")
{
    CanvasColorModel model;
    model.set_palette({{"#FF0000", "Red"}, {"#00FF00", "Green"}, {"#0000FF", "Blue"}});
    REQUIRE(model.palette().size() == 3);
}

TEST_CASE("Recent colors MRU dedup", "[color][recent]")
{
    CanvasColorModel model;
    model.set_max_recent(3);
    model.push_recent("#FF0000");
    model.push_recent("#00FF00");
    model.push_recent("#0000FF");
    model.push_recent("#FF0000"); // moved to front
    REQUIRE(model.recent_colors().size() == 3);
    REQUIRE(model.recent_colors()[0] == "#FF0000");
}

TEST_CASE("Recent colors capped", "[color][recent]")
{
    CanvasColorModel model;
    model.set_max_recent(2);
    model.push_recent("#111111");
    model.push_recent("#222222");
    model.push_recent("#333333");
    REQUIRE(model.recent_colors().size() == 2);
}

TEST_CASE("Style copy/paste", "[color][copy]")
{
    CanvasColorModel model;
    REQUIRE_FALSE(model.has_copied_style());
    model.copy_style("#FF0000", "#00FF00");
    REQUIRE(model.has_copied_style());
    REQUIRE(model.copied_fill() == "#FF0000");
    REQUIRE(model.copied_stroke() == "#00FF00");
}

TEST_CASE("Contrast warning for similar colors", "[color][contrast]")
{
    REQUIRE(CanvasColorModel::has_contrast_warning("#FFFFFF", "#EEEEEE"));
    REQUIRE_FALSE(CanvasColorModel::has_contrast_warning("#000000", "#FFFFFF"));
}

TEST_CASE("Fill and stroke color", "[color][active]")
{
    CanvasColorModel model;
    model.set_fill_color({"accent", ColorType::kSemantic});
    REQUIRE(model.fill_color().type == ColorType::kSemantic);
    REQUIRE(model.fill_color().value == "accent");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
