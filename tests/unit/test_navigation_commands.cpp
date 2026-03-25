/**
 * @file test_navigation_commands.cpp
 * @brief Phase 30: Unit tests for NavigationCommandSet and JumpToController.
 */

#include "ui/JumpToController.h"
#include "ui/NavigationCommandSet.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <set>
#include <string>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// NavigationCommandSet — command registry
// ═══════════════════════════════════════════════════════

TEST_CASE("NavigationCommandSet - standard commands", "[navigation][commands]")
{
    auto commands = NavigationCommandSet::standard_commands();
    CHECK(commands.size() == static_cast<size_t>(NavigationCommandSet::command_count()));
    CHECK(commands.size() >= 10);
}

TEST_CASE("NavigationCommandSet - all commands have labels", "[navigation][commands]")
{
    auto commands = NavigationCommandSet::standard_commands();
    for (const auto& cmd : commands)
    {
        INFO("Command: " << cmd.command_id);
        CHECK_FALSE(cmd.label.empty());
        CHECK_FALSE(cmd.shortcut.empty());
        CHECK_FALSE(cmd.icon_name.empty());
    }
}

TEST_CASE("NavigationCommandSet - unique command IDs", "[navigation][commands]")
{
    auto commands = NavigationCommandSet::standard_commands();
    std::set<std::string> seen;
    for (const auto& cmd : commands)
    {
        CHECK(seen.find(cmd.command_id) == seen.end());
        seen.insert(cmd.command_id);
    }
}

TEST_CASE("NavigationCommandSet - scope filtering", "[navigation][commands]")
{
    auto global_cmds = NavigationCommandSet::commands_for_scope(NavigationScope::kGlobal);
    auto editor_cmds = NavigationCommandSet::commands_for_scope(NavigationScope::kEditor);
    auto canvas_cmds = NavigationCommandSet::commands_for_scope(NavigationScope::kEditor);
    auto pane_cmds = NavigationCommandSet::commands_for_scope(NavigationScope::kPane);

    CHECK_FALSE(global_cmds.empty());
    CHECK_FALSE(editor_cmds.empty());
    CHECK_FALSE(canvas_cmds.empty());
    CHECK_FALSE(pane_cmds.empty());

    // All global commands should have kGlobal scope
    for (const auto& cmd : global_cmds)
    {
        CHECK(cmd.scope == NavigationScope::kGlobal);
    }
}

TEST_CASE("NavigationCommandSet - find command", "[navigation][commands]")
{
    const auto* back_cmd = NavigationCommandSet::find_command("nav.back");
    REQUIRE(back_cmd != nullptr);
    CHECK(back_cmd->label == "Go Back");
    CHECK(back_cmd->scope == NavigationScope::kGlobal);

    const auto* missing = NavigationCommandSet::find_command("nonexistent");
    CHECK(missing == nullptr);
}

TEST_CASE("NavigationCommandSet - scope labels", "[navigation][commands]")
{
    auto commands = NavigationCommandSet::standard_commands();
    for (const auto& cmd : commands)
    {
        auto scope = cmd.scope_label();
        CHECK_FALSE(scope.empty());
    }
}

TEST_CASE("NavigationCommandSet - available scopes", "[navigation][commands]")
{
    auto scopes = NavigationCommandSet::available_scopes();
    CHECK(scopes.size() == 3);
}

// ═══════════════════════════════════════════════════════
// JumpToController — basics
// ═══════════════════════════════════════════════════════

TEST_CASE("JumpToController - empty controller", "[navigation][jump]")
{
    JumpToController controller;
    CHECK(controller.target_count() == 0);
    auto results = controller.search("test");
    CHECK(results.empty());
}

TEST_CASE("JumpToController - add and search", "[navigation][jump]")
{
    JumpToController controller;
    controller.add_target({"main.cpp", "src/main.cpp", "file", JumpTargetKind::kFile, "f1"});
    controller.add_target(
        {"Application.h", "src/Application.h", "file", JumpTargetKind::kFile, "f2"});
    controller.add_target({"README.md", "docs/README.md", "file", JumpTargetKind::kFile, "f3"});

    CHECK(controller.target_count() == 3);

    auto results = controller.search("main");
    CHECK(results.size() == 1);
    CHECK(results[0].label == "main.cpp");
}

