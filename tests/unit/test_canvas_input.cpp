#include "canvas/CanvasTool.h"
#include "canvas/PanTool.h"
#include "canvas/SelectTool.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ============================================================================
// ToolMode and Enums
// ============================================================================

TEST_CASE("ToolMode enum values", "[canvas][input]")
{
    REQUIRE(static_cast<uint8_t>(ToolMode::Select) == 0);
    REQUIRE(static_cast<uint8_t>(ToolMode::Pan) == 1);
    REQUIRE(static_cast<uint8_t>(ToolMode::Draw) == 2);
    REQUIRE(static_cast<uint8_t>(ToolMode::Shape) == 3);
    REQUIRE(static_cast<uint8_t>(ToolMode::Text) == 4);
    REQUIRE(static_cast<uint8_t>(ToolMode::Connector) == 5);
    REQUIRE(static_cast<uint8_t>(ToolMode::StickyNote) == 6);
    REQUIRE(static_cast<uint8_t>(ToolMode::Eraser) == 7);
    REQUIRE(static_cast<uint8_t>(ToolMode::Comment) == 8);
}

TEST_CASE("MouseButton enum values", "[canvas][input]")
{
    REQUIRE(static_cast<uint8_t>(MouseButton::None) == 0);
    REQUIRE(static_cast<uint8_t>(MouseButton::Left) == 1);
    REQUIRE(static_cast<uint8_t>(MouseButton::Middle) == 2);
    REQUIRE(static_cast<uint8_t>(MouseButton::Right) == 3);
}

// ============================================================================
// ModifierKeys
// ============================================================================

TEST_CASE("ModifierKeys bitwise operations", "[canvas][input]")
{
    SECTION("single modifiers")
    {
        REQUIRE(has_modifier(ModifierKeys::Shift, ModifierKeys::Shift));
        REQUIRE_FALSE(has_modifier(ModifierKeys::Shift, ModifierKeys::Ctrl));
        REQUIRE_FALSE(has_modifier(ModifierKeys::None, ModifierKeys::Shift));
    }

    SECTION("combined modifiers")
    {
        const auto combined = ModifierKeys::Shift | ModifierKeys::Ctrl;
        REQUIRE(has_modifier(combined, ModifierKeys::Shift));
        REQUIRE(has_modifier(combined, ModifierKeys::Ctrl));
        REQUIRE_FALSE(has_modifier(combined, ModifierKeys::Alt));
    }

    SECTION("all modifiers")
    {
        const auto all =
            ModifierKeys::Shift | ModifierKeys::Ctrl | ModifierKeys::Alt | ModifierKeys::Meta;
        REQUIRE(has_modifier(all, ModifierKeys::Shift));
        REQUIRE(has_modifier(all, ModifierKeys::Ctrl));
        REQUIRE(has_modifier(all, ModifierKeys::Alt));
        REQUIRE(has_modifier(all, ModifierKeys::Meta));
    }
}

// ============================================================================
// CanvasInputEvent
// ============================================================================

TEST_CASE("CanvasInputEvent default values", "[canvas][input]")
{
    const CanvasInputEvent evt;
    REQUIRE(evt.screen_pos.x == 0.0);
    REQUIRE(evt.screen_pos.y == 0.0);
    REQUIRE(evt.world_pos.x == 0.0);
    REQUIRE(evt.world_pos.y == 0.0);
    REQUIRE(evt.button == MouseButton::None);
    REQUIRE(evt.modifiers == ModifierKeys::None);
    REQUIRE(evt.scroll_delta_x == 0.0);
    REQUIRE(evt.scroll_delta_y == 0.0);
    REQUIRE(evt.click_count == 0);
    REQUIRE_FALSE(evt.is_drag);
}

// ============================================================================
// SelectTool
// ============================================================================

TEST_CASE("SelectTool properties", "[canvas][input]")
{
    SelectTool tool;
    REQUIRE(tool.tool_mode() == ToolMode::Select);
    REQUIRE(tool.cursor_name() == "arrow");
    REQUIRE_FALSE(tool.is_marquee_active());
    REQUIRE_FALSE(tool.marquee_bounds().has_value());
}

// ============================================================================
// PanTool
// ============================================================================

TEST_CASE("PanTool properties", "[canvas][input]")
{
    PanTool tool;
    REQUIRE(tool.tool_mode() == ToolMode::Pan);
    REQUIRE(tool.cursor_name() == "hand");
    REQUIRE_FALSE(tool.is_panning());
}

// ============================================================================
// CanvasInputEvent construction
// ============================================================================

TEST_CASE("CanvasInputEvent custom construction", "[canvas][input]")
{
    CanvasInputEvent evt;
    evt.screen_pos = Point2D{100.0, 200.0};
    evt.world_pos = Point2D{50.0, 150.0};
    evt.button = MouseButton::Left;
    evt.modifiers = ModifierKeys::Shift | ModifierKeys::Ctrl;
    evt.scroll_delta_y = 1.5;
    evt.click_count = 2;
    evt.is_drag = true;

    REQUIRE(evt.screen_pos.x == 100.0);
    REQUIRE(evt.world_pos.y == 150.0);
    REQUIRE(evt.button == MouseButton::Left);
    REQUIRE(has_modifier(evt.modifiers, ModifierKeys::Shift));
    REQUIRE(has_modifier(evt.modifiers, ModifierKeys::Ctrl));
    REQUIRE(evt.scroll_delta_y == 1.5);
    REQUIRE(evt.click_count == 2);
    REQUIRE(evt.is_drag);
}

// ============================================================================
// CanvasInputManager (basic, no wx dependency needed)
// ============================================================================

TEST_CASE("CanvasInputManager ToolMode casting", "[canvas][input]")
{
    // Verify round-trip casting for tool registration keys.
    for (uint8_t mode_idx = 0; mode_idx <= 8; ++mode_idx)
    {
        const auto mode = static_cast<ToolMode>(mode_idx);
        REQUIRE(static_cast<uint8_t>(mode) == mode_idx);
    }
}
