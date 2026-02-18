/// @file BookmarkNavigator.h
/// @brief V9 Phase 37 — Sequential bookmark navigation with filtering.
#pragma once

#include "Bookmark.h"

#include <cstddef>
#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// Sort order for bookmark navigation.
enum class BookmarkSortOrder
{
    kCreationTime, // Oldest first
    kLabel,        // Alphabetical by label
    kDocument,     // Grouped by document
    kBlockType     // Grouped by block type
};

/// Summary statistics for bookmarks.
struct BookmarkSummary
{
    int total_bookmarks{0};
    int total_labels{0};
    int total_documents{0};
    std::vector<std::pair<std::string, int>> per_label_counts;    // label → count
    std::vector<std::pair<std::string, int>> per_document_counts; // doc → count
};

/// Sequential navigator through bookmarked blocks.
///
/// Provides next/previous navigation with wrap-around, filtering by label,
/// sort order control, and visit tracking.
class BookmarkNavigator
{
public:
    BookmarkNavigator() = default;

    /// Load bookmark entries for navigation.
    void load(std::vector<BookmarkEntry> entries);

    /// Move to the next bookmark (wraps around). Returns the entry.
    auto next() -> const BookmarkEntry*;

    /// Move to the previous bookmark (wraps around). Returns the entry.
    auto previous() -> const BookmarkEntry*;

    /// Jump to a specific index. Returns nullptr if out of range.
    auto go_to(std::size_t index) -> const BookmarkEntry*;

    /// Get the current bookmark entry (nullptr if empty).
    [[nodiscard]] auto current() const -> const BookmarkEntry*;

    /// Get the current index.
    [[nodiscard]] auto current_index() const -> std::size_t;

    /// Get total number of navigable entries.
    [[nodiscard]] auto count() const -> std::size_t;

    /// Filter navigation to a specific label (empty = show all).
    void filter_by_label(const std::string& label);

    /// Get the current filter label (empty if no filter).
    [[nodiscard]] auto filter_label() const -> std::string;

    /// Set the sort order for navigation.
    void set_sort_order(BookmarkSortOrder order);

    /// Get the current sort order.
    [[nodiscard]] auto sort_order() const -> BookmarkSortOrder;

    /// Mark the current bookmark as visited.
    void mark_visited();

    /// Check if a bookmark at the given index has been visited.
    [[nodiscard]] auto is_visited(std::size_t index) const -> bool;

    /// Get indices of all visited bookmarks.
    [[nodiscard]] auto visited_indices() const -> std::vector<std::size_t>;

    /// Get count of visited bookmarks.
    [[nodiscard]] auto visited_count() const -> std::size_t;

    /// Clear visited tracking.
    void clear_visited();

    /// Check if navigation is at the boundary.
    [[nodiscard]] auto has_next() const -> bool;
    [[nodiscard]] auto has_previous() const -> bool;

    /// Generate summary statistics.
    [[nodiscard]] auto summary() const -> BookmarkSummary;

private:
    std::vector<BookmarkEntry> all_entries_;
    std::vector<BookmarkEntry> filtered_entries_;
    std::size_t current_index_{0};
    std::string filter_label_;
    BookmarkSortOrder sort_order_{BookmarkSortOrder::kCreationTime};
    std::unordered_set<std::size_t> visited_;

    /// Rebuild filtered_entries_ from all_entries_ based on filter and sort.
    void rebuild_filtered();
    /// Apply sort to filtered entries.
    void apply_sort();
};

} // namespace markamp::core
