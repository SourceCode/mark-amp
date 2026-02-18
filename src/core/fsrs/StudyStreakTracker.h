#pragma once

#include <chrono>
#include <cstdint>
#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core::fsrs
{

// ============================================================================
// Study Day: review activity for a single calendar day
// ============================================================================

struct StudyDay
{
    int32_t year{0};
    int32_t month{0};
    int32_t day{0};
    int32_t cards_reviewed{0};
    int32_t time_spent_seconds{0};

    /// Get a comparable date key (YYYY-MM-DD).
    [[nodiscard]] auto date_key() const -> std::string;

    /// Check if this day has any reviews recorded.
    [[nodiscard]] auto has_activity() const -> bool
    {
        return cards_reviewed > 0;
    }
};

// ============================================================================
// Streak Milestone: achievement badges for consistent study
// ============================================================================

enum class StreakMilestone
{
    Week7,      // 7-day streak
    Month30,    // 30-day streak
    Hundred100, // 100-day streak
    Year365     // 365-day streak
};

// ============================================================================
// StudyStreakTracker: tracks daily review streaks and activity heatmap
// ============================================================================

class StudyStreakTracker
{
public:
    StudyStreakTracker() = default;

    /// Record a review event at the given timestamp.
    /// Increments the card count for that day.
    void record_review(std::chrono::system_clock::time_point timestamp);

    /// Record time spent reviewing on a specific day.
    void record_time(std::chrono::system_clock::time_point timestamp, int32_t seconds);

    /// Get the current consecutive-day streak (ending today or yesterday).
    [[nodiscard]] auto current_streak() const -> int32_t;

    /// Get the longest streak ever achieved.
    [[nodiscard]] auto longest_streak() const -> int32_t;

    /// Get heatmap data for the last N days.
    [[nodiscard]] auto get_heatmap_data(int32_t days) const -> std::vector<StudyDay>;

    /// Get all milestones that have been achieved.
    [[nodiscard]] auto achieved_milestones() const -> std::vector<StreakMilestone>;

    /// Check if the user needs to review today to maintain their streak.
    [[nodiscard]] auto needs_review_today() const -> bool;

    /// Get the total number of reviews across all time.
    [[nodiscard]] auto total_reviews() const -> int32_t;

    // ── Persistence ──

    /// Serialize tracker state to JSON.
    [[nodiscard]] auto serialize() const -> std::string;

    /// Deserialize tracker state from JSON.
    [[nodiscard]] auto deserialize(const std::string& json) -> std::expected<void, std::string>;

private:
    /// Date-keyed activity map. Key = "YYYY-MM-DD"
    std::unordered_map<std::string, StudyDay> activity_;

    /// Get or create a StudyDay entry for the given timestamp.
    auto get_or_create_day(std::chrono::system_clock::time_point timestamp) -> StudyDay&;

    /// Convert a time_point to a date key string.
    [[nodiscard]] static auto to_date_key(std::chrono::system_clock::time_point timestamp)
        -> std::string;

    /// Get today's date key.
    [[nodiscard]] static auto today_key() -> std::string;

    /// Get the date key for N days before the given date key.
    [[nodiscard]] static auto date_key_minus_days(const std::string& date_key, int32_t days)
        -> std::string;

    /// Parse a date key into year/month/day.
    struct DateComponents
    {
        int32_t year{0};
        int32_t month{0};
        int32_t day{0};
    };
    [[nodiscard]] static auto parse_date_key(const std::string& key) -> DateComponents;

    /// Advance a date key by one day.
    [[nodiscard]] static auto next_day(const std::string& date_key) -> std::string;

    /// Go back one day from a date key.
    [[nodiscard]] static auto prev_day(const std::string& date_key) -> std::string;
};

} // namespace markamp::core::fsrs