TEST_CASE("JumpToController - case insensitive search", "[navigation][jump]")
{
    JumpToController controller;
    controller.add_target(
        {"MainFrame.cpp", "src/MainFrame.cpp", "file", JumpTargetKind::kFile, "f1"});

    auto results = controller.search("mainframe");
    CHECK(results.size() == 1);
    CHECK(results[0].label == "MainFrame.cpp");
}

TEST_CASE("JumpToController - prefix match scores higher", "[navigation][jump]")
{
    JumpToController controller;
    controller.add_target({"utils.cpp", "src/utils.cpp", "file", JumpTargetKind::kFile, "f1"});
    controller.add_target(
        {"test_utils.cpp", "tests/test_utils.cpp", "file", JumpTargetKind::kFile, "f2"});

    auto results = controller.search("utils");
    REQUIRE(results.size() == 2);
    // "utils.cpp" should be first (prefix match)
    CHECK(results[0].label == "utils.cpp");
    CHECK(results[1].label == "test_utils.cpp");
}

TEST_CASE("JumpToController - empty query returns all", "[navigation][jump]")
{
    JumpToController controller;
    controller.add_target({"a.cpp", "", "file", JumpTargetKind::kFile, "f1"});
    controller.add_target({"b.cpp", "", "file", JumpTargetKind::kFile, "f2"});

    auto results = controller.search("");
    CHECK(results.size() == 2);
}

TEST_CASE("JumpToController - filter by kind", "[navigation][jump]")
{
    JumpToController controller;
    controller.add_target({"main.cpp", "", "file", JumpTargetKind::kFile, "f1"});
    controller.add_target({"MyClass", "", "symbol", JumpTargetKind::kSymbol, "s1"});
    controller.add_target({"Overview", "", "heading", JumpTargetKind::kHeading, "h1"});

    JumpFilter filter;
    filter.include_symbols = false;
    filter.include_headings = false;

    auto results = controller.search("", filter);
    CHECK(results.size() == 1);
    CHECK(results[0].kind == JumpTargetKind::kFile);
}

TEST_CASE("JumpToController - MRU via record_access", "[navigation][jump]")
{
    JumpToController controller;
    controller.add_target({"a.cpp", "", "file", JumpTargetKind::kFile, "f1"});
    controller.add_target({"b.cpp", "", "file", JumpTargetKind::kFile, "f2"});

    // Boost b.cpp
    controller.record_access("f2");
    controller.record_access("f2");

    auto results = controller.search("");
    REQUIRE(results.size() == 2);
    // b.cpp should be first due to higher MRU score
    CHECK(results[0].label == "b.cpp");
}

TEST_CASE("JumpToController - recent targets", "[navigation][jump]")
{
    JumpToController controller;
    controller.add_target({"a.cpp", "", "file", JumpTargetKind::kFile, "f1"});
    controller.add_target({"b.cpp", "", "file", JumpTargetKind::kFile, "f2"});
    controller.add_target({"c.cpp", "", "file", JumpTargetKind::kFile, "f3"});

    controller.record_access("f1");
    controller.record_access("f3");

    auto recent = controller.recent_targets(10);
    CHECK(recent.size() == 2);
}

TEST_CASE("JumpToController - clear targets", "[navigation][jump]")
{
    JumpToController controller;
    controller.add_target({"a.cpp", "", "file", JumpTargetKind::kFile, "f1"});
    CHECK(controller.target_count() == 1);

    controller.clear_targets();
    CHECK(controller.target_count() == 0);
}

TEST_CASE("JumpToController - available kinds", "[navigation][jump]")
{
    auto kinds = JumpToController::available_kinds();
    CHECK(kinds.size() == 5);
}

TEST_CASE("JumpFilter - active count", "[navigation][jump][filter]")
{
    JumpFilter filter;
    CHECK(filter.active_count() == 4); // All enabled by default

    filter.include_symbols = false;
    CHECK(filter.active_count() == 3);
}

TEST_CASE("JumpTarget kind labels", "[navigation][jump]")
{
    for (const auto& kind : JumpToController::available_kinds())
    {
        JumpTarget target;
        target.kind = kind;
        auto label = target.kind_label();
        CHECK_FALSE(label.empty());
    }
}
