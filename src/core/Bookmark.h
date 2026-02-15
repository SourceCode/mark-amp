#pragma once

#include "BlockRef.h"

#include <chrono>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// A single bookmark entry associating a block with a label.
struct BookmarkEntry
{
    /// The bookmarked block.
    BlockId block_id;

    /// The bookmark label (group name).
    std::string label;

    /// The root document containing this block.
    std::string root_id;

    /// Document title for display.
    std::string doc_title;

    /// Block type (heading, paragraph, etc.) for icon display.
    std::string block_type;

    /// Content snippet of the bookmarked block (first 100 chars).
    std::string content_snippet;

    /// When the bookmark was created.
    std::chrono::system_clock::time_point created_at;
};

/// A group of bookmarks sharing the same label.
struct BookmarkGroup
{
    /// The bookmark label (group name).
    std::string label;

    /// All blocks bookmarked with this label.
    std::vector<BookmarkEntry> entries;

    /// Number of blocks in this group.
    [[nodiscard]] auto count() const -> std::size_t
    {
        return entries.size();
    }

    /// Whether the group is empty.
    [[nodiscard]] auto is_empty() const -> bool
    {
        return entries.empty();
    }
};

/// In-memory bookmark index for fast lookups.
class BookmarkIndex
{
public:
    /// Add a bookmark.
    void add(const BlockId& block_id, const std::string& label);

    /// Remove a bookmark from a specific block.
    void remove(const BlockId& block_id);

    /// Check if a block is bookmarked.
    [[nodiscard]] auto is_bookmarked(const BlockId& block_id) const -> bool;

    /// Get the bookmark label for a block (empty if not bookmarked).
    [[nodiscard]] auto get_label(const BlockId& block_id) const -> std::string;

    /// Get all blocks with a specific label.
    [[nodiscard]] auto get_blocks_with_label(const std::string& label) const
        -> std::vector<BlockId>;

    /// Get all unique bookmark labels.
    [[nodiscard]] auto get_all_labels() const -> std::vector<std::string>;

    /// Rename a label across all bookmarks. Returns affected count.
    [[nodiscard]] auto rename_label(const std::string& old_label, const std::string& new_label)
        -> int;

    /// Remove all bookmarks with a specific label. Returns removed count.
    [[nodiscard]] auto remove_label(const std::string& label) -> int;

    /// Get total number of bookmarks.
    [[nodiscard]] auto total_count() const -> std::size_t;

    /// Clear the entire index.
    void clear();

private:
    /// block_id -> label
    std::unordered_map<BlockId, std::string, BlockIdHash> block_to_label_;

    /// label -> set of block IDs
    std::unordered_map<std::string, std::unordered_set<BlockId, BlockIdHash>> label_to_blocks_;
};

} // namespace markamp::core
