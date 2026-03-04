#include "UxMetricsCollector.h"

#include <algorithm>
#include <limits>

namespace markamp::ui
{

auto MetricEvent::type_name() const -> std::string
{
    switch (type)
    {
        case UxMetricType::kMisclick:
            return "misclick";
        case UxMetricType::kCommandLatency:
            return "command_latency";
        case UxMetricType::kUndoAfterAction:
            return "undo_after_action";
        case UxMetricType::kCommandFrequency:
            return "command_frequency";
        case UxMetricType::kDragCancelRate:
            return "drag_cancel_rate";
        case UxMetricType::kKeyboardNavRate:
            return "keyboard_nav_rate";
    }
    return "unknown";
}

auto MetricSummary::average() const -> double
{
    if (count == 0)
    {
        return 0.0;
    }
    return total / count;
}

void UxMetricsCollector::record(const MetricEvent& event)
{
    if (!enabled_)
    {
        return;
    }

    events_.push_back(event);

    // Enforce capacity by trimming oldest
    if (static_cast<int>(events_.size()) > max_events_)
    {
        events_.erase(events_.begin());
    }
}

void UxMetricsCollector::record(UxMetricType type, double value, const std::string& context)
{
    MetricEvent event;
    event.type = type;
    event.value = value;
    event.context = context;
    record(event);
}

auto UxMetricsCollector::event_count() const -> int
{
    return static_cast<int>(events_.size());
}

auto UxMetricsCollector::events_of_type(UxMetricType type) const -> std::vector<MetricEvent>
{
    std::vector<MetricEvent> result;
    for (const auto& event : events_)
    {
        if (event.type == type)
        {
            result.push_back(event);
        }
    }
    return result;
}

auto UxMetricsCollector::summary(UxMetricType type) const -> MetricSummary
{
    MetricSummary result;
    result.type = type;
    result.min_value = std::numeric_limits<double>::max();
    result.max_value = std::numeric_limits<double>::lowest();

    for (const auto& event : events_)
    {
        if (event.type == type)
        {
            ++result.count;
            result.total += event.value;
            result.min_value = std::min(result.min_value, event.value);
            result.max_value = std::max(result.max_value, event.value);
        }
    }

    if (result.count == 0)
    {
        result.min_value = 0.0;
        result.max_value = 0.0;
    }

    return result;
}

auto UxMetricsCollector::all_summaries() const -> std::vector<MetricSummary>
{
    std::vector<MetricSummary> summaries;
    summaries.push_back(summary(UxMetricType::kMisclick));
    summaries.push_back(summary(UxMetricType::kCommandLatency));
    summaries.push_back(summary(UxMetricType::kUndoAfterAction));
    summaries.push_back(summary(UxMetricType::kCommandFrequency));
    summaries.push_back(summary(UxMetricType::kDragCancelRate));
    summaries.push_back(summary(UxMetricType::kKeyboardNavRate));
    return summaries;
}

void UxMetricsCollector::set_enabled(bool enabled)
{
    enabled_ = enabled;
}

auto UxMetricsCollector::is_enabled() const -> bool
{
    return enabled_;
}

void UxMetricsCollector::set_max_events(int max_events)
{
    max_events_ = max_events;
}

auto UxMetricsCollector::max_events() const -> int
{
    return max_events_;
}

void UxMetricsCollector::clear()
{
    events_.clear();
}

} // namespace markamp::ui
