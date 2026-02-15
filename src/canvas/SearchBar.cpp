#include "SearchBar.h"

#include <unordered_set>

namespace markamp::canvas
{

// ── Query ──────────────────────────────────────────────────────────────

auto SearchBar::set_query(const std::string& query) -> void
{
    query_ = query;
    current_index_ = -1;
}

auto SearchBar::query() const -> const std::string&
{
    return query_;
}

// ── Results ────────────────────────────────────────────────────────────

auto SearchBar::set_results(const std::vector<SearchResult>& results) -> void
{
    results_ = results;
    current_index_ = results_.empty() ? -1 : 0;
}

auto SearchBar::result_count() const -> int
{
    return static_cast<int>(results_.size());
}

auto SearchBar::current_index() const -> int
{
    return current_index_;
}

auto SearchBar::current_result() const -> const SearchResult*
{
    if (current_index_ < 0 || current_index_ >= static_cast<int>(results_.size()))
    {
        return nullptr;
    }
    return &results_[static_cast<size_t>(current_index_)];
}

// ── Navigation ─────────────────────────────────────────────────────────

auto SearchBar::next_result() -> void
{
    if (results_.empty())
    {
        return;
    }
    current_index_ = (current_index_ + 1) % static_cast<int>(results_.size());
}

auto SearchBar::previous_result() -> void
{
    if (results_.empty())
    {
        return;
    }
    current_index_ = (current_index_ - 1 + static_cast<int>(results_.size())) %
                     static_cast<int>(results_.size());
}

auto SearchBar::highlighted_objects() const -> std::vector<ObjectId>
{
    std::unordered_set<ObjectId> unique_ids;
    for (const auto& result : results_)
    {
        unique_ids.insert(result.object_id);
    }
    return {unique_ids.begin(), unique_ids.end()};
}

// ── Visibility ─────────────────────────────────────────────────────────

auto SearchBar::show() -> void
{
    visible_ = true;
}
auto SearchBar::hide() -> void
{
    visible_ = false;
    current_index_ = -1;
}
auto SearchBar::is_visible() const -> bool
{
    return visible_;
}

// ── Callback ───────────────────────────────────────────────────────────

auto SearchBar::set_on_navigate(OnNavigateToResult callback) -> void
{
    on_navigate_ = std::move(callback);
}

} // namespace markamp::canvas
