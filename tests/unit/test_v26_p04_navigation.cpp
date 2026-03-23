/// @file test_v26_p04_navigation.cpp
/// @brief V26 Phase 04: Navigation and wayfinding convergence tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26ShellConvergence.h"

using namespace markamp::ui;

// ── Tab Convergence Metrics ─────────────────────────────────────────────────

TEST_CASE("V26 P04: Tab height and dimensions", "[v26][p04]")
{
    REQUIRE(TabConvergenceMetrics::kTabHeight == 34);
    REQUIRE(TabConvergenceMetrics::kTabPaddingH > 0);
    REQUIRE(TabConvergenceMetrics::kCloseBtnSize > 0);
    REQUIRE(TabConvergenceMetrics::kModifiedDotSize > 0);
}

TEST_CASE("V26 P04: Tab opacity hierarchy", "[v26][p04]")
{
    REQUIRE(TabConvergenceMetrics::kPreviewOpacity < TabConvergenceMetrics::kInactiveOpacity);
    REQUIRE(TabConvergenceMetrics::kInactiveOpacity < TabConvergenceMetrics::kHoverOpacity);
    REQUIRE(TabConvergenceMetrics::kHoverOpacity <= 1.0F);
}

TEST_CASE("V26 P04: Tab active indicator height", "[v26][p04]")
{
    REQUIRE(TabConvergenceMetrics::kActiveIndicatorH == 2);
    REQUIRE(TabConvergenceMetrics::kPinnedStripeW == 2);
}

TEST_CASE("V26 P04: Tab close button visible for active tab", "[v26][p04]")
{
    REQUIRE(TabConvergenceMetrics::close_btn_visible_for_active());
}

// ── Breadcrumb Convergence Metrics ──────────────────────────────────────────

TEST_CASE("V26 P04: Breadcrumb bar height", "[v26][p04]")
{
    REQUIRE(BreadcrumbConvergenceMetrics::kBarHeight == 24);
    REQUIRE(BreadcrumbConvergenceMetrics::kSegmentPaddingH > 0);
    REQUIRE(BreadcrumbConvergenceMetrics::kSeparatorWidth > 0);
}

TEST_CASE("V26 P04: Breadcrumb max visible segments", "[v26][p04]")
{
    REQUIRE(BreadcrumbConvergenceMetrics::kMaxVisibleSegments > 0);
    REQUIRE(BreadcrumbConvergenceMetrics::kMaxVisibleSegments <= 10);
}

TEST_CASE("V26 P04: Breadcrumb muted opacity", "[v26][p04]")
{
    REQUIRE(BreadcrumbConvergenceMetrics::kMutedOpacity > 0.0F);
    REQUIRE(BreadcrumbConvergenceMetrics::kMutedOpacity < 1.0F);
}

// ── Activity Bar Convergence Metrics ────────────────────────────────────────

TEST_CASE("V26 P04: Activity bar dimensions", "[v26][p04]")
{
    REQUIRE(ActivityBarConvergenceMetrics::kBarWidth == 48);
    REQUIRE(ActivityBarConvergenceMetrics::kSlotHeight == 48);
    REQUIRE(ActivityBarConvergenceMetrics::kIconSize == 24);
}

TEST_CASE("V26 P04: Activity bar active indicator", "[v26][p04]")
{
    REQUIRE(ActivityBarConvergenceMetrics::kActiveIndicatorW > 0);
    REQUIRE(ActivityBarConvergenceMetrics::kActiveIndicatorH > ActivityBarConvergenceMetrics::kActiveIndicatorW);
}

TEST_CASE("V26 P04: Activity bar opacity hierarchy", "[v26][p04]")
{
    REQUIRE(ActivityBarConvergenceMetrics::kInactiveOpacity < ActivityBarConvergenceMetrics::kHoverOpacity);
    REQUIRE(ActivityBarConvergenceMetrics::kHoverOpacity <= 1.0F);
}

// ── Status Bar Convergence Metrics ──────────────────────────────────────────

TEST_CASE("V26 P04: Status bar dimensions", "[v26][p04]")
{
    REQUIRE(StatusBarConvergenceMetrics::kHeight == 24);
    REQUIRE(StatusBarConvergenceMetrics::kItemPaddingH > 0);
    REQUIRE(StatusBarConvergenceMetrics::kIconSize > 0);
    REQUIRE(StatusBarConvergenceMetrics::kProgressBarHeight > 0);
}

TEST_CASE("V26 P04: Status bar separator width", "[v26][p04]")
{
    REQUIRE(StatusBarConvergenceMetrics::kSeparatorW == 1);
}
