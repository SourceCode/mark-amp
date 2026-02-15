#pragma once

#include "canvas/CanvasSearch.h"
#include "canvas/CanvasTypes.h"

#include <functional>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// UI search bar for navigating canvas search results.
class SearchBar
{
public:
    using OnNavigateToResult = std::function<void(ObjectId, const AABB& bounds)>;

    // ── Query ──────────────────────────────────────────────────────
    auto set_query(const std::string& query) -> void;
    [[nodiscard]] auto query() const -> const std::string&;

    // ── Results ────────────────────────────────────────────────────
    auto set_results(const std::vector<SearchResult>& results) -> void;
    [[nodiscard]] auto result_count() const -> int;
    [[nodiscard]] auto current_index() const -> int;
    [[nodiscard]] auto current_result() const -> const SearchResult*;

    // ── Navigation ─────────────────────────────────────────────────
    auto next_result() -> void;
    auto previous_result() -> void;

    /// Get all result object IDs for highlighting.
    [[nodiscard]] auto highlighted_objects() const -> std::vector<ObjectId>;

    // ── Visibility ─────────────────────────────────────────────────
    auto show() -> void;
    auto hide() -> void;
    [[nodiscard]] auto is_visible() const -> bool;

    // ── Callback ───────────────────────────────────────────────────
    auto set_on_navigate(OnNavigateToResult callback) -> void;

private:
    std::string query_;
    std::vector<SearchResult> results_;
    int current_index_{-1};
    bool visible_{false};
    OnNavigateToResult on_navigate_;
};

} // namespace markamp::canvas
