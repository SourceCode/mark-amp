// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/ContextMenuModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_menu() -> ContextMenuModel
{
    ContextMenuModel model;
    model.set_items({
        {"copy", "Copy", "Cmd+C", MenuActionCategory::kEdit, true, true, false},
        {"paste", "Paste", "Cmd+V", MenuActionCategory::kEdit, true, true, false},
        {"open", "Open", "Cmd+O", MenuActionCategory::kNavigation, true, true, false},
        {"new_file", "New File", "Cmd+N", MenuActionCategory::kCreate, true, true, false},
        {"", "", "", MenuActionCategory::kOther, false, true, true}, // separator
        {"delete", "Delete", "Del", MenuActionCategory::kEdit, true, true, false},
    });
    return model;
}

TEST_CASE("Items sort by category", "[contextmenu][sort]")
{
    auto model = make_test_menu();
    const auto sorted = model.sorted_items();
    REQUIRE(sorted[0].action_id == "open"); // kNavigation first
    REQUIRE(sorted[1].action_id == "copy"); // kEdit next
}

TEST_CASE("Visible items filter hidden", "[contextmenu][visible]")
{
    auto model = make_test_menu();
    model.set_visible("paste", false);
    const auto vis = model.visible_items();
    for (const auto& item : vis)
    {
        REQUIRE(item.action_id != "paste");
    }
}

TEST_CASE("Dynamic enablement", "[contextmenu][enable]")
{
    auto model = make_test_menu();
    model.set_enabled("copy", false);
    const auto items = model.visible_items();
    for (const auto& item : items)
    {
        if (item.action_id == "copy")
        {
            REQUIRE_FALSE(item.is_enabled);
        }
    }
}

TEST_CASE("Empty state detection", "[contextmenu][empty]")
{
    ContextMenuModel model;
    model.set_items({
        {"copy", "Copy", "", MenuActionCategory::kEdit, false, true, false},
    });
    REQUIRE(model.is_empty_state());
}

TEST_CASE("Empty area fallbacks have content", "[contextmenu][fallback]")
{
    const auto fallbacks = ContextMenuModel::empty_area_fallbacks();
    REQUIRE(fallbacks.size() >= 3);
}

TEST_CASE("Action count excludes separators", "[contextmenu][count]")
{
    auto model = make_test_menu();
    REQUIRE(model.action_count() == 5); // 6 items - 1 separator
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
