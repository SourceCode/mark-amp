#include "SelectionSemantics.h"

#include <algorithm>
#include <limits>

namespace markamp::canvas
{

auto SelectionSemantics::mode_name(SelectionMode mode) -> std::string
{
    switch (mode)
    {
        case SelectionMode::kClick:
            return "click";
        case SelectionMode::kShiftClick:
            return "shift_click";
        case SelectionMode::kMarquee:
            return "marquee";
        case SelectionMode::kLasso:
            return "lasso";
    }
    return "unknown";
}

void SelectionSemantics::select(const SelectedItem& item)
{
    selected_.clear();
    selected_.push_back(item);
}

void SelectionSemantics::add_to_selection(const SelectedItem& item)
{
    // Don't add duplicates
    for (const auto& sel : selected_)
    {
        if (sel.object_id == item.object_id)
        {
            return;
        }
    }
    selected_.push_back(item);
}

void SelectionSemantics::toggle_selection(const std::string& object_id)
{
    auto iter =
        std::find_if(selected_.begin(),
                     selected_.end(),
                     [&object_id](const SelectedItem& sel) { return sel.object_id == object_id; });
    if (iter != selected_.end())
    {
        selected_.erase(iter);
    }
}

void SelectionSemantics::clear()
{
    selected_.clear();
}

void SelectionSemantics::deselect(const std::string& object_id)
{
    selected_.erase(std::remove_if(selected_.begin(),
                                   selected_.end(),
                                   [&object_id](const SelectedItem& sel)
                                   { return sel.object_id == object_id; }),
                    selected_.end());
}

auto SelectionSemantics::count() const -> int
{
    return static_cast<int>(selected_.size());
}

auto SelectionSemantics::is_selected(const std::string& object_id) const -> bool
{
    return std::any_of(selected_.begin(),
                       selected_.end(),
                       [&object_id](const SelectedItem& sel)
                       { return sel.object_id == object_id; });
}

auto SelectionSemantics::items() const -> const std::vector<SelectedItem>&
{
    return selected_;
}

auto SelectionSemantics::bounds() const -> SelectionBounds
{
    if (selected_.empty())
    {
        return {};
    }

    SelectionBounds result;
    result.min_x = std::numeric_limits<double>::max();
    result.min_y = std::numeric_limits<double>::max();
    result.max_x = std::numeric_limits<double>::lowest();
    result.max_y = std::numeric_limits<double>::lowest();

    for (const auto& item : selected_)
    {
        result.min_x = std::min(result.min_x, item.pos_x);
        result.min_y = std::min(result.min_y, item.pos_y);
        result.max_x = std::max(result.max_x, item.pos_x + item.width);
        result.max_y = std::max(result.max_y, item.pos_y + item.height);
    }

    return result;
}

} // namespace markamp::canvas
