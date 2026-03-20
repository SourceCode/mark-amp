/// @file FeedbackTaxonomy.h
/// @brief P09-T01: Standardized notification, progress, and status feedback.
///
/// Defines one feedback language for toasts, status items, progress, and
/// inline state. Provides throttling helpers and channel mapping.
#pragma once

#include "EventBus.h"

#include <chrono>
#include <string>
#include <unordered_map>

namespace markamp::core
{

/// Feedback severity level.
enum class FeedbackSeverity
{
    kInfo,
    kSuccess,
    kWarning,
    kError,
};

/// Feedback channel (where the message appears).
enum class FeedbackChannel
{
    kToast,       ///< Transient notification toast
    kStatusBar,   ///< Status bar item
    kProgress,    ///< Progress indicator
    kInlinePanel, ///< Inline panel message
    kSilent,      ///< Log only
};

/// A feedback request.
struct FeedbackRequest
{
    std::string message;
    FeedbackSeverity severity{FeedbackSeverity::kInfo};
    FeedbackChannel channel{FeedbackChannel::kToast};
    int duration_ms{3000};
    std::string source; ///< Originating subsystem
};

/// Manages feedback taxonomy and throttling.
class FeedbackTaxonomy
{
public:
    explicit FeedbackTaxonomy(EventBus& bus);

    /// Emit feedback through the appropriate channel.
    void emit(const FeedbackRequest& request);

    /// Map a task type to a default channel.
    void map_task_channel(const std::string& task_type, FeedbackChannel channel);

    /// Get channel for a task type.
    [[nodiscard]] auto channel_for(const std::string& task_type) const -> FeedbackChannel;

    /// Check if a message should be throttled.
    [[nodiscard]] auto should_throttle(const std::string& source) const -> bool;

    /// Get total feedback count.
    [[nodiscard]] auto total_count() const -> int { return total_emitted_; }

private:
    EventBus& event_bus_;
    std::unordered_map<std::string, FeedbackChannel> task_channel_map_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> last_emit_;
    int total_emitted_{0};
    static constexpr int kThrottleIntervalMs = 500;
};

} // namespace markamp::core
