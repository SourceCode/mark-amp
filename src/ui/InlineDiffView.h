// ============================================================================
// File: src/ui/InlineDiffView.h
// Phase 46: Diff Editor and Merge Tool — Inline diff view model
// ============================================================================
#pragma once

#include "../core/HistoryTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// A rendered line in the inline diff view.
struct InlineDiffLine
{
    std::string content;
    core::DiffLineType type{core::DiffLineType::Context};
    int old_line_number{-1};
    int new_line_number{-1};
};

/// Model for inline (unified) diff view.
/// Interleaves old and new content in a single editor, with deleted lines
/// shown as strikethrough and added lines highlighted.
class InlineDiffViewModel
{
public:
    InlineDiffViewModel() = default;

    /// Build the inline line list from a diff result and original lines.
    void build(const core::DiffResult& diff,
               const std::vector<std::string>& old_lines,
               const std::vector<std::string>& new_lines);

    /// Get the interleaved lines.
    [[nodiscard]] auto lines() const -> const std::vector<InlineDiffLine>&
    {
        return lines_;
    }

    /// Total line count.
    [[nodiscard]] auto total_lines() const -> int
    {
        return static_cast<int>(lines_.size());
    }

    /// Navigate to next change. Returns line index.
    [[nodiscard]] auto navigate_next() -> int;

    /// Navigate to previous change.
    [[nodiscard]] auto navigate_prev() -> int;

    /// Current change index.
    [[nodiscard]] auto current_change_index() const -> int
    {
        return current_change_;
    }

    /// Total change count.
    [[nodiscard]] auto change_count() const -> int
    {
        return static_cast<int>(change_line_indices_.size());
    }

private:
    std::vector<InlineDiffLine> lines_;
    std::vector<int> change_line_indices_;
    int current_change_{-1};
};

} // namespace markamp::ui
