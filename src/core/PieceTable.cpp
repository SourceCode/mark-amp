#include "PieceTable.h"

#include <algorithm>
#include <stdexcept>

namespace markamp::core
{

PieceTable::PieceTable(std::string content)
    : original_buffer_(std::move(content))
    , total_size_(original_buffer_.size())
{
    if (!original_buffer_.empty())
    {
        pieces_.push_back(Piece{BufferSource::Original, 0, original_buffer_.size()});
    }
}

void PieceTable::insert(std::size_t offset, std::string_view text)
{
    if (text.empty())
    {
        return;
    }

    // Record where in the append buffer this text starts
    auto append_offset = append_buffer_.size();
    append_buffer_.append(text);

    Piece new_piece{BufferSource::Append, append_offset, text.size()};

    if (pieces_.empty() || offset >= total_size_)
    {
        // Append at end
        pieces_.push_back(new_piece);
        total_size_ += text.size();
        return;
    }

    auto [piece_idx, inner_offset] = find_piece(offset);

    if (inner_offset == 0)
    {
        // Insert before the piece — no splitting needed
        pieces_.insert(pieces_.begin() + static_cast<std::ptrdiff_t>(piece_idx), new_piece);
    }
    else
    {
        // Split the existing piece at inner_offset
        auto& existing = pieces_[piece_idx];
        Piece left{existing.source, existing.offset, inner_offset};
        Piece right{
            existing.source, existing.offset + inner_offset, existing.length - inner_offset};

        // Replace existing piece with [left, new_piece, right]
        pieces_[piece_idx] = left;
        auto insert_pos = pieces_.begin() + static_cast<std::ptrdiff_t>(piece_idx) + 1;
        pieces_.insert(insert_pos, {new_piece, right});
    }

    total_size_ += text.size();
}

void PieceTable::erase(std::size_t offset, std::size_t count)
{
    if (count == 0 || pieces_.empty())
    {
        return;
    }

    // Clamp to actual content
    if (offset >= total_size_)
    {
        return;
    }
    count = std::min(count, total_size_ - offset);

    auto end_offset = offset + count;

    // Find the piece containing the start of the deletion
    auto [start_idx, start_inner] = find_piece(offset);

    // Find the piece containing the end of the deletion
    auto [end_idx, end_inner] = find_piece(end_offset);

    // Build replacement pieces for the partially affected boundaries
    std::vector<Piece> replacements;

    // Left remnant of start piece (if deletion doesn't start at piece boundary)
    if (start_inner > 0)
    {
        auto& start_piece = pieces_[start_idx];
        replacements.push_back(Piece{start_piece.source, start_piece.offset, start_inner});
    }

    // Right remnant of end piece (if deletion doesn't end at piece boundary)
    if (end_idx < pieces_.size() && end_inner > 0)
    {
        auto& end_piece = pieces_[end_idx];
        if (end_inner < end_piece.length)
        {
            replacements.push_back(Piece{
                end_piece.source, end_piece.offset + end_inner, end_piece.length - end_inner});
        }
        ++end_idx; // This piece is consumed
    }

    // Determine the range to erase: from start_idx to end_idx (exclusive)
    auto erase_begin = pieces_.begin() + static_cast<std::ptrdiff_t>(start_idx);
    auto erase_end =
        pieces_.begin() + static_cast<std::ptrdiff_t>(std::min(end_idx, pieces_.size()));

    // Replace the affected range with the remnants
    pieces_.erase(erase_begin, erase_end);
    pieces_.insert(pieces_.begin() + static_cast<std::ptrdiff_t>(start_idx),
                   replacements.begin(),
                   replacements.end());

    total_size_ -= count;
}

auto PieceTable::text() const -> std::string
{
    std::string result;
    result.reserve(total_size_);
    for (const auto& piece : pieces_)
    {
        const auto& buf = buffer_for(piece.source);
        result.append(buf, piece.offset, piece.length);
    }
    return result;
}

auto PieceTable::substr(std::size_t offset, std::size_t count) const -> std::string
{
    if (offset >= total_size_)
    {
        return {};
    }
    count = std::min(count, total_size_ - offset);
    if (count == 0)
    {
        return {};
    }

    std::string result;
    result.reserve(count);

    auto [piece_idx, inner_offset] = find_piece(offset);
    std::size_t remaining = count;

    for (auto i = piece_idx; i < pieces_.size() && remaining > 0; ++i)
    {
        const auto& piece = pieces_[i];
        const auto& buf = buffer_for(piece.source);

        auto start = (i == piece_idx) ? inner_offset : std::size_t{0};
        auto available = piece.length - start;
        auto to_copy = std::min(available, remaining);

        result.append(buf, piece.offset + start, to_copy);
        remaining -= to_copy;
    }

    return result;
}

auto PieceTable::at(std::size_t offset) const -> char
{
    if (offset >= total_size_)
    {
        throw std::out_of_range("PieceTable::at: offset out of range");
    }

    auto [piece_idx, inner_offset] = find_piece(offset);
    const auto& piece = pieces_[piece_idx];
    const auto& buf = buffer_for(piece.source);
    return buf[piece.offset + inner_offset];
}

auto PieceTable::size() const noexcept -> std::size_t
{
    return total_size_;
}

auto PieceTable::empty() const noexcept -> bool
{
    return total_size_ == 0;
}

auto PieceTable::piece_count() const noexcept -> std::size_t
{
    return pieces_.size();
}

auto PieceTable::find_piece(std::size_t offset) const -> std::pair<std::size_t, std::size_t>
{
    std::size_t pos = 0;
    for (std::size_t i = 0; i < pieces_.size(); ++i)
    {
        if (offset < pos + pieces_[i].length)
        {
            return {i, offset - pos};
        }
        pos += pieces_[i].length;
    }
    // offset == total_size_ — points past-the-end
    return {pieces_.size(), 0};
}

auto PieceTable::buffer_for(BufferSource source) const -> const std::string&
{
    return (source == BufferSource::Original) ? original_buffer_ : append_buffer_;
}

void PieceTable::recalculate_size() noexcept
{
    total_size_ = 0;
    for (const auto& piece : pieces_)
    {
        total_size_ += piece.length;
    }
}

} // namespace markamp::core
