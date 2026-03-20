/// @file FeedbackTaxonomy.cpp
/// @brief P09-T01: Standardized notification, progress, and status feedback.

#include "FeedbackTaxonomy.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

FeedbackTaxonomy::FeedbackTaxonomy(EventBus& bus)
    : event_bus_(bus)
{
    // Default task-to-channel mappings
    map_task_channel("export", FeedbackChannel::kProgress);
    map_task_channel("save", FeedbackChannel::kStatusBar);
    map_task_channel("search", FeedbackChannel::kInlinePanel);
    map_task_channel("command", FeedbackChannel::kToast);
    map_task_channel("error", FeedbackChannel::kToast);
}

void FeedbackTaxonomy::emit(const FeedbackRequest& request)
{
    if (should_throttle(request.source))
    {
        MARKAMP_LOG_DEBUG("Feedback throttled: {} ({})", request.message, request.source);
        return;
    }

    // Map severity to notification level
    events::NotificationLevel level{};
    switch (request.severity)
    {
    case FeedbackSeverity::kInfo:
        level = events::NotificationLevel::Info;
        break;
    case FeedbackSeverity::kSuccess:
        level = events::NotificationLevel::Success;
        break;
    case FeedbackSeverity::kWarning:
        level = events::NotificationLevel::Warning;
        break;
    case FeedbackSeverity::kError:
        level = events::NotificationLevel::Error;
        break;
    }

    if (request.channel != FeedbackChannel::kSilent)
    {
        event_bus_.publish(events::NotificationEvent{
            request.message, level, request.duration_ms});
    }

    last_emit_[request.source] = std::chrono::steady_clock::now();
    ++total_emitted_;

    MARKAMP_LOG_DEBUG("Feedback: [{}] {} (ch: {})", request.source,
                      request.message, static_cast<int>(request.channel));
}

void FeedbackTaxonomy::map_task_channel(const std::string& task_type,
                                         FeedbackChannel channel)
{
    task_channel_map_[task_type] = channel;
}

auto FeedbackTaxonomy::channel_for(const std::string& task_type) const -> FeedbackChannel
{
    auto iter = task_channel_map_.find(task_type);
    if (iter != task_channel_map_.end())
    {
        return iter->second;
    }
    return FeedbackChannel::kToast; // default
}

auto FeedbackTaxonomy::should_throttle(const std::string& source) const -> bool
{
    auto iter = last_emit_.find(source);
    if (iter == last_emit_.end())
    {
        return false;
    }
    const auto elapsed = std::chrono::steady_clock::now() - iter->second;
    return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
           < kThrottleIntervalMs;
}

} // namespace markamp::core
