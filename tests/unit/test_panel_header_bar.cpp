/**
 * @file test_panel_header_bar.cpp
 * @brief Phase 27: Unit tests for PanelHeaderBar layout, actions, and per-panel wiring.
 */

#include "core/EventBus.h"
#include "ui/PanelHeaderBar.h"
#include "ui/PanelHeaderIcons.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <set>

// ═══════════════════════════════════════════════════════
// PanelHeaderAction data structure
// ═══════════════════════════════════════════════════════

TEST_CASE("PanelHeaderAction default values", "[panel_header]")
{
    markamp::ui::PanelHeaderAction action;
    CHECK(action.action_id.empty());
    CHECK(action.icon_name.empty());
    CHECK(action.tooltip.empty());
    CHECK(action.shortcut.empty());
    CHECK_FALSE(action.is_toggle);
    CHECK_FALSE(action.is_toggled_on);
    CHECK(action.is_enabled);
    CHECK_FALSE(action.separator_before);
}

TEST_CASE("PanelHeaderAction with values", "[panel_header]")
{
    markamp::ui::PanelHeaderAction action;
    action.action_id = "test.action";
    action.icon_name = "test-icon";
    action.tooltip = "Test Action";
    action.shortcut = "Ctrl+T";
    action.is_toggle = true;
    action.is_toggled_on = true;
    action.separator_before = true;

    CHECK(action.action_id == "test.action");
    CHECK(action.icon_name == "test-icon");
    CHECK(action.tooltip == "Test Action");
    CHECK(action.shortcut == "Ctrl+T");
    CHECK(action.is_toggle);
    CHECK(action.is_toggled_on);
    CHECK(action.separator_before);
}

// ═══════════════════════════════════════════════════════
// Explorer panel actions
// ═══════════════════════════════════════════════════════

TEST_CASE("Explorer actions", "[panel_header][explorer]")
{
    auto actions = markamp::ui::panel_actions::explorer_actions();

    REQUIRE(actions.size() == 6);

    CHECK(actions[0].action_id == "explorer.new_file");
    CHECK(actions[0].tooltip == "New File");
    CHECK(actions[0].shortcut == "Ctrl+N");

    CHECK(actions[1].action_id == "explorer.new_folder");
    CHECK(actions[2].action_id == "explorer.collapse_all");
    CHECK(actions[2].separator_before);

    CHECK(actions[3].action_id == "explorer.refresh");
    CHECK(actions[4].action_id == "explorer.filter");
    CHECK(actions[4].is_toggle);

    CHECK(actions[5].action_id == "explorer.open_terminal");
}

// ═══════════════════════════════════════════════════════
// Search panel actions
// ═══════════════════════════════════════════════════════

TEST_CASE("Search actions", "[panel_header][search]")
{
    auto actions = markamp::ui::panel_actions::search_actions();

    REQUIRE(actions.size() == 6);

    CHECK(actions[0].action_id == "search.clear");
    CHECK(actions[1].action_id == "search.collapse");

    // Regex, Case, Whole Word should all be toggles
    CHECK(actions[2].action_id == "search.regex");
    CHECK(actions[2].is_toggle);
    CHECK(actions[2].shortcut == "Alt+R");

    CHECK(actions[3].action_id == "search.case");
    CHECK(actions[3].is_toggle);

    CHECK(actions[4].action_id == "search.whole_word");
    CHECK(actions[4].is_toggle);

    CHECK(actions[5].action_id == "search.open_editor");
}

// ═══════════════════════════════════════════════════════
// Output panel actions
// ═══════════════════════════════════════════════════════

TEST_CASE("Output actions", "[panel_header][output]")
{
    auto actions = markamp::ui::panel_actions::output_actions();

    REQUIRE(actions.size() == 5);

    CHECK(actions[0].action_id == "output.clear");
    CHECK(actions[1].action_id == "output.lock_scroll");
    CHECK(actions[1].is_toggle);
    CHECK(actions[2].action_id == "output.word_wrap");
    CHECK(actions[2].is_toggle);
    CHECK(actions[3].action_id == "output.timestamps");
    CHECK(actions[3].is_toggle);
    CHECK(actions[4].action_id == "output.open_editor");
}

