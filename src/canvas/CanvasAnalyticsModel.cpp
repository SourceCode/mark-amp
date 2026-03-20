#include "canvas/CanvasAnalyticsModel.h"

#include <algorithm>

namespace markamp::canvas
{

auto CanvasAnalyticsModel::record_event(const std::string& event_name,
                                         const std::string& category) -> void
{
    if (opted_out_)
    {
        return;
    }
    AnalyticsEntry entry;
    entry.event_name = event_name;
    entry.category = category;
    events_.push_back(entry);
}

auto CanvasAnalyticsModel::events() const -> const std::vector<AnalyticsEntry>&
{
    return events_;
}

auto CanvasAnalyticsModel::pending_count() const -> int
{
    return static_cast<int>(events_.size());
}

auto CanvasAnalyticsModel::start_flush() -> void
{
    flush_state_ = AnalyticsFlushState::kFlushing;
}

auto CanvasAnalyticsModel::complete_flush() -> void
{
    events_.clear();
    flush_state_ = AnalyticsFlushState::kIdle;
}

auto CanvasAnalyticsModel::fail_flush() -> void
{
    flush_state_ = AnalyticsFlushState::kFailed;
}

auto CanvasAnalyticsModel::flush_state() const -> AnalyticsFlushState
{
    return flush_state_;
}

auto CanvasAnalyticsModel::set_opted_out(bool opted_out) -> void
{
    opted_out_ = opted_out;
}

auto CanvasAnalyticsModel::is_opted_out() const -> bool
{
    return opted_out_;
}

auto CanvasAnalyticsModel::count_by_category(const std::string& category) const -> int
{
    return static_cast<int>(
        std::count_if(events_.begin(), events_.end(),
                      [&category](const AnalyticsEntry& entry) { return entry.category == category; }));
}

} // namespace markamp::canvas
