/// @file test_v20_shell_visual.cpp
/// @brief V20 Phase 09 – ShellVisualSystem unit tests.

#include "core/ShellVisualSystem.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ShellVisual: defaults registered", "[v20][shell-visual]")
{
    EventBus bus;
    ShellVisualSystem svs(bus);

    REQUIRE(svs.total_tokens() > 0);
    REQUIRE(svs.all_chrome().size() >= 8);
    REQUIRE(svs.all_states().size() >= 8);
}

TEST_CASE("ShellVisual: chrome lookup", "[v20][shell-visual]")
{
    EventBus bus;
    ShellVisualSystem svs(bus);

    auto* sidebar = svs.chrome("shell.sidebar");
    REQUIRE(sidebar != nullptr);
    REQUIRE(sidebar->layer == ShellLayer::kSidebar);
    REQUIRE(sidebar->has_border());

    auto* editor = svs.chrome("shell.editor");
    REQUIRE(editor != nullptr);
    REQUIRE(editor->layer == ShellLayer::kEditor);

    REQUIRE(svs.chrome("nonexistent") == nullptr);
}

TEST_CASE("ShellVisual: state lookup", "[v20][shell-visual]")
{
    EventBus bus;
    ShellVisualSystem svs(bus);

    auto* tab_hover = svs.state("tab", "hover");
    REQUIRE(tab_hover != nullptr);

    auto* tree_selected = svs.state("tree-row", "selected");
    REQUIRE(tree_selected != nullptr);
    REQUIRE(tree_selected->background == "#094771");

    REQUIRE(svs.state("nonexistent", "hover") == nullptr);
}

TEST_CASE("ShellVisual: custom chrome registration", "[v20][shell-visual]")
{
    EventBus bus;
    ShellVisualSystem svs(bus);

    int initial = svs.total_tokens();

    ShellChromeToken custom;
    custom.name = "shell.custom";
    custom.layer = ShellLayer::kPanel;
    custom.background = "#123456";
    svs.register_chrome(custom);

    REQUIRE(svs.total_tokens() == initial + 1);
    REQUIRE(svs.chrome("shell.custom") != nullptr);
}

TEST_CASE("ShellVisual: dialog has shadow", "[v20][shell-visual]")
{
    EventBus bus;
    ShellVisualSystem svs(bus);

    auto* dialog = svs.chrome("shell.dialog");
    REQUIRE(dialog != nullptr);
    REQUIRE(dialog->has_shadow());
    REQUIRE(dialog->shadow_elevation == 4);
}
