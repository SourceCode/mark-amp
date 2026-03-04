// ============================================================================
// File: src/ui/MergeEditor.h
// Phase 46: Diff Editor and Merge Tool — Merge editor model
// ============================================================================
#pragma once

#include "../core/MergeEngine.h"

#include <string>
#include <vector>

namespace markamp::ui
{

/// Model for the 3-way merge editor UI.
/// Tracks merge state, conflict resolution actions, unresolved count.
class MergeEditorModel
{
public:
    MergeEditorModel() = default;

    /// Initialize with a merge result.
    void set_merge_result(core::MergeResult result);

    /// Get the current merge result.
    [[nodiscard]] auto merge_result() const -> const core::MergeResult&
    {
        return merge_result_;
    }

    /// Accept theirs for a conflict.
    void accept_theirs(size_t conflict_index);

    /// Accept yours for a conflict.
    void accept_yours(size_t conflict_index);

    /// Accept both (theirs then yours).
    void accept_both(size_t conflict_index);

    /// Accept all theirs.
    void accept_all_theirs();

    /// Accept all yours.
    void accept_all_yours();

    /// Get the number of unresolved conflicts.
    [[nodiscard]] auto unresolved_count() const -> int
    {
        return merge_result_.unresolved_count();
    }

    /// Whether the merge is complete.
    [[nodiscard]] auto is_complete() const -> bool
    {
        return merge_result_.is_fully_resolved();
    }

    /// Get final merged text.
    [[nodiscard]] auto merged_text() const -> std::string
    {
        return merge_result_.merged_text();
    }

    /// Navigate to the next conflict. Returns conflict index.
    [[nodiscard]] auto navigate_next_conflict() -> int;

    /// Navigate to the previous conflict.
    [[nodiscard]] auto navigate_prev_conflict() -> int;

    /// Current conflict index.
    [[nodiscard]] auto current_conflict_index() const -> int
    {
        return current_conflict_;
    }

    /// Total conflict count.
    [[nodiscard]] auto conflict_count() const -> int
    {
        return static_cast<int>(merge_result_.conflicts.size());
    }

private:
    core::MergeResult merge_result_;
    int current_conflict_{-1};
};

} // namespace markamp::ui
