#include "UxMetricsModel.h"

#include <algorithm>

namespace markamp::ui
{

void UxMetricsModel::record(MetricEvent event)
{
    events_.push_back(std::move(event));
}

auto UxMetricsModel::events() const -> const std::vector<MetricEvent>&
{
    return events_;
}

void UxMetricsModel::clear_events()
{
    events_.clear();
}

auto UxMetricsModel::count_by_type(MetricType type) const -> int
{
    int count = 0;
    for (const auto& evt : events_)
    {
        if (evt.type == type)
        {
            ++count;
        }
    }
    return count;
}

auto UxMetricsModel::average_by_type(MetricType type) const -> double
{
    double sum = 0.0;
    int count = 0;
    for (const auto& evt : events_)
    {
        if (evt.type == type)
        {
            sum += evt.value;
            ++count;
        }
    }
    return (count > 0) ? (sum / count) : 0.0;
}

void UxMetricsModel::set_telemetry_enabled(bool enabled)
{
    telemetry_enabled_ = enabled;
}
auto UxMetricsModel::telemetry_enabled() const -> bool
{
    return telemetry_enabled_;
}

auto UxMetricsModel::should_emit(MetricType type) const -> bool
{
    if (!telemetry_enabled_)
    {
        return false;
    }
    return std::find(excluded_types_.begin(), excluded_types_.end(), type) == excluded_types_.end();
}

void UxMetricsModel::exclude_metric(MetricType type)
{
    if (std::find(excluded_types_.begin(), excluded_types_.end(), type) == excluded_types_.end())
    {
        excluded_types_.push_back(type);
    }
}

void UxMetricsModel::include_metric(MetricType type)
{
    excluded_types_.erase(std::remove(excluded_types_.begin(), excluded_types_.end(), type),
                          excluded_types_.end());
}

} // namespace markamp::ui
