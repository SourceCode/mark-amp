// ============================================================================
// File: src/canvas/WidgetMessageBus.cpp
// Phase 14: Canvas Extensibility — inter-widget messaging
// ============================================================================
#include "canvas/WidgetMessageBus.h"

#include "core/Events.h"

#include <algorithm>

namespace markamp::canvas
{

WidgetMessageBus::WidgetMessageBus(core::EventBus& event_bus, size_t max_queue_size)
    : event_bus_(event_bus)
    , max_queue_size_(max_queue_size)
{
}

// ── Send ──────────────────────────────────────────────────────────

auto WidgetMessageBus::send_message(const WidgetMessage& message) -> bool
{
    if (messages_delivered_ >= max_queue_size_)
    {
        ++messages_dropped_;
        return false;
    }

    bool delivered = false;

    for (const auto& sub : subscriptions_)
    {
        // Point-to-point: only deliver to the target widget
        if (!message.target_widget_id.empty() &&
            sub.info.subscriber_widget_id != message.target_widget_id)
        {
            continue;
        }

        // Check message type pattern match
        if (matches_pattern(message.message_type, sub.info.message_type_pattern))
        {
            if (sub.callback)
            {
                sub.callback(message);
                delivered = true;
            }
        }
    }

    if (delivered)
    {
        ++messages_delivered_;
    }

    return delivered;
}

auto WidgetMessageBus::broadcast(const WidgetMessage& message) -> size_t
{
    if (messages_delivered_ >= max_queue_size_)
    {
        ++messages_dropped_;
        return 0;
    }

    size_t delivery_count = 0;

    for (const auto& sub : subscriptions_)
    {
        // Skip sender to avoid self-delivery
        if (sub.info.subscriber_widget_id == message.source_widget_id)
        {
            continue;
        }

        if (matches_pattern(message.message_type, sub.info.message_type_pattern))
        {
            if (sub.callback)
            {
                sub.callback(message);
                ++delivery_count;
            }
        }
    }

    if (delivery_count > 0)
    {
        ++messages_delivered_;
    }

    return delivery_count;
}

// ── Subscribe ─────────────────────────────────────────────────────

auto WidgetMessageBus::subscribe(const std::string& subscriber_widget_id,
                                 const std::string& message_type_pattern,
                                 WidgetMessageCallback callback) -> WidgetMessageSubscription
{
    WidgetMessageSubscription info;
    info.subscription_id = next_sub_id_++;
    info.subscriber_widget_id = subscriber_widget_id;
    info.message_type_pattern = message_type_pattern;

    InternalSubscription sub;
    sub.info = info;
    sub.callback = std::move(callback);

    subscriptions_.push_back(std::move(sub));

    return info;
}

auto WidgetMessageBus::unsubscribe(size_t subscription_id) -> bool
{
    auto iter = std::ranges::find_if(subscriptions_,
                                     [subscription_id](const InternalSubscription& sub)
                                     { return sub.info.subscription_id == subscription_id; });
    if (iter != subscriptions_.end())
    {
        subscriptions_.erase(iter);
        return true;
    }
    return false;
}

auto WidgetMessageBus::unsubscribe_widget(const std::string& widget_id) -> size_t
{
    size_t removed = 0;
    auto iter = subscriptions_.begin();
    while (iter != subscriptions_.end())
    {
        if (iter->info.subscriber_widget_id == widget_id)
        {
            iter = subscriptions_.erase(iter);
            ++removed;
        }
        else
        {
            ++iter;
        }
    }
    return removed;
}

// ── Query ─────────────────────────────────────────────────────────

auto WidgetMessageBus::subscription_count() const -> size_t
{
    return subscriptions_.size();
}

auto WidgetMessageBus::subscriptions_for_widget(const std::string& widget_id) const
    -> std::vector<WidgetMessageSubscription>
{
    std::vector<WidgetMessageSubscription> result;
    for (const auto& sub : subscriptions_)
    {
        if (sub.info.subscriber_widget_id == widget_id)
        {
            result.push_back(sub.info);
        }
    }
    return result;
}

auto WidgetMessageBus::messages_delivered() const -> size_t
{
    return messages_delivered_;
}

auto WidgetMessageBus::messages_dropped() const -> size_t
{
    return messages_dropped_;
}

auto WidgetMessageBus::max_queue_size() const -> size_t
{
    return max_queue_size_;
}

// ── Cleanup ───────────────────────────────────────────────────────

auto WidgetMessageBus::clear() -> void
{
    subscriptions_.clear();
    messages_delivered_ = 0;
    messages_dropped_ = 0;
}

// ── Private ───────────────────────────────────────────────────────

auto WidgetMessageBus::matches_pattern(const std::string& message_type, const std::string& pattern)
    -> bool
{
    // Wildcard matches everything
    if (pattern == "*")
    {
        return true;
    }

    // Exact match
    if (pattern == message_type)
    {
        return true;
    }

    // Prefix match with trailing wildcard (e.g. "data.*" matches "data.update")
    if (pattern.size() > 1 && pattern.back() == '*')
    {
        const auto prefix = pattern.substr(0, pattern.size() - 1);
        return message_type.starts_with(prefix);
    }

    return false;
}

} // namespace markamp::canvas
