/// @file CanvasEventBridge.cpp
/// @brief V20 P04-T02: Canvas event bridge implementation.

#include "CanvasEventBridge.h"

#include "Logger.h"

namespace markamp::core
{

CanvasEventBridge::CanvasEventBridge(EventBus& shared_bus)
    : shared_bus_(shared_bus)
{
}

void CanvasEventBridge::set_context(const CanvasContext& context)
{
    ++update_count_;
    context_ = context;

    MARKAMP_LOG_DEBUG("Canvas context set: board={}, tool={}, focused={}, selected={}",
                      context_.active_board_id.value, context_.active_tool,
                      context_.is_canvas_focused, context_.selected_object_count);
}

void CanvasEventBridge::clear_context()
{
    ++update_count_;
    context_ = {};

    MARKAMP_LOG_DEBUG("Canvas context cleared");
}

} // namespace markamp::core
