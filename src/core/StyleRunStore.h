#pragma once

#include <cstdint>
#include <vector>

namespace markamp::core
{

/// A single style run: a contiguous range of characters sharing the same style.
///
/// Compact 10-byte struct. Stored in per-line vectors for batch rendering.
/// At paint time, iterate style runs and set wxDC state once per run
/// instead of per character.
///
/// Pattern implemented: #25 Precomputed style runs (color/attributes) per line
struct StyleRun
{
    uint32_t start{0};    // byte offset within the line
    uint32_t length{0};   // number of bytes
    uint16_t style_id{0}; // maps to a style (color, font, etc.)

    [[nodiscard]] auto end() const noexcept -> uint32_t
    {
        return start + length;
    }
};

/// Per-line storage of precomputed style runs.
///
/// Updated incrementally from tokenizer deltas (AsyncHighlighter).
/// Merges adjacent runs with the same style_id to minimize DC state changes.
///
/// Pattern implemented: #25 Precomputed style runs (color/attributes) per line
class StyleRunStore
{
public:
    StyleRunStore() = default;

    /// Set the style runs for a specific line, merging adjacent identical styles.
    void update_line(std::size_t line, std::vector<StyleRun> runs)
    {
        merge_adjacent(runs);
        ensure_capacity(line);
        lines_[line] = std::move(runs);
    }

    /// Get the style runs for a specific line.
    [[nodiscard]] auto get_line(std::size_t line) const -> const std::vector<StyleRun>&
    {
        static const std::vector<StyleRun> empty;
        if (line < lines_.size())
        {
            return lines_[line];
        }
        return empty;
    }

    /// Invalidate a range of lines (e.g., after an edit).
    void invalidate_range(std::size_t start_line, std::size_t end_line)
    {
        for (auto line = start_line; line < end_line && line < lines_.size(); ++line)
        {
            lines_[line].clear();
        }
    }

    /// Invalidate all cached runs.
    void invalidate_all()
    {
        lines_.clear();
    }

    /// Number of lines with cached runs.
    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return lines_.size();
    }

    /// Check if a line has cached runs.
    [[nodiscard]] auto has_runs(std::size_t line) const noexcept -> bool
    {
        return line < lines_.size() && !lines_[line].empty();
    }

    /// Total number of style runs across all lines.
    [[nodiscard]] auto total_run_count() const noexcept -> std::size_t
    {
        std::size_t count = 0;
        for (const auto& line_runs : lines_)
        {
            count += line_runs.size();
        }
        return count;
    }

private:
    std::vector<std::vector<StyleRun>> lines_;

    void ensure_capacity(std::size_t line)
    {
        if (line >= lines_.size())
        {
            lines_.resize(line + 1);
        }
    }

    /// Merge adjacent runs with the same style_id.
    static void merge_adjacent(std::vector<StyleRun>& runs)
    {
        if (runs.size() <= 1)
        {
            return;
        }

        std::vector<StyleRun> merged;
        merged.reserve(runs.size());
        merged.push_back(runs[0]);

        for (std::size_t idx = 1; idx < runs.size(); ++idx)
        {
            auto& last = merged.back();
            if (runs[idx].style_id == last.style_id && runs[idx].start == last.end())
            {
                // Merge: extend the previous run
                last.length += runs[idx].length;
            }
            else
            {
                merged.push_back(runs[idx]);
            }
        }

        runs = std::move(merged);
    }
};

} // namespace markamp::core
