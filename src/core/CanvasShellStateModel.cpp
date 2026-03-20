/// @file CanvasShellStateModel.cpp
/// @brief P07-T02: Shared canvas shell state for tool rail, inspector, minimap.

#include "CanvasShellStateModel.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

#include <algorithm>

namespace markamp::core
{

CanvasShellStateModel::CanvasShellStateModel(EventBus& bus)
    : event_bus_(bus)
{
}

void CanvasShellStateModel::set_tool(CanvasTool tool)
{
    if (tool == active_tool_)
    {
        return;
    }
    active_tool_ = tool;
    event_bus_.publish(events::NotificationEvent{
        "Tool: " + std::to_string(static_cast<int>(tool)),
        events::NotificationLevel::Info, 1000});
    MARKAMP_LOG_DEBUG("Canvas tool: {}", static_cast<int>(tool));
}

void CanvasShellStateModel::set_zoom(double zoom_percent)
{
    zoom_percent_ = std::clamp(zoom_percent, kMinZoom, kMaxZoom);
    MARKAMP_LOG_DEBUG("Canvas zoom: {}%", zoom_percent_);
}

void CanvasShellStateModel::zoom_in()
{
    set_zoom(zoom_percent_ + kZoomStep);
}

void CanvasShellStateModel::zoom_out()
{
    set_zoom(zoom_percent_ - kZoomStep);
}

void CanvasShellStateModel::zoom_to_fit()
{
    set_zoom(100.0);
    MARKAMP_LOG_DEBUG("Canvas zoom to fit");
}

void CanvasShellStateModel::zoom_reset()
{
    set_zoom(100.0);
}

void CanvasShellStateModel::toggle_inspector()
{
    set_inspector_visible(!inspector_visible_);
}

void CanvasShellStateModel::set_inspector_visible(bool visible)
{
    if (visible == inspector_visible_)
    {
        return;
    }
    inspector_visible_ = visible;
    MARKAMP_LOG_DEBUG("Canvas inspector: {}", visible ? "visible" : "hidden");
}

void CanvasShellStateModel::toggle_minimap()
{
    set_minimap_visible(!minimap_visible_);
}

void CanvasShellStateModel::set_minimap_visible(bool visible)
{
    if (visible == minimap_visible_)
    {
        return;
    }
    minimap_visible_ = visible;
    MARKAMP_LOG_DEBUG("Canvas minimap: {}", visible ? "visible" : "hidden");
}

void CanvasShellStateModel::set_board_dirty(bool dirty)
{
    board_dirty_ = dirty;
}

} // namespace markamp::core
