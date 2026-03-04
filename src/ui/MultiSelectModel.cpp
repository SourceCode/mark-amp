#include "MultiSelectModel.h"

#include <algorithm>

namespace markamp::ui
{

auto SelectionModifiers::mode() const -> SelectionMode
{
    if (shift)
    {
        return SelectionMode::kRange;
    }
    if (ctrl_or_cmd)
    {
        return SelectionMode::kToggle;
    }
    return SelectionMode::kSingle;
}

void MultiSelectModel::set_items(std::vector<std::string> item_ids)
{
    items_ = std::move(item_ids);
    selected_.assign(items_.size(), false);
    focused_id_.clear();
    anchor_index_ = -1;
}

auto MultiSelectModel::item_count() const -> int
{
    return static_cast<int>(items_.size());
}

void MultiSelectModel::click(const std::string& item_id, const SelectionModifiers& modifiers)
{
    switch (modifiers.mode())
    {
        case SelectionMode::kSingle:
            select(item_id);
            break;
        case SelectionMode::kToggle:
            toggle(item_id);
            break;
        case SelectionMode::kRange:
            select_range(item_id);
            break;
        case SelectionMode::kAdditive:
        {
            int idx = index_of(item_id);
            if (idx >= 0)
            {
                selected_[static_cast<size_t>(idx)] = true;
                set_focus(item_id);
            }
            break;
        }
    }
}

void MultiSelectModel::select(const std::string& item_id)
{
    std::fill(selected_.begin(), selected_.end(), false);
    int idx = index_of(item_id);
    if (idx >= 0)
    {
        selected_[static_cast<size_t>(idx)] = true;
        anchor_index_ = idx;
        set_focus(item_id);
    }
}

void MultiSelectModel::toggle(const std::string& item_id)
{
    int idx = index_of(item_id);
    if (idx >= 0)
    {
        selected_[static_cast<size_t>(idx)] = !selected_[static_cast<size_t>(idx)];
        anchor_index_ = idx;
        set_focus(item_id);
    }
}

void MultiSelectModel::select_range(const std::string& target_id)
{
    int target_idx = index_of(target_id);
    if (target_idx < 0)
    {
        return;
    }

    int anchor = anchor_index_ >= 0 ? anchor_index_ : 0;
    int range_start = std::min(anchor, target_idx);
    int range_end = std::max(anchor, target_idx);

    std::fill(selected_.begin(), selected_.end(), false);
    for (int range_idx = range_start; range_idx <= range_end; ++range_idx)
    {
        selected_[static_cast<size_t>(range_idx)] = true;
    }
    set_focus(target_id);
}

void MultiSelectModel::select_all()
{
    std::fill(selected_.begin(), selected_.end(), true);
}

void MultiSelectModel::clear_selection()
{
    std::fill(selected_.begin(), selected_.end(), false);
    focused_id_.clear();
    anchor_index_ = -1;
}

auto MultiSelectModel::is_selected(const std::string& item_id) const -> bool
{
    int idx = index_of(item_id);
    if (idx >= 0)
    {
        return selected_[static_cast<size_t>(idx)];
    }
    return false;
}

auto MultiSelectModel::selected_ids() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (size_t item_idx = 0; item_idx < items_.size(); ++item_idx)
    {
        if (selected_[item_idx])
        {
            result.push_back(items_[item_idx]);
        }
    }
    return result;
}

auto MultiSelectModel::selection_count() const -> int
{
    return static_cast<int>(std::count(selected_.begin(), selected_.end(), true));
}

auto MultiSelectModel::summary() const -> SelectionSummary
{
    SelectionSummary result;
    result.total_count = selection_count();
    result.focused_index = index_of(focused_id_);
    result.anchor_index = anchor_index_;
    result.focused_id = focused_id_;
    return result;
}

auto MultiSelectModel::focused_id() const -> const std::string&
{
    return focused_id_;
}

auto MultiSelectModel::is_focused(const std::string& item_id) const -> bool
{
    return focused_id_ == item_id;
}

auto MultiSelectModel::index_of(const std::string& item_id) const -> int
{
    for (size_t item_idx = 0; item_idx < items_.size(); ++item_idx)
    {
        if (items_[item_idx] == item_id)
        {
            return static_cast<int>(item_idx);
        }
    }
    return -1;
}

void MultiSelectModel::set_focus(const std::string& item_id)
{
    focused_id_ = item_id;
}

} // namespace markamp::ui
