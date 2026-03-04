/// @file ChangeTracker.cpp
/// @brief V13 Phase 32 Task 13 — Change tracking implementation.

#include "core/ChangeTracker.h"

namespace markamp::core
{

void ChangeTracker::record_edit(int line)
{
    changed_lines_.insert(line);
}

void ChangeTracker::record_edit_range(int start_line, int end_line)
{
    for (int line = start_line; line <= end_line; ++line)
    {
        changed_lines_.insert(line);
    }
}

void ChangeTracker::mark_saved()
{
    changed_lines_.clear();
}

auto ChangeTracker::next_change(int current_line) const -> int
{
    if (changed_lines_.empty())
    {
        return -1;
    }

    // Find the first change strictly after current_line
    auto iter = changed_lines_.upper_bound(current_line);
    if (iter != changed_lines_.end())
    {
        return *iter;
    }

    // Wrap to the first change
    return *changed_lines_.begin();
}

auto ChangeTracker::prev_change(int current_line) const -> int
{
    if (changed_lines_.empty())
    {
        return -1;
    }

    // Find the first change strictly before current_line
    auto iter = changed_lines_.lower_bound(current_line);
    if (iter != changed_lines_.begin())
    {
        --iter;
        return *iter;
    }

    // Wrap to the last change
    return *changed_lines_.rbegin();
}

auto ChangeTracker::is_changed(int line) const -> bool
{
    return changed_lines_.count(line) > 0;
}

auto ChangeTracker::changed_lines() const -> std::vector<int>
{
    return {changed_lines_.begin(), changed_lines_.end()};
}

auto ChangeTracker::change_count() const -> size_t
{
    return changed_lines_.size();
}

auto ChangeTracker::has_changes() const -> bool
{
    return !changed_lines_.empty();
}

} // namespace markamp::core
