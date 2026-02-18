#include "StudyStreakTracker.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace markamp::core::fsrs
{

// ── StudyDay ────────────────────────────────────────────────────────────────

auto StudyDay::date_key() const -> std::string
{
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << year << '-' << std::setw(2) << month << '-'
        << std::setw(2) << day;
    return oss.str();
}

// ── StudyStreakTracker Public API ────────────────────────────────────────────

void StudyStreakTracker::record_review(std::chrono::system_clock::time_point timestamp)
{
    auto& study_day = get_or_create_day(timestamp);
    ++study_day.cards_reviewed;
}

void StudyStreakTracker::record_time(std::chrono::system_clock::time_point timestamp,
                                     int32_t seconds)
{
    auto& study_day = get_or_create_day(timestamp);
    study_day.time_spent_seconds += seconds;
}

auto StudyStreakTracker::current_streak() const -> int32_t
{
    auto key = today_key();

    // If no activity today, check from yesterday
    if (activity_.find(key) == activity_.end() || !activity_.at(key).has_activity())
    {
        key = prev_day(key);
        if (activity_.find(key) == activity_.end() || !activity_.at(key).has_activity())
        {
            return 0;
        }
    }

    int32_t streak = 0;
    while (activity_.find(key) != activity_.end() && activity_.at(key).has_activity())
    {
        ++streak;
        key = prev_day(key);
    }

    return streak;
}

auto StudyStreakTracker::longest_streak() const -> int32_t
{
    if (activity_.empty())
    {
        return 0;
    }

    // Collect all date keys and sort
    std::vector<std::string> keys;
    keys.reserve(activity_.size());
    for (const auto& [key, day] : activity_)
    {
        if (day.has_activity())
        {
            keys.push_back(key);
        }
    }

    if (keys.empty())
    {
        return 0;
    }

    std::sort(keys.begin(), keys.end());

    int32_t max_streak = 1;
    int32_t current = 1;

    for (size_t idx = 1; idx < keys.size(); ++idx)
    {
        if (next_day(keys[idx - 1]) == keys[idx])
        {
            ++current;
            max_streak = std::max(max_streak, current);
        }
        else
        {
            current = 1;
        }
    }

    return max_streak;
}

auto StudyStreakTracker::get_heatmap_data(int32_t days) const -> std::vector<StudyDay>
{
    std::vector<StudyDay> heatmap;
    heatmap.reserve(static_cast<size_t>(days));

    auto key = today_key();
    for (int32_t day_index = 0; day_index < days; ++day_index)
    {
        const auto it = activity_.find(key);
        if (it != activity_.end())
        {
            heatmap.push_back(it->second);
        }
        else
        {
            // Empty day
            const auto components = parse_date_key(key);
            StudyDay empty;
            empty.year = components.year;
            empty.month = components.month;
            empty.day = components.day;
            heatmap.push_back(empty);
        }
        key = prev_day(key);
    }

    return heatmap;
}

auto StudyStreakTracker::achieved_milestones() const -> std::vector<StreakMilestone>
{
    std::vector<StreakMilestone> milestones;
    const int32_t longest = longest_streak();

    if (longest >= 7)
    {
        milestones.push_back(StreakMilestone::Week7);
    }
    if (longest >= 30)
    {
        milestones.push_back(StreakMilestone::Month30);
    }
    if (longest >= 100)
    {
        milestones.push_back(StreakMilestone::Hundred100);
    }
    if (longest >= 365)
    {
        milestones.push_back(StreakMilestone::Year365);
    }

    return milestones;
}

auto StudyStreakTracker::needs_review_today() const -> bool
{
    const auto key = today_key();
    const auto it = activity_.find(key);
    if (it == activity_.end() || !it->second.has_activity())
    {
        // No activity today — check if there's a streak at risk
        const auto yesterday_key = prev_day(key);
        const auto yesterday_it = activity_.find(yesterday_key);
        return yesterday_it != activity_.end() && yesterday_it->second.has_activity();
    }
    return false; // Already reviewed today
}

auto StudyStreakTracker::total_reviews() const -> int32_t
{
    int32_t total = 0;
    for (const auto& [key, day] : activity_)
    {
        total += day.cards_reviewed;
    }
    return total;
}

// ── Persistence ─────────────────────────────────────────────────────────────

auto StudyStreakTracker::serialize() const -> std::string
{
    std::ostringstream oss;
    oss << "[\n";

    size_t idx = 0;
    for (const auto& [key, day] : activity_)
    {
        oss << "  {"
            << "\"date\": \"" << key << "\", "
            << "\"cards_reviewed\": " << day.cards_reviewed << ", "
            << "\"time_spent_seconds\": " << day.time_spent_seconds << "}";

        if (idx + 1 < activity_.size())
        {
            oss << ',';
        }
        oss << '\n';
        ++idx;
    }

    oss << "]\n";
    return oss.str();
}

auto StudyStreakTracker::deserialize(const std::string& json) -> std::expected<void, std::string>
{
    activity_.clear();

    if (json.empty())
    {
        return std::unexpected("JSON data is empty");
    }

    // Simple key-value extraction from JSON array of objects
    // Look for patterns: "date": "YYYY-MM-DD", "cards_reviewed": N, "time_spent_seconds": N
    size_t pos = 0;
    while ((pos = json.find("\"date\"", pos)) != std::string::npos)
    {
        // Extract date value
        const auto date_start = json.find('"', pos + 6);
        if (date_start == std::string::npos)
        {
            break;
        }
        const auto date_start_inner = date_start + 1;
        const auto date_end = json.find('"', date_start_inner);
        if (date_end == std::string::npos)
        {
            break;
        }
        const auto date_str = json.substr(date_start_inner, date_end - date_start_inner);

        // Extract cards_reviewed
        int32_t cards = 0;
        const auto cards_key_pos = json.find("\"cards_reviewed\"", date_end);
        if (cards_key_pos != std::string::npos)
        {
            const auto colon_pos = json.find(':', cards_key_pos + 16);
            if (colon_pos != std::string::npos)
            {
                cards = std::stoi(json.substr(colon_pos + 1));
            }
        }

        // Extract time_spent_seconds
        int32_t time_spent = 0;
        const auto time_key_pos = json.find("\"time_spent_seconds\"", date_end);
        if (time_key_pos != std::string::npos)
        {
            const auto colon_pos = json.find(':', time_key_pos + 20);
            if (colon_pos != std::string::npos)
            {
                time_spent = std::stoi(json.substr(colon_pos + 1));
            }
        }

        const auto components = parse_date_key(date_str);
        StudyDay day;
        day.year = components.year;
        day.month = components.month;
        day.day = components.day;
        day.cards_reviewed = cards;
        day.time_spent_seconds = time_spent;
        activity_[date_str] = day;

        pos = date_end + 1;
    }

    return {};
}

// ── Private Helpers ─────────────────────────────────────────────────────────

auto StudyStreakTracker::get_or_create_day(std::chrono::system_clock::time_point timestamp)
    -> StudyDay&
{
    const auto key = to_date_key(timestamp);
    auto it = activity_.find(key);
    if (it == activity_.end())
    {
        const auto components = parse_date_key(key);
        StudyDay day;
        day.year = components.year;
        day.month = components.month;
        day.day = components.day;
        auto [inserted_it, success] = activity_.emplace(key, day);
        return inserted_it->second;
    }
    return it->second;
}

auto StudyStreakTracker::to_date_key(std::chrono::system_clock::time_point timestamp) -> std::string
{
    const auto time_c = std::chrono::system_clock::to_time_t(timestamp);
    struct tm time_tm
    {
    };

#if defined(_WIN32)
    localtime_s(&time_tm, &time_c);
#else
    localtime_r(&time_c, &time_tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&time_tm, "%Y-%m-%d");
    return oss.str();
}

auto StudyStreakTracker::today_key() -> std::string
{
    return to_date_key(std::chrono::system_clock::now());
}

auto StudyStreakTracker::date_key_minus_days(const std::string& date_key, int32_t days)
    -> std::string
{
    auto result = date_key;
    for (int32_t idx = 0; idx < days; ++idx)
    {
        result = prev_day(result);
    }
    return result;
}

auto StudyStreakTracker::parse_date_key(const std::string& key) -> DateComponents
{
    DateComponents components;

    if (key.size() < 10)
    {
        return components;
    }

    components.year = std::stoi(key.substr(0, 4));
    components.month = std::stoi(key.substr(5, 2));
    components.day = std::stoi(key.substr(8, 2));
    return components;
}

auto StudyStreakTracker::next_day(const std::string& date_key) -> std::string
{
    const auto components = parse_date_key(date_key);
    struct tm time_tm
    {
    };
    time_tm.tm_year = components.year - 1900;
    time_tm.tm_mon = components.month - 1;
    time_tm.tm_mday = components.day + 1; // mktime normalizes overflow
    time_tm.tm_hour = 12;                 // Avoid DST edge cases

    const auto time_c = mktime(&time_tm);
    struct tm result_tm
    {
    };

#if defined(_WIN32)
    localtime_s(&result_tm, &time_c);
#else
    localtime_r(&time_c, &result_tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&result_tm, "%Y-%m-%d");
    return oss.str();
}

auto StudyStreakTracker::prev_day(const std::string& date_key) -> std::string
{
    const auto components = parse_date_key(date_key);
    struct tm time_tm
    {
    };
    time_tm.tm_year = components.year - 1900;
    time_tm.tm_mon = components.month - 1;
    time_tm.tm_mday = components.day - 1; // mktime normalizes underflow
    time_tm.tm_hour = 12;

    const auto time_c = mktime(&time_tm);
    struct tm result_tm
    {
    };

#if defined(_WIN32)
    localtime_s(&result_tm, &time_c);
#else
    localtime_r(&time_c, &result_tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&result_tm, "%Y-%m-%d");
    return oss.str();
}

} // namespace markamp::core::fsrs
