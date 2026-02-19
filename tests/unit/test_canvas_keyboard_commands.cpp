// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/KeyboardCommandModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Binding registry and lookup", "[keyboard][bindings]")
{
    KeyboardCommandModel model;
    model.set_bindings({
        {"select-all", "Select All", "Ctrl+A", "edit", true},
        {"tool-pen", "Pen Tool", "P", "tool", true},
        {"nudge-up", "Nudge Up", "Up", "arrange", true},
    });
    REQUIRE(model.bindings().size() == 3);
    REQUIRE(model.find_binding("tool-pen").shortcut == "P");
    REQUIRE_FALSE(model.find_binding("nonexistent").active);
}

TEST_CASE("Category filtering", "[keyboard][category]")
{
    KeyboardCommandModel model;
    model.set_bindings({
        {"select-all", "Select All", "Ctrl+A", "edit", true},
        {"tool-pen", "Pen", "P", "tool", true},
        {"tool-rect", "Rectangle", "R", "tool", true},
    });
    REQUIRE(model.bindings_in_category("tool").size() == 2);
    REQUIRE(model.bindings_in_category("edit").size() == 1);
}

TEST_CASE("Conflict detection", "[keyboard][conflicts]")
{
    KeyboardCommandModel model;
    model.set_bindings({
        {"cmd-a", "A", "Ctrl+D", "edit", true},
        {"cmd-b", "B", "Ctrl+D", "edit", true}, // conflict!
        {"cmd-c", "C", "Ctrl+Z", "edit", true},
    });
    REQUIRE(model.has_conflicts());
    REQUIRE(model.conflicts().size() == 1);
    REQUIRE(model.conflicts()[0] == "Ctrl+D");
}

TEST_CASE("No conflicts when unique", "[keyboard][conflicts]")
{
    KeyboardCommandModel model;
    model.set_bindings({
        {"cmd-a", "A", "Ctrl+A", "edit", true},
        {"cmd-b", "B", "Ctrl+B", "edit", true},
    });
    REQUIRE_FALSE(model.has_conflicts());
}

TEST_CASE("Nudge step and grid mode", "[keyboard][nudge]")
{
    KeyboardCommandModel model;
    model.set_nudge_step(10.0);
    REQUIRE(model.nudge_step() == 10.0);
    model.set_grid_nudge(true);
    REQUIRE(model.grid_nudge());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
