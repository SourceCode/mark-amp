/// @file ActivityFeed.h
/// @brief V9 Phase 39 — User activity feed for tracking vault operations.
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Categories for activity entries.
enum class ActivityCategory : uint8_t
{
    kFileEdit = 0,
    kFileCreate = 1,
    kFileDelete = 2,
    kFileRename = 3,
    kNavigation = 4,
    kSearch = 5,
    kSync = 6,
    kPlugin = 7,
    kSettings = 8,
    kOther = 9,
};

/// Convert ActivityCategory to its display name.
[[nodiscard]] auto activity_category_name(ActivityCategory category) -> std::string;

/// A single activity entry in the feed.
struct ActivityEntry
{
    std::string activity_id; ///< Unique identifier
    std::string description; ///< Human-readable description
    ActivityCategory category{ActivityCategory::kOther};
    std::string source; ///< Module/plugin source
    std::string target; ///< Target file/resource path
    std::chrono::system_clock::time_point timestamp;
    bool pinned{false}; ///< If true, stays at top
};

/// Manages a chronological feed of user activities within the workspace.
///
/// Tracks file edits, navigation, searches, sync events, and plugin actions.
/// Supports filtering by category, searching by keyword, and pinning
/// important entries.
class ActivityFeed
{
public:
    ActivityFeed() = default;

    // ── Recording ─────────────────────────────────────────────────────
    void record(ActivityEntry entry);
    [[nodiscard]] auto entry_count() const -> int;
    [[nodiscard]] auto all_entries() const -> const std::vector<ActivityEntry>&;

    // ── Lookup ────────────────────────────────────────────────────────
    [[nodiscard]] auto find_entry(const std::string& activity_id) const -> const ActivityEntry*;

    // ── Filtering ─────────────────────────────────────────────────────
    [[nodiscard]] auto entries_by_category(ActivityCategory category) const
        -> std::vector<const ActivityEntry*>;
    [[nodiscard]] auto entries_by_source(const std::string& source) const
        -> std::vector<const ActivityEntry*>;
    [[nodiscard]] auto recent_entries(int count) const -> std::vector<const ActivityEntry*>;
    [[nodiscard]] auto search(const std::string& keyword) const
        -> std::vector<const ActivityEntry*>;

    // ── Pin management ────────────────────────────────────────────────
    auto pin_entry(const std::string& activity_id) -> bool;
    auto unpin_entry(const std::string& activity_id) -> bool;
    [[nodiscard]] auto pinned_entries() const -> std::vector<const ActivityEntry*>;

    // ── Categories ────────────────────────────────────────────────────
    [[nodiscard]] auto active_categories() const -> std::vector<ActivityCategory>;

    // ── Cleanup ───────────────────────────────────────────────────────
    void set_max_entries(int max_entries);
    void clear_all();

private:
    std::vector<ActivityEntry> entries_;
    int max_entries_{1000};

    auto find_mut(const std::string& activity_id) -> ActivityEntry*;
    void enforce_limit();
};

} // namespace markamp::core
