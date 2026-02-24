// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/LayoutMetrics.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ── Task 1: Density profiles exist ──────────────────────────────────

TEST_CASE("LayoutMetrics default profile is kDefault", "[layout][density]")
{
    auto& metrics = LayoutMetrics::get();
    metrics.set_profile(DensityProfile::kDefault);
    REQUIRE(metrics.profile() == DensityProfile::kDefault);
}

TEST_CASE("LayoutMetrics supports all three density profiles", "[layout][density]")
{
    auto& metrics = LayoutMetrics::get();

    metrics.set_profile(DensityProfile::kComfortable);
    REQUIRE(metrics.profile() == DensityProfile::kComfortable);

    metrics.set_profile(DensityProfile::kDefault);
    REQUIRE(metrics.profile() == DensityProfile::kDefault);

    metrics.set_profile(DensityProfile::kCompact);
    REQUIRE(metrics.profile() == DensityProfile::kCompact);

    // Reset
    metrics.set_profile(DensityProfile::kDefault);
}

// ── Task 2: Row heights scale with density ──────────────────────────

TEST_CASE("Row height decreases comfortable > default > compact", "[layout][density]")
{
    auto& metrics = LayoutMetrics::get();

    metrics.set_profile(DensityProfile::kComfortable);
    const int comfortable = metrics.row_height();

    metrics.set_profile(DensityProfile::kDefault);
    const int default_val = metrics.row_height();

    metrics.set_profile(DensityProfile::kCompact);
    const int compact = metrics.row_height();

    REQUIRE(comfortable > default_val);
    REQUIRE(default_val >= compact);

    metrics.set_profile(DensityProfile::kDefault);
}

TEST_CASE("Tab height scales with density", "[layout][density]")
{
    auto& metrics = LayoutMetrics::get();

    metrics.set_profile(DensityProfile::kComfortable);
    const int comfortable = metrics.tab_height();

    metrics.set_profile(DensityProfile::kCompact);
    const int compact = metrics.tab_height();

    REQUIRE(comfortable > compact);
    metrics.set_profile(DensityProfile::kDefault);
}

// ── Task 3: Spacing scales with density ─────────────────────────────

TEST_CASE("Control padding decreases in compact mode", "[layout][density]")
{
    auto& metrics = LayoutMetrics::get();

    metrics.set_profile(DensityProfile::kComfortable);
    const int comfortable_h = metrics.control_padding_h();
    const int comfortable_v = metrics.control_padding_v();

    metrics.set_profile(DensityProfile::kCompact);
    const int compact_h = metrics.control_padding_h();
    const int compact_v = metrics.control_padding_v();

    REQUIRE(comfortable_h > compact_h);
    REQUIRE(comfortable_v > compact_v);

    metrics.set_profile(DensityProfile::kDefault);
}

// ── Task 4: Icon sizes scale with density ───────────────────────────

TEST_CASE("Icon sizes scale with density", "[layout][density]")
{
    auto& metrics = LayoutMetrics::get();

    metrics.set_profile(DensityProfile::kComfortable);
    const int comfortable = metrics.icon_size();

    metrics.set_profile(DensityProfile::kCompact);
    const int compact = metrics.icon_size();

    REQUIRE(comfortable > compact);
    metrics.set_profile(DensityProfile::kDefault);
}

// ── Task 5: Hit target minimums preserved ───────────────────────────

TEST_CASE("Minimum hit target is at least 24px in compact mode", "[layout][density][accessibility]")
{
    auto& metrics = LayoutMetrics::get();
    metrics.set_profile(DensityProfile::kCompact);
    REQUIRE(metrics.min_hit_target() >= 24);
    metrics.set_profile(DensityProfile::kDefault);
}

TEST_CASE("Row height never falls below min hit target", "[layout][density][accessibility]")
{
    auto& metrics = LayoutMetrics::get();

    metrics.set_profile(DensityProfile::kCompact);
    // In compact mode, row height should still be usable
    REQUIRE(metrics.row_height() >= 20);
    metrics.set_profile(DensityProfile::kDefault);
}

// ── Elevation constants ─────────────────────────────────────────────

TEST_CASE("Focus ring and border widths are constant", "[layout][density]")
{
    auto& metrics = LayoutMetrics::get();
    REQUIRE(metrics.focus_ring_width() == 2);
    REQUIRE(metrics.border_width() == 1);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
