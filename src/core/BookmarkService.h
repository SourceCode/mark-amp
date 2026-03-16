#pragma once

#include "Bookmark.h"
#include "EventBus.h"

#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace markamp::core
{

class Config;

/// Service for managing block bookmarks.
/// Bookmarks are stored as IAL attributes and indexed for fast retrieval.
class BookmarkService
{
public:
    BookmarkService(EventBus& event_bus, Config& config);

    /// Return all bookmarks grouped by label, sorted alphabetically.
    [[nodiscard]] auto get_bookmarks() -> std::vector<BookmarkGroup>;

    /// Return all bookmark entries for a specific label.
    [[nodiscard]] auto get_bookmark_blocks(const std::string& label) -> std::vector<BookmarkEntry>;

    /// Set a bookmark on a block with the given label.
    void set_bookmark(const BlockId& block_id, const std::string& label);

    /// Remove the bookmark from a block.
    void remove_bookmark(const BlockId& block_id);

    /// Rename a bookmark label across all blocks.
    void rename_bookmark(const std::string& old_label, const std::string& new_label);

    /// Search bookmark labels and content snippets.
    [[nodiscard]] auto search_bookmarks(const std::string& query) -> std::vector<BookmarkGroup>;

    /// Return all unique bookmark labels with their block counts.
    [[nodiscard]] auto get_bookmark_labels() -> std::vector<std::pair<std::string, int>>;

    /// Full rebuild of BookmarkIndex from block database.
    void rebuild_index();

    /// (#111) Return the total number of bookmarks across all labels.
    [[nodiscard]] auto bookmark_count() -> std::size_t;

    /// (#112) Return the number of unique bookmark labels.
    [[nodiscard]] auto label_count() -> std::size_t;

private:
    EventBus& event_bus_;
    Config& config_;
    BookmarkIndex index_;
    mutable std::mutex index_mutex_;
};

} // namespace markamp::core
