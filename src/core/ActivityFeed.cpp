/// @file ActivityFeed.cpp
/// @brief V9 Phase 39 — ActivityFeed implementation.

#include "ActivityFeed.h"

#include <algorithm>
#include <set>

namespace markamp::core
{

auto activity_category_name(ActivityCategory category) -> std::string
{
    switch (category)
    {
        case ActivityCategory::kFileEdit:
            return "file_edit";
        case ActivityCategory::kFileCreate:
            return "file_create";
        case ActivityCategory::kFileDelete:
            return "file_delete";
        case ActivityCategory::kFileRename:
            return "file_rename";
        case ActivityCategory::kNavigation:
            return "navigation";
        case ActivityCategory::kSearch:
            return "search";
        case ActivityCategory::kSync:
            return "sync";
        case ActivityCategory::kPlugin:
            return "plugin";
        case ActivityCategory::kSettings:
            return "settings";
        case ActivityCategory::kOther:
            return "other";
    }
    return "other";
}

void ActivityFeed::record(ActivityEntry entry)
{
    if (entry.activity_id.empty())
    {
        entry.activity_id = "act_" + std::to_string(entries_.size());
    }
    if (entry.timestamp == std::chrono::system_clock::time_point{})
    {
        entry.timestamp = std::chrono::system_clock::now();
    }
    entries_.push_back(std::move(entry));
    enforce_limit();
}

auto ActivityFeed::entry_count() const -> int
{
    return static_cast<int>(entries_.size());
}

auto ActivityFeed::all_entries() const -> const std::vector<ActivityEntry>&
{
    return entries_;
}

auto ActivityFeed::find_entry(const std::string& activity_id) const -> const ActivityEntry*
{
    for (const auto& entry : entries_)
    {
        if (entry.activity_id == activity_id)
        {
            return &entry;
        }
    }
    return nullptr;
}

// ── Filtering ─────────────────────────────────────────────────────

auto ActivityFeed::entries_by_category(ActivityCategory category) const
    -> std::vector<const ActivityEntry*>
{
    std::vector<const ActivityEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.category == category)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto ActivityFeed::entries_by_source(const std::string& source) const
    -> std::vector<const ActivityEntry*>
{
    std::vector<const ActivityEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.source == source)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto ActivityFeed::recent_entries(int count) const -> std::vector<const ActivityEntry*>
{
    std::vector<const ActivityEntry*> result;
    int start = std::max(0, static_cast<int>(entries_.size()) - count);
    for (int idx = static_cast<int>(entries_.size()) - 1; idx >= start; --idx)
    {
        result.push_back(&entries_[static_cast<size_t>(idx)]);
    }
    return result;
}

auto ActivityFeed::search(const std::string& keyword) const -> std::vector<const ActivityEntry*>
{
    std::vector<const ActivityEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.description.find(keyword) != std::string::npos ||
            entry.target.find(keyword) != std::string::npos ||
            entry.source.find(keyword) != std::string::npos)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

// ── Pin management ────────────────────────────────────────────────

auto ActivityFeed::pin_entry(const std::string& activity_id) -> bool
{
    auto* entry = find_mut(activity_id);
    if (entry == nullptr)
    {
        return false;
    }
    entry->pinned = true;
    return true;
}

auto ActivityFeed::unpin_entry(const std::string& activity_id) -> bool
{
    auto* entry = find_mut(activity_id);
    if (entry == nullptr)
    {
        return false;
    }
    entry->pinned = false;
    return true;
}

auto ActivityFeed::pinned_entries() const -> std::vector<const ActivityEntry*>
{
    std::vector<const ActivityEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.pinned)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

// ── Categories ────────────────────────────────────────────────────

auto ActivityFeed::active_categories() const -> std::vector<ActivityCategory>
{
    std::set<ActivityCategory> seen;
    for (const auto& entry : entries_)
    {
        seen.insert(entry.category);
    }
    return {seen.begin(), seen.end()};
}

// ── Cleanup ───────────────────────────────────────────────────────

void ActivityFeed::set_max_entries(int max_entries)
{
    max_entries_ = max_entries;
    enforce_limit();
}

// (#64) Clear entries of a specific category, preserving pinned.
void ActivityFeed::clear_category(ActivityCategory category)
{
    entries_.erase(std::remove_if(entries_.begin(),
                                  entries_.end(),
                                  [category](const ActivityEntry& entry)
                                  { return entry.category == category && !entry.pinned; }),
                   entries_.end());
}

void ActivityFeed::clear_all()
{
    entries_.clear();
}

auto ActivityFeed::find_mut(const std::string& activity_id) -> ActivityEntry*
{
    for (auto& entry : entries_)
    {
        if (entry.activity_id == activity_id)
        {
            return &entry;
        }
    }
    return nullptr;
}

void ActivityFeed::enforce_limit()
{
    while (static_cast<int>(entries_.size()) > max_entries_)
    {
        // Remove oldest non-pinned entry
        auto iter = std::find_if(entries_.begin(),
                                 entries_.end(),
                                 [](const ActivityEntry& entry) { return !entry.pinned; });
        if (iter != entries_.end())
        {
            entries_.erase(iter);
        }
        else
        {
            break; // All entries are pinned
        }
    }
}

} // namespace markamp::core
