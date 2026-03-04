// ============================================================================
// File: src/ui/SideBySideDiffView.h
// Phase 46: Diff Editor and Merge Tool — Side-by-side diff view model
// ============================================================================
#pragma once

#include "../core/HistoryTypes.h"
#include "../core/WordDiffEngine.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Side of the diff panel.
enum class DiffSide : uint8_t
{
    Left,
    Right
};

/// A rendered line in the side-by-side view.
struct SideBySideLine
{
    std::string content;
    int original_line_number{-1}; ///< -1 for filler lines
    bool is_filler{false};        ///< Blank alignment filler
    bool is_added{false};
    bool is_deleted{false};
    bool is_modified{false};
    std::vector<core::WordSpan> word_spans; ///< Word-level highlights
};

/// Model for side-by-side diff view.
/// Manages alignment filler lines, scroll sync, word-level highlight spans.
class SideBySideDiffViewModel
{
public:
    SideBySideDiffViewModel() = default;

    /// Build the aligned line lists from a diff result and the original lines.
    void build(const core::DiffResult& diff,
               const std::vector<std::string>& old_lines,
               const std::vector<std::string>& new_lines,
               bool enable_word_diff = false);

    /// Get left (old) side lines.
    [[nodiscard]] auto left_lines() const -> const std::vector<SideBySideLine>&
    {
        return left_lines_;
    }

    /// Get right (new) side lines.
    [[nodiscard]] auto right_lines() const -> const std::vector<SideBySideLine>&
    {
        return right_lines_;
    }

    /// Total aligned line count (both sides have the same count due to fillers).
    [[nodiscard]] auto total_lines() const -> int
    {
        return static_cast<int>(left_lines_.size());
    }

    /// Enable/disable scroll synchronization.
    void set_scroll_sync(bool enabled)
    {
        scroll_sync_enabled_ = enabled;
    }
    [[nodiscard]] auto scroll_sync_enabled() const -> bool
    {
        return scroll_sync_enabled_;
    }

    /// Navigate to the next change. Returns the aligned line index.
    [[nodiscard]] auto navigate_next() -> int;

    /// Navigate to the previous change.
    [[nodiscard]] auto navigate_prev() -> int;

    /// Get current change index (0-based).
    [[nodiscard]] auto current_change_index() const -> int
    {
        return current_change_;
    }

    /// Get total number of change regions.
    [[nodiscard]] auto change_count() const -> int
    {
        return static_cast<int>(change_line_indices_.size());
    }

private:
    std::vector<SideBySideLine> left_lines_;
    std::vector<SideBySideLine> right_lines_;
    std::vector<int> change_line_indices_; ///< Aligned line indices for each change
    int current_change_{-1};
    bool scroll_sync_enabled_{true};
};

} // namespace markamp::ui
