// ============================================================================
// File: src/ui/DiffGutter.h
// Phase 46: Diff Editor and Merge Tool — Diff gutter indicator model
// ============================================================================
#pragma once

#include "../core/DiffTypes.h"
#include "../core/HistoryTypes.h"

#include <cstdint>
#include <vector>

namespace markamp::core
{
struct DiffResult;
} // namespace markamp::core

namespace markamp::ui
{

/// Gutter indicator type for diff view.
enum class GutterIndicator : uint8_t
{
    Unchanged,
    Added,
    Deleted,
    Modified,
    FillerLine ///< Blank alignment filler in side-by-side view
};

/// A single gutter decoration for one line.
struct GutterDecoration
{
    int line_number{0};
    GutterIndicator indicator{GutterIndicator::Unchanged};
    bool is_highlighted{false}; ///< Current change is highlighted
};

/// Model for diff gutter indicators.
/// Maps line indices to indicator types based on diff results.
class DiffGutterModel
{
public:
    DiffGutterModel() = default;

    /// Build gutter decorations from a diff result.
    void build_from_diff(const core::DiffResult& diff, int total_old_lines, int total_new_lines);

    /// Get decorations for the left (old) side.
    [[nodiscard]] auto left_decorations() const -> const std::vector<GutterDecoration>&
    {
        return left_decorations_;
    }

    /// Get decorations for the right (new) side.
    [[nodiscard]] auto right_decorations() const -> const std::vector<GutterDecoration>&
    {
        return right_decorations_;
    }

    /// Highlight a specific change by index.
    void highlight_change(int change_index);

    /// Clear all highlights.
    void clear_highlights();

    /// Get total number of change regions.
    [[nodiscard]] auto change_count() const -> int
    {
        return change_count_;
    }

    /// Get the line number of a change by index.
    [[nodiscard]] auto change_line(int change_index) const -> int;

private:
    std::vector<GutterDecoration> left_decorations_;
    std::vector<GutterDecoration> right_decorations_;
    std::vector<int> change_start_lines_; ///< Start line of each change region
    int change_count_{0};
};

} // namespace markamp::ui
