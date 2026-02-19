#include "FrameContainerModel.h"

#include <algorithm>

namespace markamp::canvas
{

void FrameContainerModel::set_children(std::vector<ContainedObject> children)
{
    children_ = std::move(children);
}
auto FrameContainerModel::children() const -> const std::vector<ContainedObject>&
{
    return children_;
}

void FrameContainerModel::add_child(ContainedObject child)
{
    children_.push_back(std::move(child));
}

void FrameContainerModel::remove_child(const std::string& object_id)
{
    children_.erase(std::remove_if(children_.begin(),
                                   children_.end(),
                                   [&](const ContainedObject& obj)
                                   { return obj.object_id == object_id; }),
                    children_.end());
}

auto FrameContainerModel::contains(const std::string& object_id) const -> bool
{
    for (const auto& child : children_)
    {
        if (child.object_id == object_id)
        {
            return true;
        }
    }
    return false;
}

auto FrameContainerModel::child_count() const -> int
{
    return static_cast<int>(children_.size());
}

void FrameContainerModel::set_style(FrameStyle style)
{
    style_ = std::move(style);
}
auto FrameContainerModel::style() const -> const FrameStyle&
{
    return style_;
}

void FrameContainerModel::set_frame_ids(std::vector<std::string> ids)
{
    frame_ids_ = std::move(ids);
}
void FrameContainerModel::set_active_frame(const std::string& frame_id)
{
    active_frame_ = frame_id;
}
auto FrameContainerModel::active_frame() const -> const std::string&
{
    return active_frame_;
}

auto FrameContainerModel::next_frame() const -> std::string
{
    if (frame_ids_.empty())
    {
        return "";
    }
    for (size_t i = 0; i < frame_ids_.size(); ++i)
    {
        if (frame_ids_[i] == active_frame_ && i + 1 < frame_ids_.size())
        {
            return frame_ids_[i + 1];
        }
    }
    return frame_ids_.front(); // wrap
}

auto FrameContainerModel::prev_frame() const -> std::string
{
    if (frame_ids_.empty())
    {
        return "";
    }
    for (size_t i = 0; i < frame_ids_.size(); ++i)
    {
        if (frame_ids_[i] == active_frame_ && i > 0)
        {
            return frame_ids_[i - 1];
        }
    }
    return frame_ids_.back(); // wrap
}

void FrameContainerModel::set_select_mode(ContainerSelectMode mode)
{
    select_mode_ = mode;
}
auto FrameContainerModel::select_mode() const -> ContainerSelectMode
{
    return select_mode_;
}

} // namespace markamp::canvas
