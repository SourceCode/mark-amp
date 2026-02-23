// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/FocusManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ── Task 1: Focus zone order is deterministic ───────────────────────

TEST_CASE("FocusManager zone order is deterministic", "[focus][keyboard]")
{
    auto& fm = FocusManager::get();

    // Enable all zones
    fm.set_zone_enabled(FocusZoneId::kMenuBar, true);
    fm.set_zone_enabled(FocusZoneId::kActivityBar, true);
    fm.set_zone_enabled(FocusZoneId::kSidebar, true);
    fm.set_zone_enabled(FocusZoneId::kEditorArea, true);
    fm.set_zone_enabled(FocusZoneId::kBottomPanel, true);
    fm.set_zone_enabled(FocusZoneId::kStatusBar, true);
    fm.set_zone_enabled(FocusZoneId::kBreadcrumb, true);

    const auto order = fm.zone_order();
    REQUIRE(order.size() == 7);
    // MenuBar should be first
    REQUIRE(order.front() == FocusZoneId::kMenuBar);
    // StatusBar should be last
    REQUIRE(order.back() == FocusZoneId::kStatusBar);
}

// ── Task 1: Tab traversal is cyclical ───────────────────────────────

TEST_CASE("Tab traversal cycles through all zones", "[focus][keyboard]")
{
    auto& fm = FocusManager::get();

    // Enable all zones
    fm.set_zone_enabled(FocusZoneId::kMenuBar, true);
    fm.set_zone_enabled(FocusZoneId::kActivityBar, true);
    fm.set_zone_enabled(FocusZoneId::kSidebar, true);
    fm.set_zone_enabled(FocusZoneId::kEditorArea, true);
    fm.set_zone_enabled(FocusZoneId::kBottomPanel, true);
    fm.set_zone_enabled(FocusZoneId::kStatusBar, true);
    fm.set_zone_enabled(FocusZoneId::kBreadcrumb, true);

    fm.set_zone(FocusZoneId::kMenuBar);

    const auto order = fm.zone_order();
    for (std::size_t idx = 1; idx < order.size(); ++idx)
    {
        fm.advance(FocusDirection::kForward);
        REQUIRE(fm.current_zone() == order[idx]);
    }

    // One more should wrap to first
    fm.advance(FocusDirection::kForward);
    REQUIRE(fm.current_zone() == order.front());
}

TEST_CASE("Shift+Tab traversal goes backward", "[focus][keyboard]")
{
    auto& fm = FocusManager::get();

    // Enable all zones
    fm.set_zone_enabled(FocusZoneId::kMenuBar, true);
    fm.set_zone_enabled(FocusZoneId::kActivityBar, true);
    fm.set_zone_enabled(FocusZoneId::kSidebar, true);
    fm.set_zone_enabled(FocusZoneId::kEditorArea, true);
    fm.set_zone_enabled(FocusZoneId::kBottomPanel, true);
    fm.set_zone_enabled(FocusZoneId::kStatusBar, true);
    fm.set_zone_enabled(FocusZoneId::kBreadcrumb, true);

    fm.set_zone(FocusZoneId::kMenuBar);
    fm.advance(FocusDirection::kBackward);
    // Should wrap to last zone (StatusBar)
    REQUIRE(fm.current_zone() == FocusZoneId::kStatusBar);
}

// ── Task 1: Disabled zones are skipped ──────────────────────────────

TEST_CASE("Disabled zones are skipped during traversal", "[focus][keyboard]")
{
    auto& fm = FocusManager::get();

    // Enable all, then disable sidebar
    fm.set_zone_enabled(FocusZoneId::kMenuBar, true);
    fm.set_zone_enabled(FocusZoneId::kActivityBar, true);
    fm.set_zone_enabled(FocusZoneId::kSidebar, false);
    fm.set_zone_enabled(FocusZoneId::kEditorArea, true);
    fm.set_zone_enabled(FocusZoneId::kBottomPanel, true);
    fm.set_zone_enabled(FocusZoneId::kStatusBar, true);
    fm.set_zone_enabled(FocusZoneId::kBreadcrumb, true);

    // Sidebar should not appear in order
    const auto order = fm.zone_order();
    for (auto zone : order)
    {
        REQUIRE(zone != FocusZoneId::kSidebar);
    }

    // Tab from ActivityBar should skip Sidebar
    fm.set_zone(FocusZoneId::kActivityBar);
    fm.advance(FocusDirection::kForward);
    REQUIRE(fm.current_zone() != FocusZoneId::kSidebar);

    // Re-enable
    fm.set_zone_enabled(FocusZoneId::kSidebar, true);
}