// ═══════════════════════════════════════════════════════
// Problems panel actions
// ═══════════════════════════════════════════════════════

TEST_CASE("Problems actions", "[panel_header][problems]")
{
    auto actions = markamp::ui::panel_actions::problems_actions();

    REQUIRE(actions.size() == 5);

    // Severity filters should be toggles and default to on
    CHECK(actions[0].action_id == "problems.filter_error");
    CHECK(actions[0].is_toggle);
    CHECK(actions[0].is_toggled_on);

    CHECK(actions[1].action_id == "problems.filter_warning");
    CHECK(actions[1].is_toggle);
    CHECK(actions[1].is_toggled_on);

    CHECK(actions[2].action_id == "problems.filter_info");
    CHECK(actions[2].is_toggle);
    CHECK(actions[2].is_toggled_on);

    CHECK(actions[3].action_id == "problems.collapse_all");
    CHECK(actions[4].action_id == "problems.quick_fix_all");
}

// ═══════════════════════════════════════════════════════
// Terminal panel actions
// ═══════════════════════════════════════════════════════

TEST_CASE("Terminal actions", "[panel_header][terminal]")
{
    auto actions = markamp::ui::panel_actions::terminal_actions();

    REQUIRE(actions.size() == 5);

    CHECK(actions[0].action_id == "terminal.new");
    CHECK(actions[0].shortcut == "Ctrl+Shift+`");

    CHECK(actions[1].action_id == "terminal.split");
    CHECK(actions[2].action_id == "terminal.kill");
    CHECK(actions[2].separator_before);
    CHECK(actions[3].action_id == "terminal.scroll_bottom");
    CHECK(actions[4].action_id == "terminal.clear");
}

// ═══════════════════════════════════════════════════════
// Build panel actions
// ═══════════════════════════════════════════════════════

TEST_CASE("Build actions", "[panel_header][build]")
{
    auto actions = markamp::ui::panel_actions::build_actions();

    REQUIRE(actions.size() == 5);

    CHECK(actions[0].action_id == "build.build");
    CHECK(actions[0].shortcut == "Ctrl+Shift+B");

    CHECK(actions[1].action_id == "build.rebuild");
    CHECK(actions[2].action_id == "build.clean");
    CHECK(actions[3].action_id == "build.configure");

    // Build stop should be disabled initially
    CHECK(actions[4].action_id == "build.stop");
    CHECK_FALSE(actions[4].is_enabled);
}

// ═══════════════════════════════════════════════════════
// Debug Console panel actions
// ═══════════════════════════════════════════════════════

TEST_CASE("Debug Console actions", "[panel_header][debug_console]")
{
    auto actions = markamp::ui::panel_actions::debug_console_actions();

    REQUIRE(actions.size() == 3);

    CHECK(actions[0].action_id == "debug_console.clear");
    CHECK(actions[1].action_id == "debug_console.log_level");
    CHECK(actions[2].action_id == "debug_console.timestamps");
    CHECK(actions[2].is_toggle);
}

// ═══════════════════════════════════════════════════════
// PanelHeaderBar action callback
// ═══════════════════════════════════════════════════════

TEST_CASE("PanelHeaderBar callback wiring", "[panel_header][callback]")
{
    std::string clicked_id;
    markamp::ui::PanelActionCallback callback = [&clicked_id](const std::string& action_id)
    { clicked_id = action_id; };

    // Just verify the callback signature works
    callback("test.action");
    CHECK(clicked_id == "test.action");
}

// ═══════════════════════════════════════════════════════
// PanelHeaderAction toggle state
// ═══════════════════════════════════════════════════════

