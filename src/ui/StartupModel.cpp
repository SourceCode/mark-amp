#include "StartupModel.h"

#include <algorithm>
#include <cctype>

namespace markamp::ui
{

void StartupModel::set_actions(std::vector<StartupAction> actions)
{
    actions_ = std::move(actions);
}

auto StartupModel::sorted_actions() const -> std::vector<StartupAction>
{
    auto sorted = actions_;
    std::sort(sorted.begin(),
              sorted.end(),
              [](const StartupAction& lhs, const StartupAction& rhs)
              { return lhs.priority < rhs.priority; });
    return sorted;
}

void StartupModel::set_presets(std::vector<WorkspacePreset> presets)
{
    presets_ = std::move(presets);
}

auto StartupModel::presets() const -> const std::vector<WorkspacePreset>&
{
    return presets_;
}

auto StartupModel::preset_by_id(const std::string& preset_id) const -> const WorkspacePreset*
{
    for (const auto& preset : presets_)
    {
        if (preset.preset_id == preset_id)
        {
            return &preset;
        }
    }
    return nullptr;
}

void StartupModel::set_recents(std::vector<RecentWorkspace> recents)
{
    recents_ = std::move(recents);
}

auto StartupModel::sorted_recents() const -> std::vector<RecentWorkspace>
{
    auto sorted = recents_;
    std::stable_sort(sorted.begin(),
                     sorted.end(),
                     [](const RecentWorkspace& lhs, const RecentWorkspace& rhs)
                     {
                         if (lhs.is_pinned != rhs.is_pinned)
                         {
                             return lhs.is_pinned;
                         }
                         return lhs.open_count > rhs.open_count;
                     });
    return sorted;
}

void StartupModel::toggle_pin(const std::string& workspace_path)
{
    for (auto& recent : recents_)
    {
        if (recent.workspace_path == workspace_path)
        {
            recent.is_pinned = !recent.is_pinned;
            return;
        }
    }
}

void StartupModel::remove_recent(const std::string& workspace_path)
{
    recents_.erase(std::remove_if(recents_.begin(),
                                  recents_.end(),
                                  [&workspace_path](const RecentWorkspace& recent)
                                  { return recent.workspace_path == workspace_path; }),
                   recents_.end());
}

auto StartupModel::search_recents(const std::string& query) const -> std::vector<RecentWorkspace>
{
    if (query.empty())
    {
        return recents_;
    }

    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    std::vector<RecentWorkspace> results;
    for (const auto& recent : recents_)
    {
        std::string lower_name = recent.display_name;
        std::transform(lower_name.begin(),
                       lower_name.end(),
                       lower_name.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

        if (lower_name.find(lower_query) != std::string::npos)
        {
            results.push_back(recent);
        }
    }
    return results;
}

auto StartupModel::recent_count() const -> int
{
    return static_cast<int>(recents_.size());
}

void StartupModel::set_first_run(bool is_first)
{
    first_run_ = is_first;
}

auto StartupModel::is_first_run() const -> bool
{
    return first_run_;
}

} // namespace markamp::ui
