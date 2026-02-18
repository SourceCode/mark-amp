#include "AVTimelineView.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <ctime>
#include <map>
#include <sstream>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Granularity string conversion
// ════════════════════════════════════════════════════════════

auto granularity_to_string(AVTimelineGranularity gran) -> std::string
{
    switch (gran)
    {
        case AVTimelineGranularity::Day:
            return "day";
        case AVTimelineGranularity::Week:
            return "week";
        case AVTimelineGranularity::Month:
            return "month";
        case AVTimelineGranularity::Quarter:
            return "quarter";
        case AVTimelineGranularity::Year:
            return "year";
    }
    return "month";
}

auto string_to_granularity(const std::string& str) -> AVTimelineGranularity
{
    if (str == "day")
    {
        return AVTimelineGranularity::Day;
    }
    if (str == "week")
    {
        return AVTimelineGranularity::Week;
    }
    if (str == "month")
    {
        return AVTimelineGranularity::Month;
    }
    if (str == "quarter")
    {
        return AVTimelineGranularity::Quarter;
    }
    if (str == "year")
    {
        return AVTimelineGranularity::Year;
    }
    return AVTimelineGranularity::Month;
}

// ════════════════════════════════════════════════════════════
// Configuration
// ════════════════════════════════════════════════════════════

void AVTimelineView::set_date_key(const std::string& key_id)
{
    date_key_id_ = key_id;
}

auto AVTimelineView::date_key() const -> std::string
{
    return date_key_id_;
}

void AVTimelineView::set_granularity(AVTimelineGranularity granularity)
{
    granularity_ = granularity;
}

auto AVTimelineView::granularity() const -> AVTimelineGranularity
{
    return granularity_;
}

// ════════════════════════════════════════════════════════════
// Time helpers: break down timestamps into calendar components
// ════════════════════════════════════════════════════════════

