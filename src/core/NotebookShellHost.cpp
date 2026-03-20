/// @file NotebookShellHost.cpp
/// @brief P08-T01: Typed notebook workbench host.

#include "NotebookShellHost.h"

#include "Events.h"
#include "Logger.h"

#include <algorithm>

namespace markamp::core
{

NotebookShellHost::NotebookShellHost(EventBus& bus)
    : event_bus_(bus)
{
}

void NotebookShellHost::set_state(NotebookHostState state)
{
    state_ = state;
    MARKAMP_LOG_DEBUG("Notebook host state: {}", static_cast<int>(state));
}

void NotebookShellHost::add_entry(const NotebookEntry& entry)
{
    entries_.push_back(entry);
    if (state_ == NotebookHostState::kEmpty)
    {
        set_state(NotebookHostState::kReady);
    }
}

void NotebookShellHost::remove_entry(const std::string& notebook_id)
{
    auto iter = std::remove_if(entries_.begin(), entries_.end(),
                               [&notebook_id](const NotebookEntry& entry)
                               { return entry.notebook_id == notebook_id; });
    if (iter != entries_.end())
    {
        entries_.erase(iter, entries_.end());
        MARKAMP_LOG_DEBUG("Notebook removed: {}", notebook_id);
    }

    if (entries_.empty())
    {
        set_state(NotebookHostState::kEmpty);
    }

    if (active_id_ == notebook_id)
    {
        active_id_.clear();
    }
}

void NotebookShellHost::open_notebook(const std::string& notebook_id)
{
    active_id_ = notebook_id;
    event_bus_.publish(events::NotificationEvent{
        "Notebook opened",
        events::NotificationLevel::Info, 1500});
    MARKAMP_LOG_INFO("Notebook opened: {}", notebook_id);
}

void NotebookShellHost::create_notebook(const std::string& title)
{
    event_bus_.publish(events::NotificationEvent{
        "Notebook created: " + title,
        events::NotificationLevel::Success, 2000});
    MARKAMP_LOG_INFO("Notebook created: {}", title);
}

} // namespace markamp::core
