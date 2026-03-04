#include "ContextMenuModel.h"

#include <algorithm>

namespace markamp::ui
{

void ContextMenuModel::set_items(std::vector<ContextMenuItem> items)
{
    items_ = std::move(items);
}

auto ContextMenuModel::sorted_items() const -> std::vector<ContextMenuItem>
{
    auto sorted = items_;
    std::stable_sort(sorted.begin(),
                     sorted.end(),
                     [](const ContextMenuItem& lhs, const ContextMenuItem& rhs)
                     { return static_cast<int>(lhs.category) < static_cast<int>(rhs.category); });
    return sorted;
}

auto ContextMenuModel::visible_items() const -> std::vector<ContextMenuItem>
{
    std::vector<ContextMenuItem> result;
    for (const auto& item : items_)
    {
        if (item.is_visible)
        {
            result.push_back(item);
        }
    }
    return result;
}

void ContextMenuModel::set_enabled(const std::string& action_id, bool enabled)
{
    auto* item = find_item(action_id);
    if (item != nullptr)
    {
        item->is_enabled = enabled;
    }
}

void ContextMenuModel::set_visible(const std::string& action_id, bool visible)
{
    auto* item = find_item(action_id);
    if (item != nullptr)
    {
        item->is_visible = visible;
    }
}

auto ContextMenuModel::is_empty_state() const -> bool
{
    for (const auto& item : items_)
    {
        if (!item.is_separator && item.is_visible && item.is_enabled)
        {
            return false;
        }
    }
    return true;
}

auto ContextMenuModel::empty_area_fallbacks() -> std::vector<ContextMenuItem>
{
    return {
        {.action_id = "new_file",
         .label = "New File",
         .shortcut = "Cmd+N",
         .category = MenuActionCategory::kCreate,
         .is_enabled = true,
         .is_visible = true,
         .is_separator = false,
         .icon_name = "file-add"},
        {.action_id = "new_folder",
         .label = "New Folder",
         .shortcut = "",
         .category = MenuActionCategory::kCreate,
         .is_enabled = true,
         .is_visible = true,
         .is_separator = false,
         .icon_name = "folder-add"},
        {.action_id = "",
         .label = "",
         .shortcut = "",
         .category = MenuActionCategory::kOther,
         .is_enabled = false,
         .is_visible = true,
         .is_separator = true,
         .icon_name = ""},
        {.action_id = "paste",
         .label = "Paste",
         .shortcut = "Cmd+V",
         .category = MenuActionCategory::kEdit,
         .is_enabled = true,
         .is_visible = true,
         .is_separator = false,
         .icon_name = "clipboard"},
        {.action_id = "open_folder",
         .label = "Open Folder…",
         .shortcut = "Cmd+O",
         .category = MenuActionCategory::kNavigation,
         .is_enabled = true,
         .is_visible = true,
         .is_separator = false,
         .icon_name = "folder-open"},
    };
}

auto ContextMenuModel::action_count() const -> int
{
    int count = 0;
    for (const auto& item : items_)
    {
        if (!item.is_separator)
        {
            ++count;
        }
    }
    return count;
}

auto ContextMenuModel::find_item(const std::string& action_id) -> ContextMenuItem*
{
    for (auto& item : items_)
    {
        if (item.action_id == action_id)
        {
            return &item;
        }
    }
    return nullptr;
}

} // namespace markamp::ui
