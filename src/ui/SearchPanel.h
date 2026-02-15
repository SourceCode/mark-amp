#pragma once

#include "../core/BlockRef.h"
#include "../core/Search.h"

#include <chrono>
#include <string>
#include <vector>

namespace markamp::core
{

/// State of search filters in the UI panel.
struct SearchFilterState
{
    /// Active search method.
    SearchMethod method{SearchMethod::Keyword};

    /// Selected block types (empty = all).
    std::vector<SearchBlockType> types;

    /// Selected notebooks.
    std::vector<std::string> boxes;

    /// Path filter text.
    std::string path_filter;

    /// Sort order.
    SearchSortOrder sort_order{SearchSortOrder::Relevance};

    /// Group by document.
    SearchGroupBy group_by{SearchGroupBy::NoGroup};

    /// Case sensitive toggle.
    bool case_sensitive{false};
};

/// A rendered search result item for display in the panel.
struct SearchResultItem
{
    /// Block ID for navigation.
    BlockId block_id;

    /// Title line (heading or truncated first line).
    std::string title;

    /// Content snippet with highlighted matches.
    std::string snippet_html;

    /// Document path for breadcrumb display.
    std::string doc_path;

    /// Block type icon name.
    std::string type_icon;

    /// Whether this result is currently selected.
    bool is_selected{false};

    /// Whether this result has been visited / navigated to.
    bool is_visited{false};
};

/// Result of a search-and-replace operation.
struct ReplaceResult
{
    /// Number of replacements made.
    int replaced_count{0};

    /// Number of blocks modified.
    int blocks_modified{0};

    /// Whether the operation was cancelled.
    bool cancelled{false};

    /// Error message if the operation failed.
    std::string error;
};

/// A search history entry.
struct SearchHistoryEntry
{
    /// The search query text.
    std::string query;

    /// The search method used.
    SearchMethod method{SearchMethod::Keyword};

    /// Number of results returned.
    int result_count{0};

    /// When the search was executed.
    std::chrono::system_clock::time_point timestamp;
};

} // namespace markamp::core

// ════════════════════════════════════════════════════════════
// SearchPanel wxWidgets class is intentionally deferred.
// The full UI implementation will be in a future UI-focused batch.
// ════════════════════════════════════════════════════════════