namespace
{

struct CalendarDate
{
    int year{0};
    int month{0};   // 1-12
    int day{0};     // 1-31
    int weekday{0}; // 0=Sun
};

auto ts_to_calendar(int64_t timestamp_ms) -> CalendarDate
{
    auto seconds = static_cast<time_t>(timestamp_ms / 1000);
    struct tm time_struct
    {
    };
    gmtime_r(&seconds, &time_struct);
    CalendarDate cal;
    cal.year = time_struct.tm_year + 1900;
    cal.month = time_struct.tm_mon + 1;
    cal.day = time_struct.tm_mday;
    cal.weekday = time_struct.tm_wday;
    return cal;
}

auto calendar_to_ts(int year, int month, int day) -> int64_t
{
    struct tm time_struct
    {
    };
    time_struct.tm_year = year - 1900;
    time_struct.tm_mon = month - 1;
    time_struct.tm_mday = day;
    time_struct.tm_hour = 0;
    time_struct.tm_min = 0;
    time_struct.tm_sec = 0;
    time_struct.tm_isdst = 0;
    auto seconds = timegm(&time_struct);
    return static_cast<int64_t>(seconds) * 1000;
}

auto month_name(int month) -> std::string
{
    static const std::array<const char*, 13> kNames = {
        "", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    if (month >= 1 && month <= 12)
    {
        return kNames.at(static_cast<size_t>(month));
    }
    return "???";
}

} // namespace

// ════════════════════════════════════════════════════════════
// Period calculations
// ════════════════════════════════════════════════════════════

auto AVTimelineView::period_start(int64_t timestamp) const -> int64_t
{
    auto cal = ts_to_calendar(timestamp);

    switch (granularity_)
    {
        case AVTimelineGranularity::Day:
            return calendar_to_ts(cal.year, cal.month, cal.day);

        case AVTimelineGranularity::Week:
        {
            // Go back to the most recent Monday
            const int days_since_monday = (cal.weekday + 6) % 7;
            auto day_ts = calendar_to_ts(cal.year, cal.month, cal.day);
            return day_ts - static_cast<int64_t>(days_since_monday) * 86400 * 1000;
        }

        case AVTimelineGranularity::Month:
            return calendar_to_ts(cal.year, cal.month, 1);

        case AVTimelineGranularity::Quarter:
        {
            const int quarter_month = ((cal.month - 1) / 3) * 3 + 1;
            return calendar_to_ts(cal.year, quarter_month, 1);
        }

        case AVTimelineGranularity::Year:
            return calendar_to_ts(cal.year, 1, 1);
    }

    return timestamp;
}

auto AVTimelineView::period_end(int64_t period_start_ts) const -> int64_t
{
    auto cal = ts_to_calendar(period_start_ts);

    switch (granularity_)
    {
        case AVTimelineGranularity::Day:
            return period_start_ts + 86400LL * 1000;

        case AVTimelineGranularity::Week:
            return period_start_ts + 7LL * 86400 * 1000;

        case AVTimelineGranularity::Month:
        {
            int next_month = cal.month + 1;
            int next_year = cal.year;
            if (next_month > 12)
            {
                next_month = 1;
                ++next_year;
            }
            return calendar_to_ts(next_year, next_month, 1);
        }

        case AVTimelineGranularity::Quarter:
        {
            int next_quarter_month = cal.month + 3;
            int next_year = cal.year;
            if (next_quarter_month > 12)
            {
                next_quarter_month -= 12;
                ++next_year;
            }
            return calendar_to_ts(next_year, next_quarter_month, 1);
        }

        case AVTimelineGranularity::Year:
            return calendar_to_ts(cal.year + 1, 1, 1);
    }

    return period_start_ts;
}

auto AVTimelineView::period_label(int64_t period_start_ts) const -> std::string
{
    auto cal = ts_to_calendar(period_start_ts);
    std::ostringstream oss;

    switch (granularity_)
    {
        case AVTimelineGranularity::Day:
            oss << cal.year << "-" << (cal.month < 10 ? "0" : "") << cal.month << "-"
                << (cal.day < 10 ? "0" : "") << cal.day;
            break;

        case AVTimelineGranularity::Week:
            oss << "Week of " << cal.year << "-" << (cal.month < 10 ? "0" : "") << cal.month << "-"
                << (cal.day < 10 ? "0" : "") << cal.day;
            break;

        case AVTimelineGranularity::Month:
            oss << month_name(cal.month) << " " << cal.year;
            break;

        case AVTimelineGranularity::Quarter:
        {
            const int quarter = (cal.month - 1) / 3 + 1;
            oss << "Q" << quarter << " " << cal.year;
            break;
        }

        case AVTimelineGranularity::Year:
            oss << cal.year;
            break;
    }

    return oss.str();
}

// ════════════════════════════════════════════════════════════
// Row timestamp extraction
// ════════════════════════════════════════════════════════════

auto AVTimelineView::get_row_timestamp(const AttributeView& attribute_view,
                                       const std::string& block_id) const -> int64_t
{
    if (date_key_id_.empty())
    {
        return 0;
    }

    const auto* val = attribute_view.get_value(date_key_id_, block_id);
    if (val == nullptr)
    {
        return 0;
    }

    if (const auto* date = val->as_date())
    {
        return date->content;
    }

    return 0;
}

// ════════════════════════════════════════════════════════════
// Bucketing
// ════════════════════════════════════════════════════════════

auto AVTimelineView::bucket_rows(const AttributeView& attribute_view) const
    -> std::vector<AVTimelineBucket>
{
    // Use ordered map to keep buckets sorted by start timestamp
    std::map<int64_t, AVTimelineBucket> bucket_map;

    auto block_ids = attribute_view.row_block_ids();

    for (const auto& bid : block_ids)
    {
        const int64_t row_time = get_row_timestamp(attribute_view, bid);
        if (row_time == 0)
        {
            continue; // Skip unscheduled rows
        }

        const int64_t bucket_start = period_start(row_time);
        auto iter = bucket_map.find(bucket_start);
        if (iter == bucket_map.end())
        {
            AVTimelineBucket bucket;
            bucket.start_ts = bucket_start;
            bucket.end_ts = period_end(bucket_start);
            bucket.label = period_label(bucket_start);
            bucket.block_ids.push_back(bid);
            bucket_map[bucket_start] = std::move(bucket);
        }
        else
        {
            iter->second.block_ids.push_back(bid);
        }
    }

    std::vector<AVTimelineBucket> result;
    result.reserve(bucket_map.size());
    for (auto& [_, bucket] : bucket_map)
    {
        result.push_back(std::move(bucket));
    }

    return result;
}

// ════════════════════════════════════════════════════════════
// Range queries
// ════════════════════════════════════════════════════════════

auto AVTimelineView::rows_in_range(const AttributeView& attribute_view,
                                   int64_t range_start,
                                   int64_t range_end) const -> std::vector<std::string>
{
    std::vector<std::string> result;
    auto block_ids = attribute_view.row_block_ids();

    for (const auto& bid : block_ids)
    {
        const int64_t row_time = get_row_timestamp(attribute_view, bid);
        if (row_time >= range_start && row_time < range_end)
        {
            result.push_back(bid);
        }
    }

    return result;
}

auto AVTimelineView::unscheduled_rows(const AttributeView& attribute_view) const
    -> std::vector<std::string>
{
    std::vector<std::string> result;
    auto block_ids = attribute_view.row_block_ids();

    for (const auto& bid : block_ids)
    {
        const int64_t row_time = get_row_timestamp(attribute_view, bid);
        if (row_time == 0)
        {
            result.push_back(bid);
        }
    }

    return result;
}

// ════════════════════════════════════════════════════════════
// Move row (drag-and-drop)
// ════════════════════════════════════════════════════════════

auto AVTimelineView::move_row(AttributeView& attribute_view,
                              const std::string& block_id,
                              int64_t new_timestamp) const -> std::expected<void, std::string>
{
    if (date_key_id_.empty())
    {
        return std::unexpected("No date key configured for timeline view");
    }

    AVValueDate date_val;
    date_val.content = new_timestamp;
    date_val.is_not_time = false;
    date_val.has_end_date = false;
    date_val.content2 = 0;
    attribute_view.set_value(date_key_id_, block_id, date_val);

    return {};
}

// ════════════════════════════════════════════════════════════
// Get bucket for timestamp
// ════════════════════════════════════════════════════════════

auto AVTimelineView::get_bucket_for_timestamp(int64_t timestamp) const -> AVTimelineBucket
{
    const int64_t bucket_start = period_start(timestamp);
    AVTimelineBucket bucket;
    bucket.start_ts = bucket_start;
    bucket.end_ts = period_end(bucket_start);
    bucket.label = period_label(bucket_start);
    return bucket;
}

} // namespace markamp::core::av
