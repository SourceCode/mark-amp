#include "TabStripModel.h"

#include <algorithm>

namespace markamp::ui
{

void TabStripModel::add_tab(TabItemModel tab)
{
    tabs_.push_back(std::move(tab));
}

void TabStripModel::remove_tab(const std::string& file_path)
{
    tabs_.erase(std::remove_if(tabs_.begin(),
                               tabs_.end(),
                               [&file_path](const TabItemModel& tab_item)
                               { return tab_item.file_path == file_path; }),
                tabs_.end());
}

auto TabStripModel::tabs() const -> const std::vector<TabItemModel>&
{
    return tabs_;
}

auto TabStripModel::tab_count() const -> int
{
    return static_cast<int>(tabs_.size());
}

void TabStripModel::set_active(const std::string& file_path)
{
    for (auto& tab : tabs_)
    {
        tab.is_active = (tab.file_path == file_path);
    }
}

auto TabStripModel::active_path() const -> std::string
{
    for (const auto& tab : tabs_)
    {
        if (tab.is_active)
        {
            return tab.file_path;
        }
    }
    return "";
}

auto TabStripModel::overflowed_indices(int viewport_width, int min_tab_width) const
    -> std::vector<int>
{
    std::vector<int> result;
    if (min_tab_width <= 0)
    {
        return result;
    }

    int used_width = 0;
    for (int idx = 0; idx < tab_count(); ++idx)
    {
        used_width += min_tab_width;
        if (used_width > viewport_width)
        {
            result.push_back(idx);
        }
    }
    return result;
}

auto TabStripModel::tab_names() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    result.reserve(tabs_.size());
    for (const auto& tab : tabs_)
    {
        result.push_back(tab.display_name);
    }
    return result;
}

void TabStripModel::close_group(const std::string& group_id)
{
    tabs_.erase(std::remove_if(tabs_.begin(),
                               tabs_.end(),
                               [&group_id](const TabItemModel& tab_item) {
                                   return tab_item.group.has_value() &&
                                          tab_item.group->group_id == group_id;
                               }),
                tabs_.end());
}

void TabStripModel::pin_group(const std::string& group_id)
{
    for (auto& tab : tabs_)
    {
        if (tab.group.has_value() && tab.group->group_id == group_id)
        {
            tab.is_pinned = true;
        }
    }
}

void TabStripModel::set_indicator(const std::string& file_path, TabIndicator indicator)
{
    auto* tab = find_tab(file_path);
    if (tab != nullptr)
    {
        tab->indicator = indicator;
    }
}

auto TabStripModel::indicator_tooltip(TabIndicator indicator) -> std::string
{
    switch (indicator)
    {
        case TabIndicator::kClean:
            return "No unsaved changes";
        case TabIndicator::kModified:
            return "Unsaved changes";
        case TabIndicator::kExternallyChanged:
            return "Changed on disk";
        case TabIndicator::kConflict:
            return "Conflict: local and external changes";
    }
    return "";
}

auto TabStripModel::reorder(int src, int dst) -> bool
{
    if (src < 0 || src >= tab_count() || dst < 0 || dst >= tab_count() || src == dst)
    {
        return false;
    }

    // Enforce: pinned tabs stay among pinned
    const bool src_pinned = tabs_[static_cast<std::size_t>(src)].is_pinned;
    const bool dst_pinned = tabs_[static_cast<std::size_t>(dst)].is_pinned;
    if (src_pinned != dst_pinned)
    {
        return false; // Cannot reorder across pinned boundary
    }

    auto item = std::move(tabs_[static_cast<std::size_t>(src)]);
    tabs_.erase(tabs_.begin() + src);
    tabs_.insert(tabs_.begin() + dst, std::move(item));
    return true;
}

void TabStripModel::pin(const std::string& file_path)
{
    auto* tab = find_tab(file_path);
    if (tab != nullptr)
    {
        tab->is_pinned = true;
    }
}

void TabStripModel::unpin(const std::string& file_path)
{
    auto* tab = find_tab(file_path);
    if (tab != nullptr)
    {
        tab->is_pinned = false;
    }
}

auto TabStripModel::find_tab(const std::string& file_path) -> TabItemModel*
{
    for (auto& tab : tabs_)
    {
        if (tab.file_path == file_path)
        {
            return &tab;
        }
    }
    return nullptr;
}

} // namespace markamp::ui
