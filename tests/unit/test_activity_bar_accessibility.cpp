// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/ActivityBarModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_model() -> ActivityBarModel
{
    ActivityBarModel model;
    model.add_item({"explorer",
                    "File Explorer",
                    "Cmd+Shift+E",
                    "File Explorer",
                    "activity-explorer",
                    false,
                    BadgeStyle::kNone,
                    0,
                    true,
                    0});
    model.add_item({"search",
                    "Search",
                    "Cmd+Shift+F",
                    "Search",
                    "activity-search",
                    false,
                    BadgeStyle::kNone,
                    0,
                    true,
                    0});
    model.add_item({"settings",
                    "Settings",
                    "Cmd+,",
                    "Settings",
                    "activity-settings",
                    true,
                    BadgeStyle::kNone,
                    0,
                    true,
                    0});
    model.add_item({"themes",
                    "Themes",
                    "",
                    "Theme Gallery",
                    "toolbar-themes",
                    true,
                    BadgeStyle::kNone,
                    0,
                    true,
                    0});
    return model;
}

// ── Phase 06 Task 1: Keyboard nav ───────────────────────────────────

TEST_CASE("ActivityBar keyboard focus wraps forward", "[activitybar][keyboard]")
{
    auto model = make_test_model();
    model.set_focus(3);
    model.focus_next();
    REQUIRE(model.focus_index() == 0);
}

TEST_CASE("ActivityBar keyboard focus wraps backward", "[activitybar][keyboard]")
{
    auto model = make_test_model();
    model.set_focus(0);
    model.focus_previous();
    REQUIRE(model.focus_index() == 3);
}

TEST_CASE("Enter activates focused item", "[activitybar][keyboard]")
{
    auto model = make_test_model();
    model.set_focus(1);
    model.activate_focused();
    REQUIRE(model.active_item_id() == "search");
}

// ── Phase 06 Task 2: Drag reorder ───────────────────────────────────

TEST_CASE("Drag reorder swaps items", "[activitybar][drag]")
{
    auto model = make_test_model();
    model.reorder(0, 2);
    const auto vis = model.visible_items();
    REQUIRE(vis[0].item_id != "explorer");
}

// ── Phase 06 Task 3: Badge display ──────────────────────────────────

TEST_CASE("Badge display caps at 99+", "[activitybar][badge]")
{
    REQUIRE(ActivityBarModel::badge_display(BadgeStyle::kCount, 5) == "5");
    REQUIRE(ActivityBarModel::badge_display(BadgeStyle::kCount, 100) == "99+");
    REQUIRE(ActivityBarModel::badge_display(BadgeStyle::kDot, 0) == "●");
    REQUIRE(ActivityBarModel::badge_display(BadgeStyle::kUrgent, 0) == "!");
    REQUIRE(ActivityBarModel::badge_display(BadgeStyle::kNone, 0).empty());
}

TEST_CASE("Badge can be set on item", "[activitybar][badge]")
{
    auto model = make_test_model();
    model.set_badge("search", BadgeStyle::kCount, 42);
    const auto vis = model.visible_items();
    REQUIRE(vis[1].badge_count == 42);
    REQUIRE(vis[1].badge_style == BadgeStyle::kCount);
}

// ── Phase 06 Task 4: Visibility/context menu ────────────────────────

TEST_CASE("Hiding item removes from visible list", "[activitybar][visibility]")
{
    auto model = make_test_model();
    REQUIRE(model.visible_items().size() == 4);
    model.set_item_visible("themes", false);
    REQUIRE(model.visible_items().size() == 3);
}

// ── Phase 06 Task 5: Accessibility ──────────────────────────────────

TEST_CASE("Focused announcement includes label and badge", "[activitybar][a11y]")
{
    auto model = make_test_model();
    model.set_badge("search", BadgeStyle::kCount, 7);
    model.set_focus(1);
    const auto announcement = model.focused_announcement();
    REQUIRE(announcement.find("Search") != std::string::npos);
    REQUIRE(announcement.find("7") != std::string::npos);
}

TEST_CASE("Focused announcement notes active state", "[activitybar][a11y]")
{
    auto model = make_test_model();
    model.set_active("explorer");
    model.set_focus(0);
    const auto announcement = model.focused_announcement();
    REQUIRE(announcement.find("active") != std::string::npos);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
