#include "CanvasSearchModel.h"

#include <algorithm>

namespace markamp::canvas
{

void CanvasSearchModel::set_query(const std::string& query)
{
    query_ = query;
    // push to history if non-empty
    if (!query.empty())
    {
        query_history_.erase(std::remove(query_history_.begin(), query_history_.end(), query),
                             query_history_.end());
        query_history_.insert(query_history_.begin(), query);
        if (static_cast<int>(query_history_.size()) > kMaxHistory)
        {
            query_history_.resize(static_cast<size_t>(kMaxHistory));
        }
    }
}

auto CanvasSearchModel::query() const -> const std::string&
{
    return query_;
}

void CanvasSearchModel::set_results(std::vector<SearchResult> results)
{
    results_ = std::move(results);
}
auto CanvasSearchModel::results() const -> const std::vector<SearchResult>&
{
    return results_;
}
auto CanvasSearchModel::result_count() const -> int
{
    return static_cast<int>(results_.size());
}

void CanvasSearchModel::add_filter(FilterChip chip)
{
    filters_.push_back(std::move(chip));
}

void CanvasSearchModel::remove_filter(const std::string& key)
{
    filters_.erase(std::remove_if(filters_.begin(),
                                  filters_.end(),
                                  [&](const FilterChip& chip) { return chip.key == key; }),
                   filters_.end());
}

void CanvasSearchModel::clear_filters()
{
    filters_.clear();
}

auto CanvasSearchModel::active_filters() const -> std::vector<FilterChip>
{
    std::vector<FilterChip> result;
    for (const auto& chip : filters_)
    {
        if (chip.active)
        {
            result.push_back(chip);
        }
    }
    return result;
}

void CanvasSearchModel::set_highlighted(const std::string& object_id)
{
    highlighted_ = object_id;
}
auto CanvasSearchModel::highlighted() const -> const std::string&
{
    return highlighted_;
}

void CanvasSearchModel::save_search(SavedSearch preset)
{
    saved_searches_.push_back(std::move(preset));
}

void CanvasSearchModel::remove_saved(const std::string& preset_name)
{
    saved_searches_.erase(std::remove_if(saved_searches_.begin(),
                                         saved_searches_.end(),
                                         [&](const SavedSearch& saved)
                                         { return saved.preset_name == preset_name; }),
                          saved_searches_.end());
}

auto CanvasSearchModel::saved_searches() const -> const std::vector<SavedSearch>&
{
    return saved_searches_;
}
auto CanvasSearchModel::query_history() const -> const std::vector<std::string>&
{
    return query_history_;
}

} // namespace markamp::canvas
