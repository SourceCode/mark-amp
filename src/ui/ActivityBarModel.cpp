#include "ActivityBarModel.h"

#include <sstream>

namespace markamp::ui
{

void ActivityBarModel::add_item(ActivityBarItemModel item)
{
    item.order_index = static_cast<int>(items_.size());
    items_.push_back(std::move(item));
}

void ActivityBarModel::remove_item(const std::string& item_id)
{
    auto iter =
        std::remove_if(items_.begin(),
                       items_.end(),
                       [&](const ActivityBarItemModel& item) { return item.item_id == item_id; });
    if (iter != items_.end())
    {
        items_.erase(iter, items_.end());
    }
}

auto ActivityBarModel::visible_items() const -> std::vector<ActivityBarItemModel>
{
    std::vector<ActivityBarItemModel> result;
    for (const auto& item : items_)
    {
        if (item.visible)
        {
            result.push_back(item);
        }
    }
    return result;
}

auto ActivityBarModel::all_items() const -> const std::vector<ActivityBarItemModel>&
{
    return items_;
}

auto ActivityBarModel::item_count() const -> int
{
    return static_cast<int>(items_.size());
}

auto ActivityBarModel::focus_index() const -> int
{
    return focus_index_;
}

void ActivityBarModel::focus_next()
{
    const auto vis = visible_items();
    if (vis.empty())
    {
        return;
    }
    focus_index_ = (focus_index_ + 1) % static_cast<int>(vis.size());
}

void ActivityBarModel::focus_previous()
{
    const auto vis = visible_items();
    if (vis.empty())
    {
        return;
    }
    const auto count = static_cast<int>(vis.size());
    focus_index_ = (focus_index_ <= 0) ? (count - 1) : (focus_index_ - 1);
}

void ActivityBarModel::set_focus(int index)
{
    focus_index_ = index;
}

auto ActivityBarModel::active_item_id() const -> std::string
{
    return active_item_id_;
}

void ActivityBarModel::activate_focused()
{
    const auto vis = visible_items();
    if (focus_index_ >= 0 && focus_index_ < static_cast<int>(vis.size()))
    {
        active_item_id_ = vis[static_cast<std::size_t>(focus_index_)].item_id;
    }
}

void ActivityBarModel::set_active(const std::string& item_id)
{
    active_item_id_ = item_id;
}

void ActivityBarModel::reorder(int src_index, int dst_index)
{
    auto vis = visible_items();
    if (src_index < 0 || src_index >= static_cast<int>(vis.size()) || dst_index < 0 ||
        dst_index >= static_cast<int>(vis.size()) || src_index == dst_index)
    {
        return;
    }

    // Find the matching items in the full items_ list and swap order_index values
    const auto& src_id = vis[static_cast<std::size_t>(src_index)].item_id;
    const auto& dst_id = vis[static_cast<std::size_t>(dst_index)].item_id;

    int src_full = -1;
    int dst_full = -1;
    for (int idx = 0; idx < static_cast<int>(items_.size()); ++idx)
    {
        if (items_[static_cast<std::size_t>(idx)].item_id == src_id)
        {
            src_full = idx;
        }
        if (items_[static_cast<std::size_t>(idx)].item_id == dst_id)
        {
            dst_full = idx;
        }
    }

    if (src_full >= 0 && dst_full >= 0)
    {
        std::swap(items_[static_cast<std::size_t>(src_full)].order_index,
                  items_[static_cast<std::size_t>(dst_full)].order_index);

        // Re-sort by order_index
        std::sort(items_.begin(),
                  items_.end(),
                  [](const ActivityBarItemModel& lhs, const ActivityBarItemModel& rhs)
                  { return lhs.order_index < rhs.order_index; });
    }
}

void ActivityBarModel::set_badge(const std::string& item_id, BadgeStyle style, int count)
{
    for (auto& item : items_)
    {
        if (item.item_id == item_id)
        {
            item.badge_style = style;
            item.badge_count = count;
            return;
        }
    }
}

auto ActivityBarModel::badge_display(BadgeStyle style, int count) -> std::string
{
    switch (style)
    {
        case BadgeStyle::kNone:
            return "";
        case BadgeStyle::kDot:
            return count > 0 ? "●" : " ";
        case BadgeStyle::kUrgent:
            return "!";
        case BadgeStyle::kCount:
            if (count > 99)
            {
                return "99+";
            }
            return count > 0 ? std::to_string(count) : "";
    }
    return "";
}

void ActivityBarModel::set_item_visible(const std::string& item_id, bool visible)
{
    for (auto& item : items_)
    {
        if (item.item_id == item_id)
        {
            item.visible = visible;
            return;
        }
    }
}

void ActivityBarModel::reset_order()
{
    for (int idx = 0; idx < static_cast<int>(items_.size()); ++idx)
    {
        items_[static_cast<std::size_t>(idx)].order_index = idx;
    }
}
void ActivityBarModel::apply_layout(const std::vector<std::pair<std::string, bool>>& layout)
{
    std::vector<ActivityBarItemModel> new_items;
    new_items.reserve(items_.size());

    for (const auto& pair : layout)
    {
        const auto& id = pair.first;
        const auto& visible = pair.second;
        auto it = std::find_if(
            items_.begin(), items_.end(), [&id](const auto& item) { return item.item_id == id; });
        if (it != items_.end())
        {
            it->visible = visible;
            new_items.push_back(*it);
        }
    }

    for (const auto& item : items_)
    {
        auto it =
            std::find_if(new_items.begin(),
                         new_items.end(),
                         [&](const auto& new_item) { return new_item.item_id == item.item_id; });
        if (it == new_items.end())
        {
            new_items.push_back(item);
        }
    }

    items_ = std::move(new_items);
    for (int idx = 0; idx < static_cast<int>(items_.size()); ++idx)
    {
        items_[static_cast<std::size_t>(idx)].order_index = idx;
    }
}

auto ActivityBarModel::get_layout() const -> std::vector<std::pair<std::string, bool>>
{
    std::vector<std::pair<std::string, bool>> layout;
    layout.reserve(items_.size());
    for (const auto& item : items_)
    {
        layout.emplace_back(item.item_id, item.visible);
    }
    return layout;
}
auto ActivityBarModel::focused_announcement() const -> std::string
{
    const auto vis = visible_items();
    if (focus_index_ < 0 || focus_index_ >= static_cast<int>(vis.size()))
    {
        return "";
    }

    const auto& item = vis[static_cast<std::size_t>(focus_index_)];
    std::ostringstream announcement;
    announcement << "Activity Bar, " << item.accessible_name;
    announcement << ", " << (focus_index_ + 1) << " of " << vis.size();

    const auto badge_text = badge_display(item.badge_style, item.badge_count);
    if (!badge_text.empty())
    {
        announcement << ", badge: " << badge_text;
    }

    if (item.item_id == active_item_id_)
    {
        announcement << ", active";
    }

    return announcement.str();
}

} // namespace markamp::ui
