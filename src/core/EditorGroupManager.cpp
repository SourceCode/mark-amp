/// @file EditorGroupManager.cpp
/// @brief V9 Phase 46 — EditorGroupManager implementation.

#include "EditorGroupManager.h"

#include <algorithm>

namespace markamp::core
{

auto EditorGroupManager::create_group() -> std::string
{
    EditorGroup group;
    group.group_id = "grp_" + std::to_string(next_group_id_++);
    if (groups_.empty())
    {
        group.is_active = true;
    }
    groups_.push_back(std::move(group));
    return groups_.back().group_id;
}

auto EditorGroupManager::close_group(const std::string& group_id) -> bool
{
    auto iter = std::remove_if(groups_.begin(),
                               groups_.end(),
                               [&](const EditorGroup& grp) { return grp.group_id == group_id; });
    if (iter == groups_.end())
    {
        return false;
    }
    groups_.erase(iter, groups_.end());

    // Ensure one active group
    if (!groups_.empty())
    {
        bool has_active = false;
        for (const auto& grp : groups_)
        {
            if (grp.is_active)
            {
                has_active = true;
                break;
            }
        }
        if (!has_active)
        {
            groups_.back().is_active = true;
        }
    }
    return true;
}

auto EditorGroupManager::split_group(const std::string& group_id, SplitDirection /*direction*/)
    -> std::string
{
    auto* source = find_group_mut(group_id);
    if (source == nullptr)
    {
        return "";
    }
    // Create a new group adjacent to the source
    return create_group();
}

auto EditorGroupManager::open_tab(const std::string& group_id,
                                  const std::string& file_path,
                                  const std::string& title) -> std::string
{
    auto* group = find_group_mut(group_id);
    if (group == nullptr)
    {
        return "";
    }

    // Check if file already open
    for (int idx = 0; idx < static_cast<int>(group->tabs.size()); ++idx)
    {
        if (group->tabs[static_cast<size_t>(idx)].file_path == file_path)
        {
            group->active_tab_index = idx;
            return group->tabs[static_cast<size_t>(idx)].tab_id;
        }
    }

    EditorTab tab;
    tab.tab_id = "tab_" + std::to_string(next_tab_id_++);
    tab.file_path = file_path;
    tab.title = title.empty() ? file_path : title;
    group->tabs.push_back(std::move(tab));
    group->active_tab_index = static_cast<int>(group->tabs.size()) - 1;
    return group->tabs.back().tab_id;
}

auto EditorGroupManager::close_tab(const std::string& group_id, const std::string& tab_id) -> bool
{
    auto* group = find_group_mut(group_id);
    if (group == nullptr)
    {
        return false;
    }
    auto iter = std::remove_if(group->tabs.begin(),
                               group->tabs.end(),
                               [&](const EditorTab& tab) { return tab.tab_id == tab_id; });
    if (iter == group->tabs.end())
    {
        return false;
    }
    group->tabs.erase(iter, group->tabs.end());

    // Adjust active index
    if (group->tabs.empty())
    {
        group->active_tab_index = -1;
    }
    else if (group->active_tab_index >= static_cast<int>(group->tabs.size()))
    {
        group->active_tab_index = static_cast<int>(group->tabs.size()) - 1;
    }
    return true;
}

auto EditorGroupManager::pin_tab(const std::string& group_id, const std::string& tab_id) -> bool
{
    auto* group = find_group_mut(group_id);
    if (group == nullptr)
    {
        return false;
    }
    auto* tab = find_tab_mut(*group, tab_id);
    if (tab == nullptr)
    {
        return false;
    }
    tab->is_pinned = true;
    return true;
}

auto EditorGroupManager::unpin_tab(const std::string& group_id, const std::string& tab_id) -> bool
{
    auto* group = find_group_mut(group_id);
    if (group == nullptr)
    {
        return false;
    }
    auto* tab = find_tab_mut(*group, tab_id);
    if (tab == nullptr)
    {
        return false;
    }
    tab->is_pinned = false;
    return true;
}

auto EditorGroupManager::move_tab(const std::string& src_group,
                                  const std::string& tab_id,
                                  const std::string& dst_group) -> bool
{
    auto* source = find_group_mut(src_group);
    auto* dest = find_group_mut(dst_group);
    if (source == nullptr || dest == nullptr)
    {
        return false;
    }

    // Find and remove tab from source
    EditorTab moved_tab;
    bool found = false;
    for (auto iter = source->tabs.begin(); iter != source->tabs.end(); ++iter)
    {
        if (iter->tab_id == tab_id)
        {
            moved_tab = std::move(*iter);
            source->tabs.erase(iter);
            found = true;
            break;
        }
    }
    if (!found)
    {
        return false;
    }

    // Add to destination
    dest->tabs.push_back(std::move(moved_tab));
    dest->active_tab_index = static_cast<int>(dest->tabs.size()) - 1;

    // Fix source active index
    if (source->tabs.empty())
    {
        source->active_tab_index = -1;
    }
    else if (source->active_tab_index >= static_cast<int>(source->tabs.size()))
    {
        source->active_tab_index = static_cast<int>(source->tabs.size()) - 1;
    }
    return true;
}

auto EditorGroupManager::set_active_group(const std::string& group_id) -> bool
{
    auto* target = find_group_mut(group_id);
    if (target == nullptr)
    {
        return false;
    }
    for (auto& grp : groups_)
    {
        grp.is_active = false;
    }
    target->is_active = true;
    return true;
}

auto EditorGroupManager::active_group() const -> const EditorGroup*
{
    for (const auto& grp : groups_)
    {
        if (grp.is_active)
        {
            return &grp;
        }
    }
    return nullptr;
}

auto EditorGroupManager::get_group(const std::string& group_id) const -> const EditorGroup*
{
    for (const auto& grp : groups_)
    {
        if (grp.group_id == group_id)
        {
            return &grp;
        }
    }
    return nullptr;
}

auto EditorGroupManager::all_groups() const -> std::vector<const EditorGroup*>
{
    std::vector<const EditorGroup*> result;
    result.reserve(groups_.size());
    for (const auto& grp : groups_)
    {
        result.push_back(&grp);
    }
    return result;
}

auto EditorGroupManager::group_count() const -> int
{
    return static_cast<int>(groups_.size());
}

auto EditorGroupManager::total_tab_count() const -> int
{
    int count = 0;
    for (const auto& grp : groups_)
    {
        count += static_cast<int>(grp.tabs.size());
    }
    return count;
}

void EditorGroupManager::clear_all()
{
    groups_.clear();
}

auto EditorGroupManager::find_group_mut(const std::string& group_id) -> EditorGroup*
{
    for (auto& grp : groups_)
    {
        if (grp.group_id == group_id)
        {
            return &grp;
        }
    }
    return nullptr;
}

auto EditorGroupManager::find_tab_mut(EditorGroup& group, const std::string& tab_id) -> EditorTab*
{
    for (auto& tab : group.tabs)
    {
        if (tab.tab_id == tab_id)
        {
            return &tab;
        }
    }
    return nullptr;
}

} // namespace markamp::core
