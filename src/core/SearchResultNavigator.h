#pragma once

/// @file SearchResultNavigator.h
/// @brief V9 Phase 10 – Search result navigation with visited tracking.
///
/// Header-only implementation of result set navigation:
///   - Next/previous with wrap-around
///   - Go-to by index
///   - Visited/unvisited tracking
///   - Post-search result filtering by predicate

#include "Search.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>
#include <set>
#include <vector>

namespace markamp::core
{

// ────────────────────────────────────────────────────────────
// SearchResultNavigator
// ────────────────────────────────────────────────────────────

/// Navigates through a set of search results with tracking.
class SearchResultNavigator
{
public:
    SearchResultNavigator() = default;

    /// Set the result set to navigate.
    void set_results(const SearchResult& results)
    {
        results_ = results;
        current_index_ = results.hits.empty() ? -1 : 0;
        visited_.clear();

        // Auto-mark first result as visited
        if (!results.hits.empty())
        {
            visited_.insert(0);
        }
    }

    /// Get the current search hit, or nullptr if no results.
    [[nodiscard]] auto current() const -> const SearchHit*
    {
        if (current_index_ < 0 || current_index_ >= static_cast<int>(results_.hits.size()))
        {
            return nullptr;
        }
        return &results_.hits[static_cast<std::size_t>(current_index_)];
    }

    /// Get the current index (-1 if no results).
    [[nodiscard]] auto current_index() const noexcept -> int
    {
        return current_index_;
    }

    /// Get total number of results.
    [[nodiscard]] auto total() const noexcept -> int
    {
        return static_cast<int>(results_.hits.size());
    }

    /// Check if there is a next result (with wrap-around, always true if results exist).
    [[nodiscard]] auto has_next() const noexcept -> bool
    {
        return !results_.hits.empty();
    }

    /// Check if there is a previous result (with wrap-around, always true if results exist).
    [[nodiscard]] auto has_previous() const noexcept -> bool
    {
        return !results_.hits.empty();
    }

    /// Navigate to the next result. Wraps around to first.
    /// Returns the new current hit, or nullptr if no results.
    auto next() -> const SearchHit*
    {
        if (results_.hits.empty())
        {
            return nullptr;
        }

        current_index_++;
        if (current_index_ >= static_cast<int>(results_.hits.size()))
        {
            current_index_ = 0; // Wrap around
        }

        visited_.insert(current_index_);
        return &results_.hits[static_cast<std::size_t>(current_index_)];
    }

    /// Navigate to the previous result. Wraps around to last.
    /// Returns the new current hit, or nullptr if no results.
    auto previous() -> const SearchHit*
    {
        if (results_.hits.empty())
        {
            return nullptr;
        }

        current_index_--;
        if (current_index_ < 0)
        {
            current_index_ = static_cast<int>(results_.hits.size()) - 1; // Wrap
        }

        visited_.insert(current_index_);
        return &results_.hits[static_cast<std::size_t>(current_index_)];
    }

    /// Navigate to a specific result by index.
    /// Returns the hit, or nullptr if index is out of range.
    auto go_to(int index) -> const SearchHit*
    {
        if (index < 0 || index >= static_cast<int>(results_.hits.size()))
        {
            return nullptr;
        }

        current_index_ = index;
        visited_.insert(current_index_);
        return &results_.hits[static_cast<std::size_t>(current_index_)];
    }

    // ── Visited Tracking ─────────────────────────────────────

    /// Mark an index as visited.
    void mark_visited(int index)
    {
        if (index >= 0 && index < static_cast<int>(results_.hits.size()))
        {
            visited_.insert(index);
        }
    }

    /// Check if an index has been visited.
    [[nodiscard]] auto is_visited(int index) const -> bool
    {
        return visited_.count(index) > 0;
    }

    /// Get the set of all visited indices.
    [[nodiscard]] auto get_visited_indices() const -> const std::set<int>&
    {
        return visited_;
    }

    /// Get the number of visited results.
    [[nodiscard]] auto visited_count() const noexcept -> std::size_t
    {
        return visited_.size();
    }

    // ── Filtering ────────────────────────────────────────────

    /// Filter results by a predicate, returning a new SearchResult.
    /// Does not modify the current navigator state.
    [[nodiscard]] auto filter_results(const std::function<bool(const SearchHit&)>& predicate) const
        -> SearchResult
    {
        SearchResult filtered;
        filtered.query = results_.query;
        filtered.page = results_.page;
        filtered.page_size = results_.page_size;

        for (const auto& hit : results_.hits)
        {
            if (predicate(hit))
            {
                filtered.hits.push_back(hit);
            }
        }

        filtered.total_count = static_cast<int>(filtered.hits.size());
        return filtered;
    }

    /// Filter by minimum score threshold.
    [[nodiscard]] auto filter_by_score(double min_score) const -> SearchResult
    {
        return filter_results([min_score](const SearchHit& hit) { return hit.score >= min_score; });
    }

    /// Filter by document path prefix.
    [[nodiscard]] auto filter_by_path(const std::string& path_prefix) const -> SearchResult
    {
        return filter_results(
            [&path_prefix](const SearchHit& hit) {
                return hit.block_path.find(path_prefix) == 0 || hit.root_id.find(path_prefix) == 0;
            });
    }

    /// Filter by block type.
    [[nodiscard]] auto filter_by_type(SearchBlockType block_type) const -> SearchResult
    {
        return filter_results([block_type](const SearchHit& hit)
                              { return hit.block_type == block_type; });
    }

    /// Get the underlying results.
    [[nodiscard]] auto results() const -> const SearchResult&
    {
        return results_;
    }

private:
    SearchResult results_;
    int current_index_{-1};
    std::set<int> visited_;
};

} // namespace markamp::core
