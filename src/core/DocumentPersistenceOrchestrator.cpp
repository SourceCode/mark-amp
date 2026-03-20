/// @file DocumentPersistenceOrchestrator.cpp
/// @brief P04-T01: Single source of truth for document save lifecycle.

#include "DocumentPersistenceOrchestrator.h"

#include "Config.h"
#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

DocumentPersistenceOrchestrator::DocumentPersistenceOrchestrator(EventBus& bus, Config& cfg)
    : event_bus_(bus)
    , config_(cfg)
{
}

auto DocumentPersistenceOrchestrator::save(const std::string& file_id) -> bool
{
    if (file_id.empty())
    {
        MARKAMP_LOG_WARN("Save called with empty file_id");
        event_bus_.publish(events::NotificationEvent{
            "No file to save.",
            events::NotificationLevel::Warning,
            2000,
        });
        return false;
    }

    // Publish save request — MainFrame/EditorPanel handles the actual I/O
    const events::TabSaveRequestEvent save_evt{file_id};
    event_bus_.publish(save_evt);

    // Mark clean after save
    mark_clean(file_id);

    MARKAMP_LOG_DEBUG("Document saved: {}", file_id);
    return true;
}

auto DocumentPersistenceOrchestrator::save_as(const std::string& file_id,
                                               const std::string& new_path) -> bool
{
    if (file_id.empty())
    {
        MARKAMP_LOG_WARN("Save-as called with empty file_id");
        return false;
    }

    const events::TabSaveAsRequestEvent save_as_evt{new_path};
    event_bus_.publish(save_as_evt);

    mark_clean(file_id);

    MARKAMP_LOG_DEBUG("Document saved as: {} -> {}", file_id, new_path);
    return true;
}

auto DocumentPersistenceOrchestrator::save_all() -> int
{
    int saved_count = 0;
    auto files = dirty_files();
    for (const auto& file_id : files)
    {
        if (save(file_id))
        {
            ++saved_count;
        }
    }

    if (saved_count > 0)
    {
        event_bus_.publish(events::NotificationEvent{
            std::to_string(saved_count) + " file(s) saved.",
            events::NotificationLevel::Success,
            2000,
        });
    }

    MARKAMP_LOG_INFO("Save all: {}/{} files saved", saved_count, files.size());
    return saved_count;
}

auto DocumentPersistenceOrchestrator::revert(const std::string& file_id) -> bool
{
    if (file_id.empty())
    {
        MARKAMP_LOG_WARN("Revert called with empty file_id");
        return false;
    }

    // Publish revert event
    events::DocumentRevertedEvent revert_evt;
    revert_evt.file_id = file_id;
    event_bus_.publish(revert_evt);

    mark_clean(file_id);

    event_bus_.publish(events::NotificationEvent{
        "File reverted.",
        events::NotificationLevel::Info,
        2000,
    });

    MARKAMP_LOG_INFO("Document reverted: {}", file_id);
    return true;
}

void DocumentPersistenceOrchestrator::mark_dirty(const std::string& file_id)
{
    if (file_id.empty())
    {
        return;
    }
    auto [iter, inserted] = dirty_set_.insert(file_id);
    if (inserted)
    {
        events::DocumentDirtyStateChangedEvent evt;
        evt.file_id = file_id;
        evt.is_dirty = true;
        event_bus_.publish(evt);
        MARKAMP_LOG_DEBUG("Document marked dirty: {}", file_id);
    }
}

void DocumentPersistenceOrchestrator::mark_clean(const std::string& file_id)
{
    if (dirty_set_.erase(file_id) > 0)
    {
        events::DocumentDirtyStateChangedEvent evt;
        evt.file_id = file_id;
        evt.is_dirty = false;
        event_bus_.publish(evt);
        MARKAMP_LOG_DEBUG("Document marked clean: {}", file_id);
    }
}

auto DocumentPersistenceOrchestrator::is_dirty(const std::string& file_id) const -> bool
{
    return dirty_set_.contains(file_id);
}

auto DocumentPersistenceOrchestrator::dirty_files() const -> std::vector<std::string>
{
    return {dirty_set_.begin(), dirty_set_.end()};
}

auto DocumentPersistenceOrchestrator::has_dirty_files() const -> bool
{
    return !dirty_set_.empty();
}

auto DocumentPersistenceOrchestrator::dirty_count() const -> int
{
    return static_cast<int>(dirty_set_.size());
}

} // namespace markamp::core
