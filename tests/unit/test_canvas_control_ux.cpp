// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/CanvasControlModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_canvas() -> CanvasControlModel
{
    CanvasControlModel model;
    model.set_tools({
        {CanvasTool::kSelect, "Select", "V", "pointer"},
        {CanvasTool::kPan, "Pan", "H", "pointer"},
        {CanvasTool::kText, "Text", "T", "create"},
        {CanvasTool::kShape, "Shape", "S", "create"},
        {CanvasTool::kConnector, "Connector", "C", "create"},
        {CanvasTool::kPen, "Pen", "P", "draw"},
        {CanvasTool::kEraser, "Eraser", "E", "draw"},
    });
    return model;
}

TEST_CASE("Tools by group", "[canvas][tools]")
{
    auto model = make_test_canvas();
    const auto pointer_tools = model.tools_by_group("pointer");
    REQUIRE(pointer_tools.size() == 2);
    const auto create_tools = model.tools_by_group("create");
    REQUIRE(create_tools.size() == 3);
}

TEST_CASE("Active tool default is select", "[canvas][tools]")
{
    auto model = make_test_canvas();
    REQUIRE(model.active_tool() == CanvasTool::kSelect);
    REQUIRE(model.active_tool_label() == "Select");
}

TEST_CASE("Set active tool", "[canvas][tools]")
{
    auto model = make_test_canvas();
    model.set_active_tool(CanvasTool::kPen);
    REQUIRE(model.active_tool() == CanvasTool::kPen);
    REQUIRE(model.active_tool_label() == "Pen");
}

TEST_CASE("Inspector properties", "[canvas][inspector]")
{
    CanvasControlModel model;
    model.set_properties({
        {"fill", "Fill Color", "#FF0000", "color", true},
        {"stroke", "Stroke Width", "2", "number", true},
    });
    REQUIRE(model.properties().size() == 2);
    REQUIRE(model.properties()[0].label == "Fill Color");
}

TEST_CASE("Single selection actions", "[canvas][selection]")
{
    CanvasControlModel model;
    model.set_selection_count(1);
    REQUIRE_FALSE(model.is_multi_select());
    const auto single_actions = model.selection_actions();
    REQUIRE(single_actions.size() == 5);
    REQUIRE(single_actions[0] == "Delete");
}

TEST_CASE("Multi-select actions", "[canvas][selection]")
{
    CanvasControlModel model;
    model.set_selection_count(3);
    REQUIRE(model.is_multi_select());
    const auto multi_actions = model.selection_actions();
    REQUIRE(multi_actions.size() == 6);
    REQUIRE(multi_actions[0] == "Group");
}

TEST_CASE("No selection returns empty actions", "[canvas][selection]")
{
    CanvasControlModel model;
    model.set_selection_count(0);
    REQUIRE(model.selection_actions().empty());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
