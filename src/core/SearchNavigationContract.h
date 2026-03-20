/// @file SearchNavigationContract.h
/// @brief P05-T02: Search result navigation contract.
///
/// Defines the search → result → editor reveal pipeline with validation
/// for stale matches and missing files.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;

/// A single search result match.
struct SearchMatch
{
    std::string file_path;
    int line_number{0};
    int column{0};
    int match_length{0};
    std::string context_line;
};

/// Search result state for UI rendering.
enum class SearchResultState
{
    kIdle,
    kSearching,
    kResults,
    kNoResults,
    kError,
};

/// Contract for search request → result → editor reveal.
class SearchNavigationContract
{
public:
    explicit SearchNavigationContract(EventBus& bus);

    /// Submit a search request.
    void search(const std::string& query, bool regex = false, bool case_sensitive = false);

    /// Activate a specific result (opens file at match location).
    void activate_result(const SearchMatch& match);

    /// Get current result state.
    [[nodiscard]] auto state() const -> SearchResultState { return state_; }

    /// Get current results.
    [[nodiscard]] auto results() const -> const std::vector<SearchMatch>& { return results_; }

    /// Get result count.
    [[nodiscard]] auto result_count() const -> int
    {
        return static_cast<int>(results_.size());
    }

    /// Clear results.
    void clear();

private:
    EventBus& event_bus_;
    SearchResultState state_{SearchResultState::kIdle};
    std::vector<SearchMatch> results_;
    std::string current_query_;
};

} // namespace markamp::core
