#include "AccessibilityModel.h"

#include <algorithm>

namespace markamp::canvas
{

void AccessibilityModel::set_objects(std::vector<AccessibleObject> objects)
{
    objects_ = std::move(objects);
    // Sort by tab_order
    std::sort(objects_.begin(),
              objects_.end(),
              [](const AccessibleObject& lhs, const AccessibleObject& rhs)
              { return lhs.tab_order < rhs.tab_order; });
    focused_index_ = 0;
}

auto AccessibilityModel::objects() const -> const std::vector<AccessibleObject>&
{
    return objects_;
}
auto AccessibilityModel::object_count() const -> int
{
    return static_cast<int>(objects_.size());
}

void AccessibilityModel::focus_next()
{
    if (!objects_.empty())
    {
        focused_index_ = (focused_index_ + 1) % static_cast<int>(objects_.size());
    }
}

void AccessibilityModel::focus_prev()
{
    if (!objects_.empty())
    {
        focused_index_ = (focused_index_ - 1 + static_cast<int>(objects_.size())) %
                         static_cast<int>(objects_.size());
    }
}

auto AccessibilityModel::focused_index() const -> int
{
    return focused_index_;
}

auto AccessibilityModel::focused_object() const -> AccessibleObject
{
    if (objects_.empty())
    {
        return {"", "", "", 0};
    }
    return objects_[static_cast<size_t>(focused_index_)];
}

void AccessibilityModel::set_focus_ring_visible(bool visible)
{
    focus_ring_visible_ = visible;
}
auto AccessibilityModel::focus_ring_visible() const -> bool
{
    return focus_ring_visible_;
}

void AccessibilityModel::set_focus_ring_contrast(double contrast)
{
    focus_ring_contrast_ = std::clamp(contrast, 0.5, 3.0);
}

auto AccessibilityModel::focus_ring_contrast() const -> double
{
    return focus_ring_contrast_;
}

void AccessibilityModel::set_reduced_motion(bool enabled)
{
    reduced_motion_ = enabled;
}
auto AccessibilityModel::reduced_motion() const -> bool
{
    return reduced_motion_;
}

} // namespace markamp::canvas
