#include "core/NavigationChromeCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("NavigationChromeCoordinator active tab metrics", "[v22][navigation]")
{
    NavigationChromeCoordinator coord;
    auto metrics = coord.tab_metrics(NavigationChromeCoordinator::TabState::kActive);

    REQUIRE(metrics.min_width > 0);
    REQUIRE(metrics.max_width > metrics.min_width);
    REQUIRE(metrics.height > 0);
    REQUIRE(metrics.active_indicator == BorderWeightToken::kMedium);
}

TEST_CASE("NavigationChromeCoordinator inactive tab metrics", "[v22][navigation]")
{
    NavigationChromeCoordinator coord;
    auto metrics = coord.tab_metrics(NavigationChromeCoordinator::TabState::kInactive);

    REQUIRE(metrics.active_indicator == BorderWeightToken::kNone);
}

TEST_CASE("NavigationChromeCoordinator pinned tab metrics", "[v22][navigation]")
{
    NavigationChromeCoordinator coord;
    auto metrics = coord.tab_metrics(NavigationChromeCoordinator::TabState::kPinned);

    // Pinned tabs are narrower (icon-only)
    REQUIRE(metrics.min_width == 36);
    REQUIRE(metrics.max_width == 36);
    REQUIRE(metrics.padding_h == 0);
}

TEST_CASE("NavigationChromeCoordinator dragging tab metrics", "[v22][navigation]")
{
    NavigationChromeCoordinator coord;
    auto metrics = coord.tab_metrics(NavigationChromeCoordinator::TabState::kDragging);

    // Dragged tabs get corner rounding
    REQUIRE(metrics.corner == CornerRadiusToken::kSm);
}

TEST_CASE("NavigationChromeCoordinator all tab states resolve", "[v22][navigation]")
{
    NavigationChromeCoordinator coord;

    for (auto state : {NavigationChromeCoordinator::TabState::kActive,
                       NavigationChromeCoordinator::TabState::kInactive,
                       NavigationChromeCoordinator::TabState::kModified,
                       NavigationChromeCoordinator::TabState::kPinned,
                       NavigationChromeCoordinator::TabState::kPreview,
                       NavigationChromeCoordinator::TabState::kDragging})
    {
        auto metrics = coord.tab_metrics(state);
        REQUIRE(metrics.height > 0);
        REQUIRE(metrics.icon_size > 0);
    }
}

TEST_CASE("NavigationChromeCoordinator breadcrumb metrics", "[v22][navigation]")
{
    NavigationChromeCoordinator coord;
    auto metrics = coord.breadcrumb_metrics();

    REQUIRE(metrics.height > 0);
    REQUIRE(metrics.segment_padding_h > 0);
    REQUIRE(metrics.separator_width > 0);
    REQUIRE(metrics.icon_size > 0);
    REQUIRE(metrics.font == TypeScaleToken::kCaption);
}

TEST_CASE("NavigationChromeCoordinator navigation history config", "[v22][navigation]")
{
    NavigationChromeCoordinator coord;
    auto config = coord.navigation_history_config();

    REQUIRE(config.button_size > 0);
    REQUIRE(config.corner == CornerRadiusToken::kSm);
    REQUIRE(config.icon_size == IconMetricToken::kSmall);
}

TEST_CASE("NavigationChromeCoordinator wayfinding parity check", "[v22][navigation]")
{
    NavigationChromeCoordinator coord;
    auto findings = coord.wayfinding_parity_check();

    // Should flag known parity issues
    REQUIRE(findings.size() >= 2);

    // Each finding should have all fields populated
    for (const auto& finding : findings)
    {
        REQUIRE_FALSE(finding.surface.empty());
        REQUIRE_FALSE(finding.description.empty());
        REQUIRE_FALSE(finding.suggestion.empty());
    }
}
