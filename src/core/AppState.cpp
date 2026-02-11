#include "AppState.h"

#include <filesystem>

namespace markamp::core
{

auto AppState::active_file_name() const -> std::string
{
    if (active_file_id.empty())
    {
        return "Untitled";
    }
    return std::filesystem::path(active_file_id).filename().string();
}

auto AppState::is_modified() const -> bool
{
    // TODO: Track dirty state when editor integration is added
    return false;
}

AppStateManager::AppStateManager(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

auto AppStateManager::state() const -> const AppState&
{
    return state_;
}

void AppStateManager::set_active_file(const std::string& file_id)
{
    state_.active_file_id = file_id;
    events::ActiveFileChangedEvent evt;
    evt.file_id = file_id;
    event_bus_.publish(evt);
}

void AppStateManager::set_view_mode(events::ViewMode mode)
{
    state_.view_mode = mode;
    events::ViewModeChangedEvent evt;
    evt.mode = mode;
    event_bus_.publish(evt);
}

void AppStateManager::set_sidebar_visible(bool visible)
{
    state_.sidebar_visible = visible;
    events::SidebarToggleEvent evt;
    evt.visible = visible;
    event_bus_.publish(evt);
}

void AppStateManager::set_cursor_position(int line, int column)
{
    state_.cursor_line = line;
    state_.cursor_column = column;
    events::CursorPositionChangedEvent evt;
    evt.line = line;
    evt.column = column;
    event_bus_.publish(evt);
}

void AppStateManager::update_content(const std::string& content)
{
    state_.active_file_content = content;
    events::EditorContentChangedEvent evt;
    evt.content = content;
    event_bus_.publish(evt);
}

} // namespace markamp::core
