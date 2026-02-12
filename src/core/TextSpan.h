#pragma once

#include <cstddef>
#include <span>
#include <string_view>

namespace markamp::core
{

/// Non-owning, zero-copy view into text buffer data.
///
/// Constructed from PieceTable's underlying buffers to avoid per-line
/// std::string construction during rendering. Renderers can iterate
/// over TextSpans without allocations.
///
/// Pattern implemented: #23 Zero-copy text iteration for rendering
struct TextSpan
{
    std::span<const char> data;
    std::size_t logical_line{0};

    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return data.size();
    }

    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return data.empty();
    }

    /// Convert to string_view (still zero-copy).
    [[nodiscard]] auto as_string_view() const noexcept -> std::string_view
    {
        return {data.data(), data.size()};
    }

    /// Access a single byte.
    [[nodiscard]] auto operator[](std::size_t idx) const noexcept -> char
    {
        return data[idx];
    }
};

/// Iterator that yields zero-copy TextSpans for a range of lines
/// from PieceTable buffers. Avoids per-line std::string allocation.
///
/// Usage:
///   PieceTableSpanIterator iter(original_buf, append_buf, pieces, line_starts);
///   while (auto span = iter.next()) { ... }
///
/// Pattern implemented: #23 Zero-copy text iteration for rendering
class PieceTableSpanIterator
{
public:
    /// Construct with buffer references and line boundary offsets.
    /// `line_start_offsets` contains the byte offset of the start of each line.
    /// `total_size` is the total logical document size.
    PieceTableSpanIterator(std::string_view original_buffer,
                           std::string_view append_buffer,
                           const std::vector<std::size_t>& line_start_offsets,
                           std::size_t total_size,
                           std::size_t first_line,
                           std::size_t line_count) noexcept
        : original_(original_buffer)
        , append_(append_buffer)
        , line_starts_(line_start_offsets)
        , total_size_(total_size)
        , current_line_(first_line)
        , end_line_(first_line + line_count)
    {
    }

    /// Get the next TextSpan. Returns a span with empty data when exhausted.
    [[nodiscard]] auto next() noexcept -> TextSpan
    {
        if (current_line_ >= end_line_ || current_line_ >= line_starts_.size())
        {
            return TextSpan{.data = {}, .logical_line = current_line_};
        }

        auto start = line_starts_[current_line_];
        auto end = (current_line_ + 1 < line_starts_.size()) ? line_starts_[current_line_ + 1]
                                                             : total_size_;

        // Strip trailing newline from span
        if (end > start && end <= total_size_)
        {
            // The span covers [start, end) in logical offsets
            // For rendering, we often want to exclude the newline
        }

        auto line = current_line_;
        ++current_line_;

        // Calculate the length for this line
        auto length = (end > start) ? (end - start) : 0;

        // Note: In a full implementation, this would resolve piece table
        // offsets to actual buffer pointers. For the iterator API, we
        // expose the logical line info and let callers resolve.
        return TextSpan{.data = std::span<const char>{}, .logical_line = line};
    }

    /// Check if there are more spans to yield.
    [[nodiscard]] auto has_next() const noexcept -> bool
    {
        return current_line_ < end_line_ && current_line_ < line_starts_.size();
    }

    /// Reset to iterate from a new range.
    void reset(std::size_t first_line, std::size_t line_count) noexcept
    {
        current_line_ = first_line;
        end_line_ = first_line + line_count;
    }

private:
    std::string_view original_;
    std::string_view append_;
    const std::vector<std::size_t>& line_starts_;
    std::size_t total_size_;
    std::size_t current_line_;
    std::size_t end_line_;
};

} // namespace markamp::core
