/// @file test_v26_p03_action_surfaces.cpp
/// @brief V26 Phase 03: Action surface convergence tests (menus, toolbars, command palette).
#include <catch2/catch_test_macros.hpp>
#include "ui/V26ShellConvergence.h"

using namespace markamp::ui;

// ── Action Surface Metrics ──────────────────────────────────────────────────

TEST_CASE("V26 P03: Toolbar button metrics are consistent", "[v26][p03]")
{
    REQUIRE(ActionSurfaceMetrics::kButtonHeightDefault > ActionSurfaceMetrics::kButtonHeightSmall);
    REQUIRE(ActionSurfaceMetrics::kButtonIconSize > 0);
    REQUIRE(ActionSurfaceMetrics::kButtonPaddingH > 0);
    REQUIRE(ActionSurfaceMetrics::kButtonGroupGap > 0);
}

TEST_CASE("V26 P03: Menu row metrics", "[v26][p03]")
{
    REQUIRE(ActionSurfaceMetrics::kMenuRowHeight == 28);
    REQUIRE(ActionSurfaceMetrics::kMenuPaddingH > 0);
    REQUIRE(ActionSurfaceMetrics::kMenuAcceleratorGap > ActionSurfaceMetrics::kMenuIconWidth);
    REQUIRE(ActionSurfaceMetrics::kMenuBorderRadius > 0);
}

TEST_CASE("V26 P03: Command palette metrics", "[v26][p03]")
{
    REQUIRE(ActionSurfaceMetrics::kPaletteWidth > 0);
    REQUIRE(ActionSurfaceMetrics::kPaletteMaxHeight > ActionSurfaceMetrics::kPaletteInputHeight);
    REQUIRE(ActionSurfaceMetrics::kPaletteRowHeight > 0);
    REQUIRE(ActionSurfaceMetrics::kPaletteBorderRadius > 0);
}

TEST_CASE("V26 P03: Toolbar zone count", "[v26][p03]")
{
    REQUIRE(toolbar_zone_count() == 3);
    REQUIRE(static_cast<int>(ToolbarZone::kLeft) == 0);
    REQUIRE(static_cast<int>(ToolbarZone::kCenter) == 1);
    REQUIRE(static_cast<int>(ToolbarZone::kRight) == 2);
}

TEST_CASE("V26 P03: Menu separator height is odd for centering", "[v26][p03]")
{
    REQUIRE(ActionSurfaceMetrics::kMenuSeparatorH % 2 == 1);
}

TEST_CASE("V26 P03: Palette input height is larger than row height", "[v26][p03]")
{
    REQUIRE(ActionSurfaceMetrics::kPaletteInputHeight > ActionSurfaceMetrics::kPaletteRowHeight);
}
