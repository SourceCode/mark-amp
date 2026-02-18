/// @file ActivityTimeline.h
/// @brief V9 Phase 39 — Timeline visualization of activity over date ranges.
#pragma once

#include "ActivityFeed.h"

#include <chrono>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace markamp::core
{

/// Summary statistics for one calendar day.
struct DailySummary
{
    std::string date; ///< "YYYY-MM-DD"
    int total_events{0};
    int file_edits{0};
    int file_creates{0};
    int navigations{0};
    int searches{0};
    int sync_events{0};
    int plugin_events{0};
    int other_events{0};
};

/// A streak of consecutive active days.
struct ActivityStreak
{
    std::string start_date; ///< "YYYY-MM-DD"
    std::string end_date;   ///< "YYYY-MM-DD"
    int length{0};          ///< Days in streak
};

/// Timeline that aggregates ActivityFeed entries by date.
///
/// Provides daily summaries, streak tracking, most-active-day computation,
/// and JSON export for visualization. Designed to power a GitHub-style
/// activity heatmap / timeline in the UI.
class ActivityTimeline
{
public:
    ActivityTimeline() = default;

    // ── Build from feed ───────────────────────────────────────────────
    /// Rebuild timeline data from an activity feed snapshot.
    void build_from_feed(const ActivityFeed& feed);

    // ── Date queries ──────────────────────────────────────────────────
    [[nodiscard]] auto summary_for_date(const std::string& date) const -> const DailySummary*;
    [[nodiscard]] auto summaries_in_range(const std::string& start_date,
                                          const std::string& end_date) const
        -> std::vector<DailySummary>;
    [[nodiscard]] auto all_summaries() const -> const std::map<std::string, DailySummary>&;
    [[nodiscard]] auto active_days() const -> int;

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto total_events() const -> int;
    [[nodiscard]] auto most_active_day() const -> std::string;
    [[nodiscard]] auto most_active_category() const -> ActivityCategory;
    [[nodiscard]] auto current_streak() const -> ActivityStreak;
    [[nodiscard]] auto longest_streak() const -> ActivityStreak;

    // ── Export ─────────────────────────────────────────────────────────
    [[nodiscard]] auto export_json() const -> std::string;

    void clear();

private:
    std::map<std::string, DailySummary> daily_summaries_;

    static auto time_point_to_date(std::chrono::system_clock::time_point tp) -> std::string;
    void add_entry_to_summary(DailySummary& summary, const ActivityEntry& entry);
};

} // namespace markamp::core
