/// @file test_v26_p05_panels.cpp
/// @brief V26 Phase 05: Panels, inspectors, and dense information chrome tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26PanelConvergence.h"

using namespace markamp::ui;

// ── Panel Header ────────────────────────────────────────────────────────────

TEST_CASE("V26 P05: Panel header dimensions", "[v26][p05]")
{
    REQUIRE(PanelHeaderV26::kHeight == 32);
    REQUIRE(PanelHeaderV26::kPaddingH > 0);
    REQUIRE(PanelHeaderV26::kTitleFontPt > PanelHeaderV26::kSubtitleFontPt);
    REQUIRE(PanelHeaderV26::kDividerH == 1);
}

TEST_CASE("V26 P05: Panel header action sizing", "[v26][p05]")
{
    REQUIRE(PanelHeaderV26::kActionSize < PanelHeaderV26::kHeight);
    REQUIRE(PanelHeaderV26::kActionGap > 0);
    REQUIRE(PanelHeaderV26::kCollapseIconSize < PanelHeaderV26::kActionSize);
}

// ── Dense Row Metrics ───────────────────────────────────────────────────────

TEST_CASE("V26 P05: Dense row height hierarchy", "[v26][p05]")
{
    REQUIRE(DenseRowMetrics::kRowHeightCompact < DenseRowMetrics::kRowHeightDefault);
    REQUIRE(DenseRowMetrics::kRowHeightDefault < DenseRowMetrics::kRowHeightComfort);
}

TEST_CASE("V26 P05: Dense row height density resolver", "[v26][p05]")
{
    REQUIRE(DenseRowMetrics::row_height_for_density(0) == 28); // comfort
    REQUIRE(DenseRowMetrics::row_height_for_density(1) == 24); // default
    REQUIRE(DenseRowMetrics::row_height_for_density(2) == 20); // compact
}

TEST_CASE("V26 P05: Dense row tree indent", "[v26][p05]")
{
    REQUIRE(DenseRowMetrics::kIndentStep == 16);
    REQUIRE(DenseRowMetrics::kIconSize == 16);
    REQUIRE(DenseRowMetrics::kIconTextGap > 0);
}

TEST_CASE("V26 P05: Dense row badge metrics", "[v26][p05]")
{
    REQUIRE(DenseRowMetrics::kBadgeHeight > 0);
    REQUIRE(DenseRowMetrics::kBadgePaddingH > 0);
    REQUIRE(DenseRowMetrics::kBadgeRadius > 0);
}

// ── Inspector Row Metrics ───────────────────────────────────────────────────

TEST_CASE("V26 P05: Inspector row dimensions", "[v26][p05]")
{
    REQUIRE(InspectorRowMetrics::kRowMinHeight > 0);
    REQUIRE(InspectorRowMetrics::kLabelWidth > 0);
    REQUIRE(InspectorRowMetrics::kActionWidth > 0);
}

TEST_CASE("V26 P05: Inspector row group hierarchy", "[v26][p05]")
{
    REQUIRE(InspectorRowMetrics::kCategoryGap > InspectorRowMetrics::kGroupGap);
    REQUIRE(InspectorRowMetrics::kGroupHeaderH > 0);
}

// ── Empty State Metrics ─────────────────────────────────────────────────────

TEST_CASE("V26 P05: Empty state role count", "[v26][p05]")
{
    REQUIRE(EmptyStateMetrics::role_count() == 3);
}

TEST_CASE("V26 P05: Empty state full-page is largest", "[v26][p05]")
{
    REQUIRE(EmptyStateMetrics::kFullPageIconSize > EmptyStateMetrics::kPanelIconSize);
    REQUIRE(EmptyStateMetrics::kFullPageTitlePt > EmptyStateMetrics::kPanelTitlePt);
}

TEST_CASE("V26 P05: Empty state role enum values", "[v26][p05]")
{
    REQUIRE(static_cast<int>(EmptyStateRole::kFullPage) == 0);
    REQUIRE(static_cast<int>(EmptyStateRole::kPanel) == 1);
    REQUIRE(static_cast<int>(EmptyStateRole::kInline) == 2);
}

// ── Panel Action Bar Metrics ────────────────────────────────────────────────

TEST_CASE("V26 P05: Panel action bar dimensions", "[v26][p05]")
{
    REQUIRE(PanelActionBarMetrics::kHeight == 28);
    REQUIRE(PanelActionBarMetrics::kButtonSize < PanelActionBarMetrics::kHeight);
    REQUIRE(PanelActionBarMetrics::kFilterChipH < PanelActionBarMetrics::kHeight);
}

TEST_CASE("V26 P05: Panel action bar search input", "[v26][p05]")
{
    REQUIRE(PanelActionBarMetrics::kSearchInputH == 24);
    REQUIRE(PanelActionBarMetrics::kFilterChipRadius > 0);
}
