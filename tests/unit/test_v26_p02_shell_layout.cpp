/// @file test_v26_p02_shell_layout.cpp
/// @brief V26 Phase 02: Shell frame layout rhythm and surface layering tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26ShellConvergence.h"
#include "core/V26PremiumTokens.h"

using namespace markamp::ui;
using namespace markamp::core;

// ── Shell Layout Policy ─────────────────────────────────────────────────────

TEST_CASE("V26 P02: Sidebar width range is valid", "[v26][p02]")
{
    REQUIRE(ShellLayoutPolicy::kSidebarMinWidth > 0);
    REQUIRE(ShellLayoutPolicy::kSidebarDefaultWidth > ShellLayoutPolicy::kSidebarMinWidth);
    REQUIRE(ShellLayoutPolicy::kSidebarMaxWidth > ShellLayoutPolicy::kSidebarDefaultWidth);
    REQUIRE(ShellLayoutPolicy::kSidebarCollapseWidth < ShellLayoutPolicy::kSidebarMinWidth);
}

TEST_CASE("V26 P02: Sidebar width validation", "[v26][p02]")
{
    REQUIRE(ShellLayoutPolicy::is_sidebar_width_valid(256));
    REQUIRE(ShellLayoutPolicy::is_sidebar_width_valid(180));
    REQUIRE(ShellLayoutPolicy::is_sidebar_width_valid(400));
    REQUIRE_FALSE(ShellLayoutPolicy::is_sidebar_width_valid(179));
    REQUIRE_FALSE(ShellLayoutPolicy::is_sidebar_width_valid(401));
}

TEST_CASE("V26 P02: Sidebar width clamping", "[v26][p02]")
{
    REQUIRE(ShellLayoutPolicy::clamp_sidebar_width(50) == 180);
    REQUIRE(ShellLayoutPolicy::clamp_sidebar_width(300) == 300);
    REQUIRE(ShellLayoutPolicy::clamp_sidebar_width(999) == 400);
}

TEST_CASE("V26 P02: Bottom panel height clamping", "[v26][p02]")
{
    REQUIRE(ShellLayoutPolicy::clamp_bottom_panel_height(50) == 100);
    REQUIRE(ShellLayoutPolicy::clamp_bottom_panel_height(300) == 300);
    REQUIRE(ShellLayoutPolicy::clamp_bottom_panel_height(999) == 600);
}

TEST_CASE("V26 P02: Activity bar dimensions", "[v26][p02]")
{
    REQUIRE(ShellLayoutPolicy::kActivityBarWidth == 48);
    REQUIRE(ShellLayoutPolicy::kActivityBarSlotH == 48);
    REQUIRE(ShellLayoutPolicy::kActivityBarIconSize == 24);
}

TEST_CASE("V26 P02: Toolbar height", "[v26][p02]")
{
    REQUIRE(ShellLayoutPolicy::kToolbarHeight == 40);
    REQUIRE(ShellLayoutPolicy::kToolbarMinButtonW == 28);
}

TEST_CASE("V26 P02: Tab bar height", "[v26][p02]")
{
    REQUIRE(ShellLayoutPolicy::kTabBarHeight == 34);
    REQUIRE(ShellLayoutPolicy::kTabMinW < ShellLayoutPolicy::kTabMaxW);
}

TEST_CASE("V26 P02: Shell surface assignment count", "[v26][p02]")
{
    REQUIRE(shell_surface_assignment_count() == 9);
}

TEST_CASE("V26 P02: Shell surface assignment enum range", "[v26][p02]")
{
    REQUIRE(static_cast<int>(ShellSurfaceAssignment::kShellFrame) == 0);
    REQUIRE(static_cast<int>(ShellSurfaceAssignment::kDialog) == 8);
}

TEST_CASE("V26 P02: Splitter metrics", "[v26][p02]")
{
    REQUIRE(ShellLayoutPolicy::kSplitterVisualWidth < ShellLayoutPolicy::kSplitterHitWidth);
}

TEST_CASE("V26 P02: Breadcrumb bar height", "[v26][p02]")
{
    REQUIRE(ShellLayoutPolicy::kBreadcrumbBarHeight == 24);
}
