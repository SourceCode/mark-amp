#include "core/ShellChromeCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ShellChromeCoordinator default proportions", "[v22][shell_chrome]")
{
    ShellChromeCoordinator coord;
    auto props = coord.workbench_proportions();

    REQUIRE(props.sidebar_width == 260);
    REQUIRE(props.secondary_sidebar_width == 260);
    REQUIRE(props.panel_height == 200);
    REQUIRE(props.activity_bar_width == 48);
    REQUIRE(props.status_bar_height == 22);
}

TEST_CASE("ShellChromeCoordinator rebalance modes", "[v22][shell_chrome]")
{
    ShellChromeCoordinator coord;

    SECTION("Default mode")
    {
        coord.rebalance_proportions(ShellChromeCoordinator::RebalanceMode::kDefault);
        auto props = coord.workbench_proportions();
        REQUIRE(props.sidebar_width == 260);
        REQUIRE(props.panel_height == 200);
    }

    SECTION("Compact mode")
    {
        coord.rebalance_proportions(ShellChromeCoordinator::RebalanceMode::kCompact);
        auto props = coord.workbench_proportions();
        REQUIRE(props.sidebar_width == 200);
        REQUIRE(props.panel_height == 160);
    }

    SECTION("Custom mode preserves existing")
    {
        coord.rebalance_proportions(ShellChromeCoordinator::RebalanceMode::kCompact);
        coord.rebalance_proportions(ShellChromeCoordinator::RebalanceMode::kCustom);
        auto props = coord.workbench_proportions();
        // Should keep compact proportions
        REQUIRE(props.sidebar_width == 200);
    }
}

TEST_CASE("ShellChromeCoordinator splitter state tracking", "[v22][shell_chrome]")
{
    ShellChromeCoordinator coord;
    REQUIRE(coord.splitter_visual_state() == ShellChromeCoordinator::SplitterState::kRest);

    coord.set_splitter_state(ShellChromeCoordinator::SplitterState::kHover);
    REQUIRE(coord.splitter_visual_state() == ShellChromeCoordinator::SplitterState::kHover);

    coord.set_splitter_state(ShellChromeCoordinator::SplitterState::kDragging);
    REQUIRE(coord.splitter_visual_state() == ShellChromeCoordinator::SplitterState::kDragging);
}

TEST_CASE("ShellChromeCoordinator responsiveness mode detection", "[v22][shell_chrome]")
{
    ShellChromeCoordinator coord;

    SECTION("Wide window")
    {
        coord.update_responsiveness(1400);
        REQUIRE(coord.responsiveness_mode() ==
                ShellChromeCoordinator::ResponsivenessMode::kWide);
    }

    SECTION("Default window")
    {
        coord.update_responsiveness(1000);
        REQUIRE(coord.responsiveness_mode() ==
                ShellChromeCoordinator::ResponsivenessMode::kDefault);
    }

    SECTION("Compact window")
    {
        coord.update_responsiveness(800);
        REQUIRE(coord.responsiveness_mode() ==
                ShellChromeCoordinator::ResponsivenessMode::kCompact);
    }

    SECTION("Exact boundary values")
    {
        coord.update_responsiveness(1200);
        REQUIRE(coord.responsiveness_mode() ==
                ShellChromeCoordinator::ResponsivenessMode::kWide);

        coord.update_responsiveness(900);
        REQUIRE(coord.responsiveness_mode() ==
                ShellChromeCoordinator::ResponsivenessMode::kDefault);

        coord.update_responsiveness(899);
        REQUIRE(coord.responsiveness_mode() ==
                ShellChromeCoordinator::ResponsivenessMode::kCompact);
    }
}

TEST_CASE("ShellChromeCoordinator empty state config", "[v22][shell_chrome]")
{
    ShellChromeCoordinator coord;
    auto config = coord.empty_state_config();

    REQUIRE_FALSE(config.title.empty());
    REQUIRE_FALSE(config.subtitle.empty());
    REQUIRE(config.show_recent_files);
    REQUIRE(config.show_quick_actions);
}
