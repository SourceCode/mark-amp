#include "AutoLayoutModel.h"

#include <algorithm>

namespace markamp::canvas
{

void AutoLayoutModel::set_objects(std::vector<LayoutObject> objects)
{
    objects_ = std::move(objects);
}
auto AutoLayoutModel::objects() const -> const std::vector<LayoutObject>&
{
    return objects_;
}

void AutoLayoutModel::set_spacing(double spacing)
{
    spacing_ = std::max(0.0, spacing);
}
auto AutoLayoutModel::spacing() const -> double
{
    return spacing_;
}

void AutoLayoutModel::apply_preset(LayoutPresetType preset, double start_x, double start_y)
{
    double cursor_x = start_x;
    double cursor_y = start_y;
    int col_count = 0;
    const int kGridCols = 3;

    for (auto& obj : objects_)
    {
        obj.pos_x = cursor_x;
        obj.pos_y = cursor_y;

        switch (preset)
        {
            case LayoutPresetType::kRow:
                cursor_x += obj.width + spacing_;
                break;
            case LayoutPresetType::kColumn:
                cursor_y += obj.height + spacing_;
                break;
            case LayoutPresetType::kGrid:
                ++col_count;
                if (col_count >= kGridCols)
                {
                    col_count = 0;
                    cursor_x = start_x;
                    cursor_y += obj.height + spacing_;
                }
                else
                {
                    cursor_x += obj.width + spacing_;
                }
                break;
        }
    }
}

void AutoLayoutModel::normalize_spacing()
{
    if (objects_.size() < 2)
    {
        return;
    }

    // Sort by pos_x then pos_y
    std::sort(objects_.begin(),
              objects_.end(),
              [](const LayoutObject& lhs, const LayoutObject& rhs)
              {
                  if (lhs.pos_y != rhs.pos_y)
                  {
                      return lhs.pos_y < rhs.pos_y;
                  }
                  return lhs.pos_x < rhs.pos_x;
              });

    // Apply uniform spacing
    for (size_t idx = 1; idx < objects_.size(); ++idx)
    {
        objects_[idx].pos_x = objects_[idx - 1].pos_x + objects_[idx - 1].width + spacing_;
    }
}

void AutoLayoutModel::set_container_bounds(double bound_x,
                                           double bound_y,
                                           double bound_w,
                                           double bound_h)
{
    bound_x_ = bound_x;
    bound_y_ = bound_y;
    bound_w_ = std::max(1.0, bound_w);
    bound_h_ = std::max(1.0, bound_h);
}

auto AutoLayoutModel::respects_bounds() const -> bool
{
    for (const auto& obj : objects_)
    {
        if (obj.pos_x < bound_x_ || obj.pos_y < bound_y_ ||
            (obj.pos_x + obj.width) > (bound_x_ + bound_w_) ||
            (obj.pos_y + obj.height) > (bound_y_ + bound_h_))
        {
            return false;
        }
    }
    return true;
}

} // namespace markamp::canvas
