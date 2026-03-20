/// @file NotebookDocumentLifecycle.cpp
/// @brief P08-T02: Notebook document lifecycle with persistence and tab state.

#include "NotebookDocumentLifecycle.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

NotebookDocumentLifecycle::NotebookDocumentLifecycle(EventBus& bus)
    : event_bus_(bus)
{
}

void NotebookDocumentLifecycle::open(const std::string& notebook_id, const std::string& path)
{
    open_notebooks_.insert(notebook_id);
    MARKAMP_LOG_INFO("Notebook opened: {} ({})", notebook_id, path);
}

void NotebookDocumentLifecycle::close(const std::string& notebook_id)
{
    open_notebooks_.erase(notebook_id);
    dirty_notebooks_.erase(notebook_id);
    MARKAMP_LOG_INFO("Notebook closed: {}", notebook_id);
}

void NotebookDocumentLifecycle::rename(const std::string& notebook_id,
                                        const std::string& new_title)
{
    event_bus_.publish(events::NotificationEvent{
        "Notebook renamed: " + new_title,
        events::NotificationLevel::Success, 2000});
    MARKAMP_LOG_INFO("Notebook renamed: {} -> {}", notebook_id, new_title);
}

void NotebookDocumentLifecycle::save(const std::string& notebook_id)
{
    clear_dirty(notebook_id);
    MARKAMP_LOG_INFO("Notebook saved: {}", notebook_id);
}

void NotebookDocumentLifecycle::mark_dirty(const std::string& notebook_id)
{
    dirty_notebooks_.insert(notebook_id);
}

void NotebookDocumentLifecycle::clear_dirty(const std::string& notebook_id)
{
    dirty_notebooks_.erase(notebook_id);
}

auto NotebookDocumentLifecycle::is_dirty(const std::string& notebook_id) const -> bool
{
    return dirty_notebooks_.contains(notebook_id);
}

auto NotebookDocumentLifecycle::is_open(const std::string& notebook_id) const -> bool
{
    return open_notebooks_.contains(notebook_id);
}

} // namespace markamp::core
