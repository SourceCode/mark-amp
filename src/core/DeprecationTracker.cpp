/// @file DeprecationTracker.cpp
/// @brief V9 Phase 50 — DeprecationTracker implementation.

#include "DeprecationTracker.h"

#include <algorithm>

namespace markamp::core
{

void DeprecationTracker::add_deprecation(DeprecationEntry entry)
{
    entries_.push_back(std::move(entry));
}

auto DeprecationTracker::is_deprecated(const std::string& feature) const -> bool
{
    return std::any_of(entries_.begin(),
                       entries_.end(),
                       [&](const DeprecationEntry& entry)
                       { return entry.feature_name == feature; });
}

auto DeprecationTracker::get_replacement(const std::string& feature) const -> std::string
{
    for (const auto& entry : entries_)
    {
        if (entry.feature_name == feature)
        {
            return entry.replacement;
        }
    }
    return "";
}

auto DeprecationTracker::get_entry(const std::string& feature) const -> const DeprecationEntry*
{
    for (const auto& entry : entries_)
    {
        if (entry.feature_name == feature)
        {
            return &entry;
        }
    }
    return nullptr;
}

auto DeprecationTracker::active_deprecations() const -> std::vector<const DeprecationEntry*>
{
    std::vector<const DeprecationEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.removed_in.empty())
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto DeprecationTracker::removed_features() const -> std::vector<const DeprecationEntry*>
{
    std::vector<const DeprecationEntry*> result;
    for (const auto& entry : entries_)
    {
        if (!entry.removed_in.empty())
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto DeprecationTracker::deprecation_count() const -> int
{
    return static_cast<int>(entries_.size());
}

void DeprecationTracker::clear()
{
    entries_.clear();
}

} // namespace markamp::core
