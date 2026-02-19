// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/EditorOverlayModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_overlay() -> EditorOverlayModel
{
    EditorOverlayModel model;
    model.set_actions({
        {"bold", "Bold", "Cmd+B", "format", false, true},
        {"italic", "Italic", "Cmd+I", "format", false, true},
        {"link", "Link", "Cmd+K", "insert", false, true},
        {"image", "Image", "", "insert", false, true},
        {"table", "Table", "", "block", false, true},
    });
    return model;
}

TEST_CASE("Actions by group", "[overlay][group]")
{
    auto model = make_test_overlay();
    const auto format_actions = model.actions_by_group("format");
    REQUIRE(format_actions.size() == 2);
}

TEST_CASE("Unique groups sorted", "[overlay][group]")
{
    auto model = make_test_overlay();
    const auto grps = model.groups();
    REQUIRE(grps.size() == 3);
    REQUIRE(grps[0] == "block");
    REQUIRE(grps[1] == "format");
    REQUIRE(grps[2] == "insert");
}

TEST_CASE("Viewport clamping: right edge", "[overlay][position]")
{
    auto pos = EditorOverlayModel::clamped_position(900, 100, 200, 50, {1000, 800});
    REQUIRE(pos.pos_x == 800); // 1000 - 200
    REQUIRE(pos.pos_y == 100);
}

TEST_CASE("Viewport clamping: bottom edge", "[overlay][position]")
{
    auto pos = EditorOverlayModel::clamped_position(100, 780, 200, 50, {1000, 800});
    REQUIRE(pos.pos_x == 100);
    REQUIRE(pos.pos_y == 750); // 800 - 50
}

TEST_CASE("Viewport clamping: fits", "[overlay][position]")
{
    auto pos = EditorOverlayModel::clamped_position(100, 100, 200, 50, {1000, 800});
    REQUIRE(pos.pos_x == 100);
    REQUIRE(pos.pos_y == 100);
}

TEST_CASE("Toggle action active state", "[overlay][state]")
{
    auto model = make_test_overlay();
    model.toggle_action("bold");
    REQUIRE(model.actions()[0].is_active);
    model.toggle_action("bold");
    REQUIRE_FALSE(model.actions()[0].is_active);
}

TEST_CASE("Set action enabled", "[overlay][state]")
{
    auto model = make_test_overlay();
    model.set_action_enabled("table", false);
    REQUIRE_FALSE(model.actions()[4].is_enabled);
}

TEST_CASE("Visibility toggle", "[overlay][visible]")
{
    EditorOverlayModel model;
    REQUIRE_FALSE(model.is_visible());
    model.set_visible(true);
    REQUIRE(model.is_visible());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
