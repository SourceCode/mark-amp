// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/CanvasInputModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Initial state is idle", "[canvas][input]")
{
    CanvasInputModel model;
    REQUIRE(model.state() == ToolState::kIdle);
    REQUIRE(model.tool() == ToolType::kSelect);
}

TEST_CASE("Full lifecycle: idle → hover → press → drag → commit", "[canvas][input]")
{
    CanvasInputModel model;
    model.hover();
    REQUIRE(model.state() == ToolState::kHover);
    model.press();
    REQUIRE(model.state() == ToolState::kPressed);
    model.drag();
    REQUIRE(model.state() == ToolState::kDragging);
    model.commit();
    REQUIRE(model.state() == ToolState::kCommitted);
}

TEST_CASE("Cancel from drag restores cancelled state", "[canvas][input]")
{
    CanvasInputModel model;
    model.press();
    model.drag();
    model.cancel();
    REQUIRE(model.state() == ToolState::kCancelled);
}

TEST_CASE("Invalid transition ignored", "[canvas][input]")
{
    CanvasInputModel model;
    model.commit(); // invalid from idle
    REQUIRE(model.state() == ToolState::kIdle);
}

TEST_CASE("Transition validation", "[canvas][input]")
{
    CanvasInputModel model;
    REQUIRE(model.can_transition(ToolState::kHover));
    REQUIRE(model.can_transition(ToolState::kPressed));
    REQUIRE_FALSE(model.can_transition(ToolState::kDragging));
    REQUIRE_FALSE(model.can_transition(ToolState::kCommitted));
}

TEST_CASE("Modifier key queries", "[canvas][input]")
{
    CanvasInputModel model;
    model.set_modifiers({true, false, false, false});
    REQUIRE(model.is_constrained());
    REQUIRE_FALSE(model.is_additive());

    model.set_modifiers({false, true, false, false});
    REQUIRE(model.is_additive());
}

TEST_CASE("Safe switch cancels active gesture", "[canvas][input]")
{
    CanvasInputModel model;
    model.press();
    model.drag();
    REQUIRE(model.is_gesture_active());
    model.safe_switch(ToolType::kShape);
    REQUIRE(model.tool() == ToolType::kShape);
    REQUIRE(model.state() == ToolState::kIdle);
}

TEST_CASE("Safe switch from idle just switches", "[canvas][input]")
{
    CanvasInputModel model;
    model.safe_switch(ToolType::kText);
    REQUIRE(model.tool() == ToolType::kText);
    REQUIRE(model.state() == ToolState::kIdle);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
