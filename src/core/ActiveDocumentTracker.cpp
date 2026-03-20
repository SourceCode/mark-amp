/// @file ActiveDocumentTracker.cpp
/// @brief P04-T04: Single active-document source of truth.

#include "ActiveDocumentTracker.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

ActiveDocumentTracker::ActiveDocumentTracker(EventBus& bus)
    : event_bus_(bus)
{
}

void ActiveDocumentTracker::set_active(const std::string& file_id, int group_id)
{
    // Deduplicate: don't re-publish if same file and group
    if (file_id == active_file_id_ && group_id == active_group_id_)
    {
        return;
    }

    // Push previous to history before switching
    if (!active_file_id_.empty())
    {
        push_to_history();
    }

    active_file_id_ = file_id;
    active_group_id_ = group_id;

    // Publish active file changed
    events::ActiveFileChangedEvent evt;
    evt.file_id = file_id;
    evt.group_id = group_id;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Active document: {} (group: {})", file_id, group_id);
}

void ActiveDocumentTracker::clear()
{
    if (active_file_id_.empty())
    {
        return;
    }

    active_file_id_.clear();
    active_group_id_ = -1;

    // Publish empty active file
    events::ActiveFileChangedEvent evt;
    evt.file_id = "";
    evt.group_id = -1;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Active document cleared");
}

void ActiveDocumentTracker::push_to_history()
{
    if (active_file_id_.empty())
    {
        return;
    }

    // Trim forward history if we're not at the end
    if (history_position_ >= 0 &&
        history_position_ < static_cast<int>(history_.size()) - 1)
    {
        history_.erase(history_.begin() + history_position_ + 1, history_.end());
    }

    history_.push_back(active_file_id_);

    // Cap history size
    if (static_cast<int>(history_.size()) > kMaxHistorySize)
    {
        history_.erase(history_.begin());
    }

    history_position_ = static_cast<int>(history_.size()) - 1;
}

void ActiveDocumentTracker::navigate_back()
{
    if (history_position_ <= 0)
    {
        return;
    }

    --history_position_;
    const auto& file_id = history_[static_cast<std::size_t>(history_position_)];

    active_file_id_ = file_id;

    events::ActiveFileChangedEvent evt;
    evt.file_id = file_id;
    evt.group_id = active_group_id_;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Navigate back to: {}", file_id);
}

void ActiveDocumentTracker::navigate_forward()
{
    if (history_position_ >= static_cast<int>(history_.size()) - 1)
    {
        return;
    }

    ++history_position_;
    const auto& file_id = history_[static_cast<std::size_t>(history_position_)];

    active_file_id_ = file_id;

    events::ActiveFileChangedEvent evt;
    evt.file_id = file_id;
    evt.group_id = active_group_id_;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Navigate forward to: {}", file_id);
}

} // namespace markamp::core
