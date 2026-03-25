/// @file CrossSurfaceNavigator.cpp
/// @brief P07-T05: Cross-surface navigation orchestrator.

#include "CrossSurfaceNavigator.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

CrossSurfaceNavigator::CrossSurfaceNavigator(EventBus& bus)
    : event_bus_(bus)
{
}

void CrossSurfaceNavigator::navigate_to(const NavigationTarget& target)
{
    switch (target.surface)
    {
    case NavigationSurface::kEditor:
        navigate_to_file(target.resource_id, target.line);
        break;
    case NavigationSurface::kGraph:
        navigate_to_graph_node(target.resource_id);
        break;
    }
}

void CrossSurfaceNavigator::navigate_to_file(const std::string& path, int line)
{
    current_ = NavigationSurface::kEditor;

    events::ActiveFileChangedEvent open_evt;
    open_evt.file_id = path;
    event_bus_.publish(open_evt);

    if (line > 0)
    {
        events::CursorPositionChangedEvent cursor_evt;
        cursor_evt.line = line;
        cursor_evt.column = 1;
        event_bus_.publish(cursor_evt);
    }

    MARKAMP_LOG_INFO("Cross-surface: editor -> {}{}", path,
                     line > 0 ? ":" + std::to_string(line) : "");
}

void CrossSurfaceNavigator::navigate_to_graph_node(const std::string& node_id)
{
    current_ = NavigationSurface::kGraph;

    MARKAMP_LOG_INFO("Cross-surface: graph -> {}", node_id);
}

} // namespace markamp::core
