#pragma once

#include "DirtyRegion.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace markamp::rendering
{

/// Selection boundary expressed as line intervals.
///
/// Pattern implemented: #35 Efficient selection/caret painting
struct SelectionInterval
{
    std::size_t start_line{0};
    std::size_t end_line{0};
    uint32_t start_col{0};
    uint32_t end_col{0};

    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return start_line == end_line && start_col == end_col;
    }

    auto operator==(const SelectionInterval& other) const noexcept -> bool = default;
    auto operator!=(const SelectionInterval& other) const noexcept -> bool = default;
};

/// Efficient selection painting via interval invalidation.
///
/// Selection deltas are computed between old and new selection states,
/// and only the changed lines are invalidated — not the entire selection range.
///
/// Pattern implemented: #35 Efficient selection/caret painting
class SelectionPainter
{
public:
    SelectionPainter() = default;

    /// Compute the lines that changed between old and new selection.
    /// Returns only the lines that entered or left the selection.
    [[nodiscard]] static auto compute_changed_lines(const SelectionInterval& old_sel,
                                                    const SelectionInterval& new_sel)
        -> std::vector<std::size_t>
    {
        std::vector<std::size_t> changed;

        if (old_sel == new_sel)
        {
            return changed;
        }

        // Both empty — nothing changed
        if (old_sel.is_empty() && new_sel.is_empty())
        {
            return changed;
        }

        // One empty, other not — invalidate the non-empty range
        if (old_sel.is_empty())
        {
            for (auto line = new_sel.start_line; line <= new_sel.end_line; ++line)
            {
                changed.push_back(line);
            }
            return changed;
        }
        if (new_sel.is_empty())
        {
            for (auto line = old_sel.start_line; line <= old_sel.end_line; ++line)
            {
                changed.push_back(line);
            }
            return changed;
        }

        // Both non-empty — find symmetric difference of line ranges
        auto min_start = std::min(old_sel.start_line, new_sel.start_line);
        auto max_start = std::max(old_sel.start_line, new_sel.start_line);
        auto min_end = std::min(old_sel.end_line, new_sel.end_line);
        auto max_end = std::max(old_sel.end_line, new_sel.end_line);

        // Lines between min_start and max_start are changed
        for (auto line = min_start; line < max_start; ++line)
        {
            changed.push_back(line);
        }
        // Lines between min_end and max_end are changed
        for (auto line = min_end + 1; line <= max_end; ++line)
        {
            changed.push_back(line);
        }

        // Boundary lines may have column changes
        if (old_sel.start_line == new_sel.start_line && old_sel.start_col != new_sel.start_col)
        {
            if (std::find(changed.begin(), changed.end(), old_sel.start_line) == changed.end())
            {
                changed.push_back(old_sel.start_line);
            }
        }
        if (old_sel.end_line == new_sel.end_line && old_sel.end_col != new_sel.end_col)
        {
            if (std::find(changed.begin(), changed.end(), old_sel.end_line) == changed.end())
            {
                changed.push_back(old_sel.end_line);
            }
        }

        return changed;
    }

    /// Compute dirty rects for the changed lines given line heights.
    /// Returns Rects in {left, top, right, bottom} format.
    [[nodiscard]] static auto compute_dirty_rects(const SelectionInterval& old_sel,
                                                  const SelectionInterval& new_sel,
                                                  int32_t line_height,
                                                  int32_t viewport_width) -> std::vector<Rect>
    {
        auto changed_lines = compute_changed_lines(old_sel, new_sel);
        std::vector<Rect> rects;
        rects.reserve(changed_lines.size());

        for (auto line : changed_lines)
        {
            auto y_top = static_cast<int32_t>(line) * line_height;
            rects.push_back(Rect{0,                     // left
                                 y_top,                 // top
                                 viewport_width,        // right
                                 y_top + line_height}); // bottom
        }

        return rects;
    }
};

} // namespace markamp::rendering
