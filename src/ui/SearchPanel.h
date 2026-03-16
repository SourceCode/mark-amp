#pragma once

#include "../core/BlockRef.h"
#include "../core/EventBus.h"
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

// ============================================================================
// SearchPanelController — coordinates search UI with backend
// ============================================================================

namespace markamp::core
{

/// Controller that bridges search UI panels to the WorkspaceSearchEngine.
/// Manages search state, publishes events, and maintains search history.
class SearchPanelController
{
public:
    explicit SearchPanelController(EventBus& event_bus);

    /// Execute a search with the given query and filter state.
    auto execute_search(const std::string& query, const SearchFilterState& filter) -> void;

    /// Get the current query and filter state.
    [[nodiscard]] auto current_query() const -> const std::string&;
    [[nodiscard]] auto current_filter() const -> const SearchFilterState&;

    /// Get the search history.
    [[nodiscard]] auto search_history() const -> const std::vector<SearchHistoryEntry>&;

    /// Clear search history.
    auto clear_history() -> void;

    /// Get the last search result count.
    [[nodiscard]] auto last_result_count() const -> int;

    /// Get the last search elapsed time in ms.
    [[nodiscard]] auto last_elapsed_ms() const -> double;

private:
    EventBus& event_bus_;
    Subscription search_completed_sub_;

    std::string current_query_;
    SearchFilterState current_filter_;
    std::vector<SearchHistoryEntry> search_history_;
    int last_result_count_{0};
    double last_elapsed_ms_{0.0};
};

} // namespace markamp::core
