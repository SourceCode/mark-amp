#include "SelectionModel.h"

#include <algorithm>

namespace markamp::node_editor
{

void SelectionModel::select(NodeId node_id)
{
    selected_.insert(node_id);
}

void SelectionModel::deselect(NodeId node_id)
{
    selected_.erase(node_id);
}

void SelectionModel::toggle(NodeId node_id)
{
    if (is_selected(node_id))
    {
        deselect(node_id);
    }
    else
    {
        select(node_id);
    }
}

void SelectionModel::select_only(NodeId node_id)
{
    selected_.clear();
    selected_.insert(node_id);
    focused_ = node_id;
}

void SelectionModel::deselect_all()
{
    selected_.clear();
}

void SelectionModel::select_all(const std::vector<NodeId>& all_node_ids)
{
    for (const auto& node_id : all_node_ids)
    {
        selected_.insert(node_id);
    }
}

void SelectionModel::select_set(const std::vector<NodeId>& node_ids)
{
    selected_.clear();
    for (const auto& node_id : node_ids)
    {
        selected_.insert(node_id);
    }
}

void SelectionModel::select_in_rect(Rect world_rect,
                                    const std::vector<NodeId>& all_node_ids,
                                    const std::vector<Rect>& all_bounds)
{
    const std::size_t kCount = std::min(all_node_ids.size(), all_bounds.size());
    for (std::size_t idx = 0; idx < kCount; ++idx)
    {
        if (world_rect.intersects(all_bounds[idx]))
        {
            selected_.insert(all_node_ids[idx]);
        }
    }
}

auto SelectionModel::is_selected(NodeId node_id) const -> bool
{
    return selected_.contains(node_id);
}

auto SelectionModel::count() const -> std::size_t
{
    return selected_.size();
}

auto SelectionModel::empty() const -> bool
{
    return selected_.empty();
}

auto SelectionModel::selected_ids() const -> std::vector<NodeId>
{
    return {selected_.begin(), selected_.end()};
}

void SelectionModel::focus_node(NodeId node_id)
{
    focused_ = node_id;
}

void SelectionModel::focus_next(const std::vector<NodeId>& all_node_ids)
{
    if (all_node_ids.empty())
    {
        return;
    }

    const std::size_t kCurrent = find_focus_index(all_node_ids);
    const std::size_t kNext = (kCurrent + 1) % all_node_ids.size();
    focused_ = all_node_ids[kNext];
}

void SelectionModel::focus_prev(const std::vector<NodeId>& all_node_ids)
{
    if (all_node_ids.empty())
    {
        return;
    }

    const std::size_t kCurrent = find_focus_index(all_node_ids);
    const std::size_t kPrev = (kCurrent == 0) ? all_node_ids.size() - 1 : kCurrent - 1;
    focused_ = all_node_ids[kPrev];
}

auto SelectionModel::focused_node() const noexcept -> NodeId
{
    return focused_;
}

auto SelectionModel::has_focus() const noexcept -> bool
{
    return focused_.is_valid();
}

void SelectionModel::clear_focus()
{
    focused_ = {};
}

auto SelectionModel::find_focus_index(const std::vector<NodeId>& all_node_ids) const -> std::size_t
{
    for (std::size_t idx = 0; idx < all_node_ids.size(); ++idx)
    {
        if (all_node_ids[idx] == focused_)
        {
            return idx;
        }
    }
    return 0;
}

} // namespace markamp::node_editor
