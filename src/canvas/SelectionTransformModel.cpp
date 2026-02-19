#include "SelectionTransformModel.h"

#include <algorithm>

namespace markamp::canvas
{

void SelectionTransformModel::set_selection(std::vector<SelectionRect> objects)
{
    selection_ = std::move(objects);
}
auto SelectionTransformModel::selection() const -> const std::vector<SelectionRect>&
{
    return selection_;
}
auto SelectionTransformModel::selection_count() const -> int
{
    return static_cast<int>(selection_.size());
}

auto SelectionTransformModel::aligned(AlignDirection direction) const -> std::vector<SelectionRect>
{
    if (selection_.size() < 2)
    {
        return selection_;
    }
    std::vector<SelectionRect> result = selection_;

    switch (direction)
    {
        case AlignDirection::kLeft:
        {
            double min_x = result[0].x;
            for (const auto& obj : result)
            {
                min_x = std::min(min_x, obj.x);
            }
            for (auto& obj : result)
            {
                obj.x = min_x;
            }
            break;
        }
        case AlignDirection::kRight:
        {
            double max_right = result[0].x + result[0].width;
            for (const auto& obj : result)
            {
                max_right = std::max(max_right, obj.x + obj.width);
            }
            for (auto& obj : result)
            {
                obj.x = max_right - obj.width;
            }
            break;
        }
        case AlignDirection::kTop:
        {
            double min_y = result[0].y;
            for (const auto& obj : result)
            {
                min_y = std::min(min_y, obj.y);
            }
            for (auto& obj : result)
            {
                obj.y = min_y;
            }
            break;
        }
        case AlignDirection::kBottom:
        {
            double max_bot = result[0].y + result[0].height;
            for (const auto& obj : result)
            {
                max_bot = std::max(max_bot, obj.y + obj.height);
            }
            for (auto& obj : result)
            {
                obj.y = max_bot - obj.height;
            }
            break;
        }
        case AlignDirection::kCenterH:
        {
            double sum_cx = 0;
            for (const auto& obj : result)
            {
                sum_cx += obj.x + obj.width / 2.0;
            }
            const double center = sum_cx / static_cast<double>(result.size());
            for (auto& obj : result)
            {
                obj.x = center - obj.width / 2.0;
            }
            break;
        }
        case AlignDirection::kCenterV:
        {
            double sum_cy = 0;
            for (const auto& obj : result)
            {
                sum_cy += obj.y + obj.height / 2.0;
            }
            const double center = sum_cy / static_cast<double>(result.size());
            for (auto& obj : result)
            {
                obj.y = center - obj.height / 2.0;
            }
            break;
        }
    }
    return result;
}

auto SelectionTransformModel::distributed(DistributeDirection direction) const
    -> std::vector<SelectionRect>
{
    if (selection_.size() < 3)
    {
        return selection_;
    }

    std::vector<SelectionRect> result = selection_;

    if (direction == DistributeDirection::kHorizontal)
    {
        std::sort(result.begin(),
                  result.end(),
                  [](const SelectionRect& lhs, const SelectionRect& rhs) { return lhs.x < rhs.x; });
        const double first_x = result.front().x;
        const double last_right = result.back().x + result.back().width;
        double total_width = 0;
        for (const auto& obj : result)
        {
            total_width += obj.width;
        }
        const double gap =
            (last_right - first_x - total_width) / (static_cast<double>(result.size()) - 1.0);
        double current_x = first_x;
        for (auto& obj : result)
        {
            obj.x = current_x;
            current_x += obj.width + gap;
        }
    }
    else
    {
        std::sort(result.begin(),
                  result.end(),
                  [](const SelectionRect& lhs, const SelectionRect& rhs) { return lhs.y < rhs.y; });
        const double first_y = result.front().y;
        const double last_bot = result.back().y + result.back().height;
        double total_height = 0;
        for (const auto& obj : result)
        {
            total_height += obj.height;
        }
        const double gap =
            (last_bot - first_y - total_height) / (static_cast<double>(result.size()) - 1.0);
        double current_y = first_y;
        for (auto& obj : result)
        {
            obj.y = current_y;
            current_y += obj.height + gap;
        }
    }
    return result;
}

void SelectionTransformModel::set_pivot(double x_ratio, double y_ratio)
{
    pivot_x_ = std::clamp(x_ratio, 0.0, 1.0);
    pivot_y_ = std::clamp(y_ratio, 0.0, 1.0);
}

auto SelectionTransformModel::pivot_x() const -> double
{
    return pivot_x_;
}
auto SelectionTransformModel::pivot_y() const -> double
{
    return pivot_y_;
}

void SelectionTransformModel::set_active_handle(HandleType handle)
{
    active_handle_ = handle;
}
auto SelectionTransformModel::active_handle() const -> HandleType
{
    return active_handle_;
}

} // namespace markamp::canvas
