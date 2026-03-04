#include "ScreenReaderAnnouncer.h"

#include <algorithm>

namespace markamp::ui
{

auto Announcement::priority_name() const -> std::string
{
    switch (priority)
    {
        case AnnouncementPriority::kPolite:
            return "polite";
        case AnnouncementPriority::kAssertive:
            return "assertive";
    }
    return "unknown";
}

void ScreenReaderAnnouncer::announce(const std::string& message, AnnouncementPriority priority)
{
    // Dedup: skip if last queued message is identical
    if (dedup_enabled_ && !queue_.empty() && queue_.back().message == message)
    {
        return;
    }

    Announcement ann;
    ann.message = message;
    ann.priority = priority;
    ann.timestamp_ms = current_time_ms_;

    // Assertive announcements go to front
    if (priority == AnnouncementPriority::kAssertive)
    {
        queue_.insert(queue_.begin(), ann);
    }
    else
    {
        queue_.push_back(ann);
    }
}

void ScreenReaderAnnouncer::announce_assertive(const std::string& message)
{
    announce(message, AnnouncementPriority::kAssertive);
}

auto ScreenReaderAnnouncer::pending() const -> const std::vector<Announcement>&
{
    return queue_;
}

auto ScreenReaderAnnouncer::pending_count() const -> int
{
    return static_cast<int>(queue_.size());
}

auto ScreenReaderAnnouncer::pop_next() -> Announcement
{
    if (queue_.empty())
    {
        return {"", AnnouncementPriority::kPolite, 0};
    }
    auto ann = queue_.front();
    queue_.erase(queue_.begin());
    history_.push_back(ann);
    ++total_announced_;
    return ann;
}

void ScreenReaderAnnouncer::clear()
{
    queue_.clear();
}

void ScreenReaderAnnouncer::set_throttle_ms(int interval_ms)
{
    throttle_ms_ = interval_ms;
}

auto ScreenReaderAnnouncer::throttle_ms() const -> int
{
    return throttle_ms_;
}

void ScreenReaderAnnouncer::set_dedup_enabled(bool enabled)
{
    dedup_enabled_ = enabled;
}

auto ScreenReaderAnnouncer::is_dedup_enabled() const -> bool
{
    return dedup_enabled_;
}

auto ScreenReaderAnnouncer::history(int max_count) const -> std::vector<Announcement>
{
    int start = std::max(0, static_cast<int>(history_.size()) - max_count);
    return {history_.begin() + start, history_.end()};
}

auto ScreenReaderAnnouncer::total_announced() const -> int
{
    return total_announced_;
}

} // namespace markamp::ui
