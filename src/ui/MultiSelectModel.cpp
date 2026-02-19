#include "MultiSelectModel.h"

#include <algorithm>

namespace markamp::ui
{

void MultiSelectModel::set_items(std::vector<SelectableItem> items)
{
    items_ = std::move(items);
    last_selected_index_ = -1;
}

auto MultiSelectModel::items() const -> const std::vector<SelectableItem>&
{
    return items_;
}

void MultiSelectModel::select(const std::string& item_id, SelectionModifier modifier)
{
    int target_index = -1;
    for (int i = 0; i < static_cast<int>(items_.size()); ++i)
    {
        if (items_[static_cast<size_t>(i)].item_id == item_id)
        {
            target_index = i;
            break;
        }
    }
    if (target_index < 0)
    {
        return;
    }

    switch (modifier)
    {
        case SelectionModifier::kNone:
            for (auto& item : items_)
            {
                item.is_selected = false;
            }
            items_[static_cast<size_t>(target_index)].is_selected = true;
            break;

        case SelectionModifier::kToggle:
            items_[static_cast<size_t>(target_index)].is_selected =
                !items_[static_cast<size_t>(target_index)].is_selected;
            break;

        case SelectionModifier::kExtend:
            if (last_selected_index_ >= 0)
            {
                const int low = std::min(last_selected_index_, target_index);
                const int high = std::max(last_selected_index_, target_index);
                for (int i = low; i <= high; ++i)
                {
                    items_[static_cast<size_t>(i)].is_selected = true;
                }
            }
            else
            {
                items_[static_cast<size_t>(target_index)].is_selected = true;
            }
            break;
    }
    last_selected_index_ = target_index;
}

void MultiSelectModel::select_all()
{
    for (auto& item : items_)
    {
        item.is_selected = true;
    }
}

void MultiSelectModel::deselect_all()
{
    for (auto& item : items_)
    {
        item.is_selected = false;
    }
    last_selected_index_ = -1;
}

auto MultiSelectModel::selected_count() const -> int
{
    int count = 0;
    for (const auto& item : items_)
    {
        if (item.is_selected)
        {
            ++count;
        }
    }
    return count;
}

auto MultiSelectModel::selected_ids() const -> std::vector<std::string>
{
    std::vector<std::string> ids;
    for (const auto& item : items_)
    {
        if (item.is_selected)
        {
            ids.push_back(item.item_id);
        }
    }
    return ids;
}

auto MultiSelectModel::bulk_actions() const -> std::vector<std::string>
{
    const int count = selected_count();
    if (count == 0)
    {
        return {};
    }
    if (count == 1)
    {
        return {"Open", "Rename", "Delete"};
    }
    return {"Move", "Delete", "Export"};
}

auto MultiSelectModel::status_text() const -> std::string
{
    const int count = selected_count();
    if (count == 0)
    {
        return "No items selected";
    }
    return std::to_string(count) + " item" + (count == 1 ? "" : "s") + " selected";
}

void MultiSelectModel::request_destructive(const std::string& action)
{
    pending_destructive_ = action;
}

void MultiSelectModel::confirm_destructive()
{
    pending_destructive_.clear();
}

void MultiSelectModel::cancel_destructive()
{
    pending_destructive_.clear();
}

auto MultiSelectModel::pending_destructive() const -> const std::string&
{
    return pending_destructive_;
}
auto MultiSelectModel::has_pending_destructive() const -> bool
{
    return !pending_destructive_.empty();
}

} // namespace markamp::ui
