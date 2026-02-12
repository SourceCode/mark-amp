#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace markamp::rendering
{

/// Per-line x-advance prefix sums for O(log n) hit-testing.
///
/// Pattern implemented: #37 Fast hit-testing (mouse → position)
struct LineAdvanceTable
{
    /// Cumulative x-advances per grapheme cluster within a line.
    /// prefix_sums[i] = x-coordinate of the right edge of cluster i.
    std::vector<int32_t> prefix_sums;

    /// The generation (font_gen) these advances were computed for.
    uint64_t font_gen{0};

    /// Number of grapheme clusters in this line.
    [[nodiscard]] auto cluster_count() const noexcept -> std::size_t
    {
        return prefix_sums.size();
    }

    /// Binary search for the column at the given x-coordinate.
    /// Returns the cluster index (column) closest to `x`.
    [[nodiscard]] auto column_at_x(int32_t x) const noexcept -> std::size_t
    {
        if (prefix_sums.empty() || x <= 0)
        {
            return 0;
        }

        // prefix_sums[i] = right edge of glyph i
        // Column i means cursor is between glyph i-1 and glyph i.
        // We want the column whose left edge is closest to x.
        //
        // Column boundaries: 0, prefix_sums[0], prefix_sums[1], ...
        // So column c has left edge = (c == 0 ? 0 : prefix_sums[c-1]).

        // Find which column's left edge is closest to x.
        // lower_bound finds the first prefix_sum >= x.
        auto iter = std::lower_bound(prefix_sums.begin(), prefix_sums.end(), x);
        auto idx = static_cast<std::size_t>(iter - prefix_sums.begin());

        // idx is the column where prefix_sums[idx] >= x (right edge of glyph idx).
        // The two candidate columns are idx and idx+1.
        // Column idx  has left edge = (idx == 0 ? 0 : prefix_sums[idx-1])
        // Column idx+1 has left edge = prefix_sums[idx]

        // If x is exactly on a boundary, cursor goes AFTER that glyph
        if (iter != prefix_sums.end() && *iter == x)
        {
            return idx + 1;
        }

        // Otherwise pick the nearer boundary
        if (idx > 0)
        {
            auto left_edge = prefix_sums[idx - 1]; // left edge of column idx
            int32_t right_edge = (iter != prefix_sums.end()) ? *iter : prefix_sums.back();
            auto dist_left = x - left_edge;
            auto dist_right = right_edge - x;
            if (dist_left <= dist_right)
            {
                return idx;
            }
        }

        return idx;
    }

    /// Get the x-position for a column index.
    [[nodiscard]] auto x_at_column(std::size_t col) const noexcept -> int32_t
    {
        if (col == 0 || prefix_sums.empty())
        {
            return 0;
        }
        if (col > prefix_sums.size())
        {
            return prefix_sums.back();
        }
        return prefix_sums[col - 1];
    }

    /// Build from individual glyph advances.
    static auto from_advances(const std::vector<int32_t>& advances, uint64_t gen = 0)
        -> LineAdvanceTable
    {
        LineAdvanceTable table;
        table.font_gen = gen;
        table.prefix_sums.reserve(advances.size());
        int32_t cumulative = 0;
        for (auto adv : advances)
        {
            cumulative += adv;
            table.prefix_sums.push_back(cumulative);
        }
        return table;
    }
};

/// O(log n) mouse-to-position hit-testing using per-line x-advance prefix sums.
///
/// Maps (pixel_x, pixel_y) to (line, column) without calling GetTextExtent.
///
/// Pattern implemented: #37 Fast hit-testing (mouse → position)
class HitTestAccelerator
{
public:
    HitTestAccelerator() = default;

    /// Result of a hit test.
    struct HitResult
    {
        std::size_t line{0};
        std::size_t column{0};
    };

    /// Perform a hit test at (x, y) given uniform line heights.
    /// O(1) for line lookup + O(log n) for column via binary search.
    [[nodiscard]] auto hit_test(int32_t x,
                                int32_t y,
                                int32_t line_height,
                                std::size_t first_visible_line,
                                std::size_t total_lines) const noexcept -> HitResult
    {
        // Line: y / line_height + first_visible_line
        std::size_t line = first_visible_line;
        if (line_height > 0 && y >= 0)
        {
            line += static_cast<std::size_t>(y / line_height);
        }
        if (line >= total_lines && total_lines > 0)
        {
            line = total_lines - 1;
        }

        // Column: binary search on prefix sums
        std::size_t col = 0;
        if (line < tables_.size() && !tables_[line].prefix_sums.empty())
        {
            col = tables_[line].column_at_x(x);
        }

        return HitResult{.line = line, .column = col};
    }

    /// Set the advance table for a line.
    void set_table(std::size_t line, LineAdvanceTable table)
    {
        ensure_capacity(line);
        tables_[line] = std::move(table);
    }

    /// Get the advance table for a line (may be empty).
    [[nodiscard]] auto get_table(std::size_t line) const -> const LineAdvanceTable&
    {
        static const LineAdvanceTable empty;
        return (line < tables_.size()) ? tables_[line] : empty;
    }

    /// Invalidate a line's advance table.
    void invalidate_line(std::size_t line)
    {
        if (line < tables_.size())
        {
            tables_[line] = LineAdvanceTable{};
        }
    }

    /// Clear all tables.
    void clear()
    {
        tables_.clear();
    }

    /// Number of cached line tables.
    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return tables_.size();
    }

private:
    std::vector<LineAdvanceTable> tables_;

    void ensure_capacity(std::size_t line)
    {
        if (line >= tables_.size())
        {
            tables_.resize(line + 1);
        }
    }
};

} // namespace markamp::rendering
