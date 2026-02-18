#pragma once

/// @file WidgetMessageBus.h
/// @brief Phase 14: Inter-widget communication bus.
///
/// Provides point-to-point and broadcast messaging between widgets
/// on the canvas. Widgets subscribe by message type pattern and
/// receive messages through registered callbacks.

#include "core/EventBus.h"

#include <cstddef>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// A message sent between widgets.
struct WidgetMessage
{
    std::string source_widget_id; ///< Sender widget
    std::string target_widget_id; ///< Target widget (empty = broadcast)
    std::string message_type;     ///< Type discriminator (e.g. "data.update")
    std::string payload_json;     ///< JSON payload
};

/// Callback for received messages: (message) -> void.
using WidgetMessageCallback = std::function<void(const WidgetMessage&)>;

/// Subscription handle for message unsubscription.
struct WidgetMessageSubscription
{
    size_t subscription_id{0};
    std::string subscriber_widget_id;
    std::string message_type_pattern; ///< Pattern to match (exact or "*" for all)
};

/// Inter-widget messaging bus for canvas app widgets.
///
/// Supports point-to-point messages (target_widget_id set) and
/// broadcasts (target_widget_id empty). Subscribers filter by
/// message type pattern.
class WidgetMessageBus
{
public:
    explicit WidgetMessageBus(core::EventBus& event_bus, size_t max_queue_size = 1000);

    // ── Send ──────────────────────────────────────────────────────

    /// Send a message to a specific widget.
    auto send_message(const WidgetMessage& message) -> bool;

    /// Broadcast a message to all subscribers.
    auto broadcast(const WidgetMessage& message) -> size_t;

    // ── Subscribe ─────────────────────────────────────────────────

    /// Subscribe a widget to messages matching a type pattern.
    /// Pattern "*" matches all message types.
    auto subscribe(const std::string& subscriber_widget_id,
                   const std::string& message_type_pattern,
                   WidgetMessageCallback callback) -> WidgetMessageSubscription;

    /// Unsubscribe by subscription ID.
    auto unsubscribe(size_t subscription_id) -> bool;

    /// Unsubscribe all subscriptions for a widget.
    auto unsubscribe_widget(const std::string& widget_id) -> size_t;

    // ── Query ─────────────────────────────────────────────────────

    /// Number of active subscriptions.
    [[nodiscard]] auto subscription_count() const -> size_t;

    /// Subscriptions for a specific widget.
    [[nodiscard]] auto subscriptions_for_widget(const std::string& widget_id) const
        -> std::vector<WidgetMessageSubscription>;

    /// Total messages delivered.
    [[nodiscard]] auto messages_delivered() const -> size_t;

    /// Messages dropped due to queue overflow.
    [[nodiscard]] auto messages_dropped() const -> size_t;

    /// Maximum queue size.
    [[nodiscard]] auto max_queue_size() const -> size_t;

    // ── Cleanup ───────────────────────────────────────────────────

    auto clear() -> void;

private:
    /// Internal subscription entry with callback.
    struct InternalSubscription
    {
        WidgetMessageSubscription info;
        WidgetMessageCallback callback;
    };

    /// Check if a message type matches a pattern.
    [[nodiscard]] static auto matches_pattern(const std::string& message_type,
                                              const std::string& pattern) -> bool;

    [[maybe_unused]] core::EventBus& event_bus_;
    std::vector<InternalSubscription> subscriptions_;
    size_t next_sub_id_{1};
    size_t max_queue_size_;
    size_t messages_delivered_{0};
    size_t messages_dropped_{0};
};

} // namespace markamp::canvas
