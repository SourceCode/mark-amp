// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/ToolRailModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Tool grouping", "[toolrail][group]")
{
    ToolRailModel model;
    model.set_tools({
        {"select", "Select", ToolGroup::kSelect, true},
        {"pen", "Pen", ToolGroup::kDraw, true},
        {"rect", "Rectangle", ToolGroup::kShape, true},
    });
    REQUIRE(model.tools_in_group(ToolGroup::kDraw).size() == 1);
    REQUIRE(model.tools_in_group(ToolGroup::kMedia).empty());
}

TEST_CASE("Active tool and quick-switch", "[toolrail][switch]")
{
    ToolRailModel model;
    model.set_tools(
        {{"select", "Select", ToolGroup::kSelect, true}, {"pen", "Pen", ToolGroup::kDraw, true}});
    model.select_tool("select");
    model.select_tool("pen");
    REQUIRE(model.active_tool() == "pen");
    REQUIRE(model.recent_stack()[0] == "select");
    model.quick_switch();
    REQUIRE(model.active_tool() == "select");
}

TEST_CASE("Context toolbar actions", "[toolrail][context]")
{
    ToolRailModel model;
    model.set_context_actions({"align-left", "distribute-h"});
    REQUIRE(model.context_actions().size() == 2);
}

TEST_CASE("Tool visibility toggle", "[toolrail][visibility]")
{
    ToolRailModel model;
    model.set_tools(
        {{"select", "Select", ToolGroup::kSelect, true}, {"pen", "Pen", ToolGroup::kDraw, true}});
    model.set_tool_visible("pen", false);
    REQUIRE(model.visible_tools().size() == 1);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
