#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace markamp::core
{

/// Which underlying buffer a piece refers to.
enum class BufferSource : uint8_t
{
    Original, // The initial file content (immutable after construction)
    Append    // Newly inserted text (append-only)
};

/// A single piece: a view into one of the two immutable buffers.
struct Piece
{
    BufferSource source{BufferSource::Original};
    std::size_t offset{0}; // byte offset within the source buffer
    std::size_t length{0}; // number of bytes
};

/// A piece table for efficient text editing.
///
/// The text is represented as a sequence of "pieces", each
/// referencing a contiguous slice of either the original file content
/// or the append buffer. Edits (insert/delete) only modify the piece
/// sequence — the underlying buffers are immutable after creation.
///
/// Complexity:
///   insert() — O(pieces) for find + O(pieces) for vector shift
///   erase()  — O(pieces)
///   text()   — O(total_bytes)
///   substr() — O(pieces + requested_bytes)
///   size()   — O(pieces)  (cached internally)
///
/// For very large files with many edits, replace the std::vector<Piece>
/// with a balanced tree (B-tree) for O(log n) indexing.
///
/// Pattern implemented: #3 Rope/piece-table text buffer
class PieceTable
{
public:
    /// Construct from initial file content.
    explicit PieceTable(std::string content = "");

    /// Insert text at the given logical byte offset.
    void insert(std::size_t offset, std::string_view text);

    /// Delete `count` bytes starting at logical byte offset.
    void erase(std::size_t offset, std::size_t count);

    /// Retrieve the full text content. O(n) in total bytes.
    [[nodiscard]] auto text() const -> std::string;

    /// Retrieve a substring from the logical buffer.
    [[nodiscard]] auto substr(std::size_t offset, std::size_t count) const -> std::string;

    /// Character at a logical offset. O(pieces).
    [[nodiscard]] auto at(std::size_t offset) const -> char;

    /// Total logical length in bytes.
    [[nodiscard]] auto size() const noexcept -> std::size_t;

    /// Whether the buffer is empty.
    [[nodiscard]] auto empty() const noexcept -> bool;

    /// Number of pieces in the piece sequence (diagnostic).
    [[nodiscard]] auto piece_count() const noexcept -> std::size_t;

private:
    std::string original_buffer_;
    std::string append_buffer_;
    std::vector<Piece> pieces_;
    std::size_t total_size_{0};

    /// Find which piece contains the given logical offset.
    /// Returns {piece_index, offset_within_piece}.
    [[nodiscard]] auto find_piece(std::size_t offset) const -> std::pair<std::size_t, std::size_t>;

    /// Get a reference to the buffer for a given source.
    [[nodiscard]] auto buffer_for(BufferSource source) const -> const std::string&;

    /// Recalculate total_size_ from pieces.
    void recalculate_size() noexcept;
};

} // namespace markamp::core
