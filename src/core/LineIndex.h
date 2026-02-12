#pragma once

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

namespace markamp::core
{

/// Cached line-break index for fast O(log n) offset ↔ (line, col) conversion.
///
/// Maintains a sorted vector of newline byte offsets. Updated incrementally
/// on each insert/erase so that offset_to_line_col() and line_col_to_offset()
/// are always O(log n) via binary search.
///
/// Patterns implemented:
///   #4  Cached line index and fast (row, col) mapping
///   #11 O(1)/O(log n) "typing path" guarantee
class LineIndex
{
public:
    LineIndex() = default;

    /// Build the index from scratch for the given content.
    void rebuild(std::string_view content);

    /// Incrementally update after an insertion at `offset` of `length` bytes.
    /// `new_newline_relative_offsets` contains positions of '\n' within the
    /// inserted text, relative to the start of the inserted text (0-based).
    void on_insert(std::size_t offset,
                   std::size_t length,
                   const std::vector<std::size_t>& new_newline_relative_offsets);

    /// Incrementally update after erasing `count` bytes starting at `offset`.
    void on_erase(std::size_t offset, std::size_t count);

    /// Convert a byte offset to a (line, column) pair. Both are 0-indexed.
    [[nodiscard]] auto offset_to_line_col(std::size_t offset) const
        -> std::pair<std::size_t, std::size_t>;

    /// Convert a (line, column) pair to a byte offset.
    [[nodiscard]] auto line_col_to_offset(std::size_t line, std::size_t col) const -> std::size_t;

    /// Total number of lines (newline_count + 1).
    [[nodiscard]] auto line_count() const noexcept -> std::size_t;

    /// Get the byte offset of the start of a given line (0-indexed).
    [[nodiscard]] auto line_start(std::size_t line) const noexcept -> std::size_t;

    /// Number of indexed newlines.
    [[nodiscard]] auto newline_count() const noexcept -> std::size_t;

private:
    /// Sorted vector of byte offsets of '\n' characters.
    std::vector<std::size_t> newline_offsets_;
};

} // namespace markamp::core
