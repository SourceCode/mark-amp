#include "SearchPanelModel.h"

#include <algorithm>
#include <map>
#include <set>

namespace markamp::ui
{

void SearchPanelModel::set_query(SearchQuery query)
{
    query_ = std::move(query);
}

auto SearchPanelModel::query() const -> const SearchQuery&
{
    return query_;
}

void SearchPanelModel::set_matches(std::vector<SearchMatch> matches)
{
    matches_ = std::move(matches);
}

auto SearchPanelModel::grouped_results() const -> std::vector<SearchFileGroup>
{
    // Preserve insertion order via ordered map
    std::map<std::string, std::vector<SearchMatch>> groups;
    std::vector<std::string> order;
    for (const auto& match : matches_)
    {
        if (groups.find(match.file_path) == groups.end())
        {
            order.push_back(match.file_path);
        }
        groups[match.file_path].push_back(match);
    }

    std::vector<SearchFileGroup> result;
    for (const auto& file_path : order)
    {
        bool collapsed = std::find(collapsed_files_.begin(), collapsed_files_.end(), file_path) !=
                         collapsed_files_.end();
        result.push_back({file_path, groups[file_path], collapsed});
    }
    return result;
}

void SearchPanelModel::toggle_collapse(const std::string& file_path)
{
    auto iter = std::find(collapsed_files_.begin(), collapsed_files_.end(), file_path);
    if (iter != collapsed_files_.end())
    {
        collapsed_files_.erase(iter);
    }
    else
    {
        collapsed_files_.push_back(file_path);
    }
}

auto SearchPanelModel::match_count() const -> int
{
    return static_cast<int>(matches_.size());
}

auto SearchPanelModel::file_count() const -> int
{
    std::set<std::string> files;
    for (const auto& match : matches_)
    {
        files.insert(match.file_path);
    }
    return static_cast<int>(files.size());
}

void SearchPanelModel::set_replace_text(const std::string& text)
{
    replace_text_ = text;
}

auto SearchPanelModel::replace_text() const -> const std::string&
{
    return replace_text_;
}

void SearchPanelModel::toggle_match_inclusion(const std::string& file_path, int line_number)
{
    for (auto& match : matches_)
    {
        if (match.file_path == file_path && match.line_number == line_number)
        {
            match.is_included = !match.is_included;
            return;
        }
    }
}

auto SearchPanelModel::included_count() const -> int
{
    int count = 0;
    for (const auto& match : matches_)
    {
        if (match.is_included)
        {
            ++count;
        }
    }
    return count;
}

void SearchPanelModel::push_history()
{
    if (!query_.text.empty())
    {
        history_.insert(history_.begin(), query_);
    }
}

auto SearchPanelModel::history() const -> const std::vector<SearchQuery>&
{
    return history_;
}

void SearchPanelModel::clear_history()
{
    history_.clear();
}

} // namespace markamp::ui