TEST_CASE("PanelHeaderAction toggle state management", "[panel_header][toggle]")
{
    markamp::ui::PanelHeaderAction toggle_action;
    toggle_action.action_id = "search.regex";
    toggle_action.is_toggle = true;
    toggle_action.is_toggled_on = false;

    CHECK_FALSE(toggle_action.is_toggled_on);

    toggle_action.is_toggled_on = true;
    CHECK(toggle_action.is_toggled_on);
}

// ═══════════════════════════════════════════════════════
// Action uniqueness across panels
// ═══════════════════════════════════════════════════════

TEST_CASE("All panel action IDs are unique", "[panel_header][uniqueness]")
{
    auto all_actions = markamp::ui::panel_actions::explorer_actions();
    auto search = markamp::ui::panel_actions::search_actions();
    auto output = markamp::ui::panel_actions::output_actions();
    auto problems = markamp::ui::panel_actions::problems_actions();
    auto terminal = markamp::ui::panel_actions::terminal_actions();
    auto build = markamp::ui::panel_actions::build_actions();
    auto debug = markamp::ui::panel_actions::debug_console_actions();

    all_actions.insert(all_actions.end(), search.begin(), search.end());
    all_actions.insert(all_actions.end(), output.begin(), output.end());
    all_actions.insert(all_actions.end(), problems.begin(), problems.end());
    all_actions.insert(all_actions.end(), terminal.begin(), terminal.end());
    all_actions.insert(all_actions.end(), build.begin(), build.end());
    all_actions.insert(all_actions.end(), debug.begin(), debug.end());

    std::set<std::string> seen_ids;
    for (const auto& action : all_actions)
    {
        CHECK(seen_ids.find(action.action_id) == seen_ids.end());
        seen_ids.insert(action.action_id);
    }
}

// ═══════════════════════════════════════════════════════
// Separator placement validation
// ═══════════════════════════════════════════════════════

TEST_CASE("Separators are not first action", "[panel_header][separators]")
{
    auto check_panel = [](const std::vector<markamp::ui::PanelHeaderAction>& actions,
                          const std::string& panel_name)
    {
        INFO("Checking panel: " << panel_name);
        if (!actions.empty())
        {
            CHECK_FALSE(actions.front().separator_before);
        }
    };

    check_panel(markamp::ui::panel_actions::explorer_actions(), "Explorer");
    check_panel(markamp::ui::panel_actions::search_actions(), "Search");
    check_panel(markamp::ui::panel_actions::output_actions(), "Output");
    check_panel(markamp::ui::panel_actions::problems_actions(), "Problems");
    check_panel(markamp::ui::panel_actions::terminal_actions(), "Terminal");
    check_panel(markamp::ui::panel_actions::build_actions(), "Build");
    check_panel(markamp::ui::panel_actions::debug_console_actions(), "Debug Console");
}

// ═══════════════════════════════════════════════════════
// All actions have icon names
// ═══════════════════════════════════════════════════════

TEST_CASE("All actions have icon names", "[panel_header][icons]")
{
    auto check_icons = [](const std::vector<markamp::ui::PanelHeaderAction>& actions,
                          const std::string& panel_name)
    {
        for (const auto& action : actions)
        {
            INFO("Panel: " << panel_name << ", Action: " << action.action_id);
            CHECK_FALSE(action.icon_name.empty());
        }
    };

    check_icons(markamp::ui::panel_actions::explorer_actions(), "Explorer");
    check_icons(markamp::ui::panel_actions::search_actions(), "Search");
    check_icons(markamp::ui::panel_actions::output_actions(), "Output");
    check_icons(markamp::ui::panel_actions::problems_actions(), "Problems");
    check_icons(markamp::ui::panel_actions::terminal_actions(), "Terminal");
    check_icons(markamp::ui::panel_actions::build_actions(), "Build");
    check_icons(markamp::ui::panel_actions::debug_console_actions(), "Debug Console");
}
