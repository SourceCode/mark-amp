/// @file ChangeTracker.h
/// @brief V13 Phase 32 Task 13 — Tracks changed lines since last save.
#pragma once

#include <set>
#include <vector>

namespace markamp::core
{

/// Tracks which lines have been modified since the last save.
///
/// Used for gutter change markers and next/prev change navigation.
class ChangeTracker
{
public:
    /// Record that a line was edited.
    void record_edit(int line);

    /// Record that a range of lines was edited.
    void record_edit_range(int start_line, int end_line);

    /// Mark the document as saved (clears all change tracking).
    void mark_saved();

    /// Get the next changed line after the given line.
    /// Returns -1 if no changes exist after the given line.
    /// Wraps to the first change if past the last change.
    [[nodiscard]] auto next_change(int current_line) const -> int;

    /// Get the previous changed line before the given line.
    /// Returns -1 if no changes exist before the given line.
    /// Wraps to the last change if before the first change.
    [[nodiscard]] auto prev_change(int current_line) const -> int;

    /// Check if a specific line has changes.
    [[nodiscard]] auto is_changed(int line) const -> bool;

    /// Get all changed lines (sorted).
    [[nodiscard]] auto changed_lines() const -> std::vector<int>;

    /// Get the total number of changed lines.
    [[nodiscard]] auto change_count() const -> size_t;

    /// Check if there are any changes.
    [[nodiscard]] auto has_changes() const -> bool;

private:
    std::set<int> changed_lines_;
};

} // namespace markamp::core
