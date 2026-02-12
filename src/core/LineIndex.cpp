#include "LineIndex.h"

namespace markamp::core
{

void LineIndex::rebuild(std::string_view content)
{
    newline_offsets_.clear();
    for (std::size_t i = 0; i < content.size(); ++i)
    {
        if (content[i] == '\n')
        {
            newline_offsets_.push_back(i);
        }
    }
}

void LineIndex::on_insert(std::size_t offset,
                          std::size_t length,
                          const std::vector<std::size_t>& new_newline_relative_offsets)
{
    // Shift all existing newline offsets that are >= insertion point
    auto shift_from = std::lower_bound(newline_offsets_.begin(), newline_offsets_.end(), offset);
    for (auto it = shift_from; it != newline_offsets_.end(); ++it)
    {
        *it += length;
    }

    // Insert the new newline positions (converted to absolute offsets)
    for (auto relative_offset : new_newline_relative_offsets)
    {
        auto absolute = offset + relative_offset;
        auto insert_at =
            std::lower_bound(newline_offsets_.begin(), newline_offsets_.end(), absolute);
        newline_offsets_.insert(insert_at, absolute);
    }
}

void LineIndex::on_erase(std::size_t offset, std::size_t count)
{
    auto erase_end = offset + count;

    // Remove newlines that fall within the erased range
    auto remove_begin = std::lower_bound(newline_offsets_.begin(), newline_offsets_.end(), offset);
    auto remove_end = std::lower_bound(newline_offsets_.begin(), newline_offsets_.end(), erase_end);
    auto shift_start = newline_offsets_.erase(remove_begin, remove_end);

    // Shift remaining offsets after the erased range
    for (auto it = shift_start; it != newline_offsets_.end(); ++it)
    {
        *it -= count;
    }
}

auto LineIndex::offset_to_line_col(std::size_t offset) const -> std::pair<std::size_t, std::size_t>
{
    // Find the first newline offset that is >= offset
    auto it = std::upper_bound(newline_offsets_.begin(), newline_offsets_.end(), offset);

    // The line number is the count of newlines before this offset
    auto line = static_cast<std::size_t>(it - newline_offsets_.begin());

    // The start of this line is just after the previous newline (or 0 for line 0)
    auto start = line_start(line);

    return {line, offset - start};
}

auto LineIndex::line_col_to_offset(std::size_t line, std::size_t col) const -> std::size_t
{
    return line_start(line) + col;
}

auto LineIndex::line_count() const noexcept -> std::size_t
{
    return newline_offsets_.size() + 1;
}

auto LineIndex::line_start(std::size_t line) const noexcept -> std::size_t
{
    if (line == 0)
    {
        return 0;
    }
    if (line <= newline_offsets_.size())
    {
        return newline_offsets_[line - 1] + 1;
    }
    // Past last line — return a sentinel
    return newline_offsets_.empty() ? 0 : newline_offsets_.back() + 1;
}

auto LineIndex::newline_count() const noexcept -> std::size_t
{
    return newline_offsets_.size();
}

} // namespace markamp::core
