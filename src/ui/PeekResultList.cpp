// ============================================================================
// File: src/ui/PeekResultList.cpp
// Phase 47: Peek View System — Result list model
// ============================================================================
#include "PeekResultList.h"

namespace markamp::ui
{

void PeekResultListModel::set_results(const std::vector<core::PeekLocation>& results)
{
    results_ = results;
    rebuild_groups();
}

void PeekResultListModel::set_display_mode(PeekResultDisplayMode mode)
{
    display_mode_ = mode;
}

void PeekResultListModel::toggle_group(size_t group_index)
{
    if (group_index < groups_.size())
    {
        groups_[group_index].is_collapsed = !groups_[group_index].is_collapsed;
    }
}

void PeekResultListModel::rebuild_groups()
{
    groups_.clear();
    std::unordered_map<std::string, size_t> file_to_group;

    for (size_t i = 0; i < results_.size(); ++i)
    {
        const auto& loc = results_[i];
        auto it = file_to_group.find(loc.file_path);
        if (it == file_to_group.end())
        {
            file_to_group[loc.file_path] = groups_.size();
            groups_.push_back({.file_path = loc.file_path,
                               .file_name = extract_filename(loc.file_path),
                               .result_indices = {i}});
        }
        else
        {
            groups_[it->second].result_indices.push_back(i);
        }
    }
}

auto PeekResultListModel::extract_filename(const std::string& path) -> std::string
{
    auto pos = path.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        return path.substr(pos + 1);
    }
    return path;
}

} // namespace markamp::ui
