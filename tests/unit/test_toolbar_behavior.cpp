// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/ToolbarModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_toolbar() -> ToolbarModel
{
    ToolbarModel model;

    model.set_global_actions({
        {"save", "Save", "Save file", "Cmd+S", ToolbarButtonKind::kMomentary, false, true, ""},
        {"wordwrap",
         "Word Wrap",
         "Toggle word wrap",
         "Alt+Z",
         ToolbarButtonKind::kToggle,
         false,
         true,
         ""},
    });

    model.add_context_slot({"editor",
                            {
                                {"format",
                                 "Format",
                                 "Format document",
                                 "Shift+Alt+F",
                                 ToolbarButtonKind::kMomentary,
                                 false,
                                 true,
                                 "editor"},
                            }});

    model.add_context_slot({"canvas",
                            {
                                {"zoom_fit",
                                 "Fit",
                                 "Zoom to fit",
                                 "",
                                 ToolbarButtonKind::kMomentary,
                                 false,
                                 true,
                                 "canvas"},
                                {"grid",
                                 "Grid",
                                 "Toggle grid",
                                 "G",
                                 ToolbarButtonKind::kToggle,
                                 false,
                                 true,
                                 "canvas"},
                            }});

    return model;
}

// ── Phase 09 Task 1: Contextual action slots ────────────────────────

TEST_CASE("Visible buttons change with mode", "[toolbar][context]")
{
    auto model = make_test_toolbar();

    model.set_mode("editor");
    REQUIRE(model.visible_buttons().size() == 3); // 2 global + 1 editor

    model.set_mode("canvas");
    REQUIRE(model.visible_buttons().size() == 4); // 2 global + 2 canvas
}

TEST_CASE("Unknown mode shows only global buttons", "[toolbar][context]")
{
    auto model = make_test_toolbar();
    model.set_mode("unknown");
    REQUIRE(model.visible_buttons().size() == 2);
}

// ── Phase 09 Task 2: Toggle semantics ───────────────────────────────

TEST_CASE("Toggle flips button state", "[toolbar][toggle]")
{
    auto model = make_test_toolbar();
    REQUIRE(model.toggle("wordwrap"));
    const auto& globals = model.global_buttons();
    REQUIRE(globals[1].is_toggled);
}

TEST_CASE("Toggle on momentary button returns false", "[toolbar][toggle]")
{
    auto model = make_test_toolbar();
    REQUIRE_FALSE(model.toggle("save")); // momentary — no toggle
}

// ── Phase 09 Task 3: Overflow ───────────────────────────────────────

TEST_CASE("Toolbar overflow detects hidden buttons", "[toolbar][overflow]")
{
    auto model = make_test_toolbar();
    model.set_mode("canvas");
    // 4 buttons × 60px = 240px, viewport = 150px → 2 overflowed
    const auto overflow = model.overflowed_indices(150, 60);
    REQUIRE(overflow.size() == 2);
}

// ── Phase 09 Task 4: Tooltip generation ─────────────────────────────

TEST_CASE("Tooltip includes shortcut and toggle state", "[toolbar][tooltip]")
{
    ToolbarButtonModel btn;
    btn.tooltip = "Toggle word wrap";
    btn.shortcut_hint = "Alt+Z";
    btn.kind = ToolbarButtonKind::kToggle;
    btn.is_toggled = true;

    const auto tip = ToolbarModel::build_tooltip(btn);
    REQUIRE(tip.find("Alt+Z") != std::string::npos);
    REQUIRE(tip.find("[ON]") != std::string::npos);
}

TEST_CASE("Tooltip for momentary button omits toggle state", "[toolbar][tooltip]")
{
    ToolbarButtonModel btn;
    btn.tooltip = "Save file";
    btn.shortcut_hint = "Cmd+S";
    btn.kind = ToolbarButtonKind::kMomentary;

    const auto tip = ToolbarModel::build_tooltip(btn);
    REQUIRE(tip.find("[ON]") == std::string::npos);
    REQUIRE(tip.find("Cmd+S") != std::string::npos);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
