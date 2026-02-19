// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/ResponsiveModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("Effective padding scales with DPI", "[responsive][scale]")
{
    ResponsiveModel model;
    model.set_scale({2.0, 4, 16});
    REQUIRE(model.effective_padding() == 8);
    REQUIRE(model.effective_icon_size() == 32);
}

TEST_CASE("1x scale returns base values", "[responsive][scale]")
{
    ResponsiveModel model;
    model.set_scale({1.0, 4, 16});
    REQUIRE(model.effective_padding() == 4);
    REQUIRE(model.effective_icon_size() == 16);
}

TEST_CASE("1.5x scale rounds correctly", "[responsive][scale]")
{
    ResponsiveModel model;
    model.set_scale({1.5, 4, 16});
    REQUIRE(model.effective_padding() == 6);
    REQUIRE(model.effective_icon_size() == 24);
}

TEST_CASE("Compact mode detection", "[responsive][compact]")
{
    ResponsiveModel model;
    model.set_compact_threshold(600);
    model.set_window_width(800);
    REQUIRE_FALSE(model.is_compact());
    model.set_window_width(500);
    REQUIRE(model.is_compact());
}

TEST_CASE("Toolbar overflow in compact mode", "[responsive][compact]")
{
    ResponsiveModel model;
    model.set_compact_threshold(600);
    model.set_window_width(500);
    model.set_toolbar_item_count(10);
    REQUIRE(model.visible_toolbar_items() == 5);
    REQUIRE(model.overflow_count() == 5);
}

TEST_CASE("No overflow in normal mode", "[responsive][compact]")
{
    ResponsiveModel model;
    model.set_compact_threshold(600);
    model.set_window_width(1024);
    model.set_toolbar_item_count(10);
    REQUIRE(model.visible_toolbar_items() == 10);
    REQUIRE(model.overflow_count() == 0);
}

TEST_CASE("Platform convention: macOS", "[responsive][platform]")
{
    ResponsiveModel model;
    model.set_platform(Platform::kMacOS);
    REQUIRE(model.close_button_side() == "left");
    REQUIRE(model.modifier_label() == "⌘");
}

TEST_CASE("Platform convention: Windows", "[responsive][platform]")
{
    ResponsiveModel model;
    model.set_platform(Platform::kWindows);
    REQUIRE(model.close_button_side() == "right");
    REQUIRE(model.modifier_label() == "Ctrl");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