// ── Task 2: Arrow key behavior per zone ─────────────────────────────

TEST_CASE("Arrow key behavior matches control type", "[focus][keyboard]")
{
    REQUIRE(FocusManager::arrow_behavior(FocusZoneId::kActivityBar) ==
            ArrowKeyBehavior::kVerticalList);
    REQUIRE(FocusManager::arrow_behavior(FocusZoneId::kSidebar) == ArrowKeyBehavior::kVerticalList);
    REQUIRE(FocusManager::arrow_behavior(FocusZoneId::kMenuBar) ==
            ArrowKeyBehavior::kHorizontalStrip);
    REQUIRE(FocusManager::arrow_behavior(FocusZoneId::kStatusBar) ==
            ArrowKeyBehavior::kHorizontalStrip);
    REQUIRE(FocusManager::arrow_behavior(FocusZoneId::kEditorArea) == ArrowKeyBehavior::kGrid);
}

// ── Task 3: Focus restoration after overlay close ───────────────────

TEST_CASE("Focus snapshot restores previous zone", "[focus][keyboard]")
{
    auto& fm = FocusManager::get();
    fm.set_zone(FocusZoneId::kEditorArea);

    // Simulate opening a dialog
    fm.push_snapshot();
    fm.set_zone(FocusZoneId::kBottomPanel); // Dialog changes focus

    // Close dialog → restore
    fm.restore();
    REQUIRE(fm.current_zone() == FocusZoneId::kEditorArea);
}

TEST_CASE("Nested snapshots restore correctly", "[focus][keyboard]")
{
    auto& fm = FocusManager::get();
    fm.set_zone(FocusZoneId::kSidebar);

    fm.push_snapshot(); // Level 1
    fm.set_zone(FocusZoneId::kEditorArea);

    fm.push_snapshot(); // Level 2
    fm.set_zone(FocusZoneId::kStatusBar);

    fm.restore(); // Back to Level 1 focus
    REQUIRE(fm.current_zone() == FocusZoneId::kEditorArea);

    fm.restore(); // Back to original
    REQUIRE(fm.current_zone() == FocusZoneId::kSidebar);
}

TEST_CASE("Restore with empty stack is a no-op", "[focus][keyboard]")
{
    auto& fm = FocusManager::get();
    fm.set_zone(FocusZoneId::kActivityBar);
    fm.restore(); // No snapshot pushed — should not crash
    REQUIRE(fm.current_zone() == FocusZoneId::kActivityBar);
}
// ── Task 4: Item-level focus tracking ─────────────────────────────────

TEST_CASE("FocusManager tracks item-level focus", "[focus][keyboard]")
{
    auto& fm = FocusManager::get();

    fm.set_focus(FocusZoneId::kActivityBar, 3);
    REQUIRE(fm.current_zone() == FocusZoneId::kActivityBar);
    REQUIRE(fm.current_item() == 3);

    // Changing zone should reset item to -1
    fm.set_zone_enabled(FocusZoneId::kSidebar, true);
    fm.advance(FocusDirection::kForward);
    REQUIRE(fm.current_zone() != FocusZoneId::kActivityBar);
    REQUIRE(fm.current_item() == -1);

    // set_item updates just the item index
    fm.set_item(5);
    REQUIRE(fm.current_item() == 5);
}

TEST_CASE("FocusManager item-level focus snapshots", "[focus][keyboard]")
{
    auto& fm = FocusManager::get();

    fm.set_focus(FocusZoneId::kSidebar, 12);
    fm.push_snapshot();

    fm.set_focus(FocusZoneId::kEditorArea, 1);

    fm.restore();
    REQUIRE(fm.current_zone() == FocusZoneId::kSidebar);
    REQUIRE(fm.current_item() == 12);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
