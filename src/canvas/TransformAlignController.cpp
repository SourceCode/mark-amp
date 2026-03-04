#include "TransformAlignController.h"

#include <algorithm>
#include <limits>
#include <numeric>

namespace markamp::canvas
{

auto TransformAlignController::command_name(AlignCommand cmd) -> std::string
{
    switch (cmd)
    {
        case AlignCommand::kLeft:
            return "left";
        case AlignCommand::kRight:
            return "right";
        case AlignCommand::kTop:
            return "top";
        case AlignCommand::kBottom:
            return "bottom";
        case AlignCommand::kCenterH:
            return "center_h";
        case AlignCommand::kCenterV:
            return "center_v";
    }
    return "unknown";
}

auto TransformAlignController::align(const std::vector<AlignItem>& items, AlignCommand command)
    -> std::vector<AlignItem>
{
    if (items.size() < 2)
    {
        return items;
    }

    std::vector<AlignItem> result = items;

    switch (command)
    {
        case AlignCommand::kLeft:
        {
            double min_x = std::numeric_limits<double>::max();
            for (const auto& item : items)
            {
                min_x = std::min(min_x, item.pos_x);
            }
            for (auto& item : result)
            {
                item.pos_x = min_x;
            }
            break;
        }
        case AlignCommand::kRight:
        {
            double max_right = std::numeric_limits<double>::lowest();
            for (const auto& item : items)
            {
                max_right = std::max(max_right, item.pos_x + item.width);
            }
            for (auto& item : result)
            {
                item.pos_x = max_right - item.width;
            }
            break;
        }
        case AlignCommand::kTop:
        {
            double min_y = std::numeric_limits<double>::max();
            for (const auto& item : items)
            {
                min_y = std::min(min_y, item.pos_y);
            }
            for (auto& item : result)
            {
                item.pos_y = min_y;
            }
            break;
        }
        case AlignCommand::kBottom:
        {
            double max_bottom = std::numeric_limits<double>::lowest();
            for (const auto& item : items)
            {
                max_bottom = std::max(max_bottom, item.pos_y + item.height);
            }
            for (auto& item : result)
            {
                item.pos_y = max_bottom - item.height;
            }
            break;
        }
        case AlignCommand::kCenterH:
        {
            double min_x = std::numeric_limits<double>::max();
            double max_right = std::numeric_limits<double>::lowest();
            for (const auto& item : items)
            {
                min_x = std::min(min_x, item.pos_x);
                max_right = std::max(max_right, item.pos_x + item.width);
            }
            double center = (min_x + max_right) / 2.0;
            for (auto& item : result)
            {
                item.pos_x = center - item.width / 2.0;
            }
            break;
        }
        case AlignCommand::kCenterV:
        {
            double min_y = std::numeric_limits<double>::max();
            double max_bottom = std::numeric_limits<double>::lowest();
            for (const auto& item : items)
            {
                min_y = std::min(min_y, item.pos_y);
                max_bottom = std::max(max_bottom, item.pos_y + item.height);
            }
            double center = (min_y + max_bottom) / 2.0;
            for (auto& item : result)
            {
                item.pos_y = center - item.height / 2.0;
            }
            break;
        }
    }

    return result;
}

auto TransformAlignController::distribute(const std::vector<AlignItem>& items,
                                          DistributeCommand command) -> std::vector<AlignItem>
{
    if (items.size() < 3)
    {
        return items;
    }

    std::vector<AlignItem> result = items;

    if (command == DistributeCommand::kHorizontal)
    {
        // Sort by x position
        std::sort(result.begin(),
                  result.end(),
                  [](const AlignItem& lhs, const AlignItem& rhs) { return lhs.pos_x < rhs.pos_x; });

        double total_width = 0.0;
        for (const auto& item : result)
        {
            total_width += item.width;
        }

        double span = (result.back().pos_x + result.back().width) - result.front().pos_x;
        double gap = (span - total_width) / static_cast<double>(result.size() - 1);

        double current_x = result.front().pos_x;
        for (auto& item : result)
        {
            item.pos_x = current_x;
            current_x += item.width + gap;
        }
    }
    else // kVertical
    {
        std::sort(result.begin(),
                  result.end(),
                  [](const AlignItem& lhs, const AlignItem& rhs) { return lhs.pos_y < rhs.pos_y; });

        double total_height = 0.0;
        for (const auto& item : result)
        {
            total_height += item.height;
        }

        double span = (result.back().pos_y + result.back().height) - result.front().pos_y;
        double gap = (span - total_height) / static_cast<double>(result.size() - 1);

        double current_y = result.front().pos_y;
        for (auto& item : result)
        {
            item.pos_y = current_y;
            current_y += item.height + gap;
        }
    }

    return result;
}

void TransformAlignController::set_pivot(double pos_x, double pos_y)
{
    pivot_x_ = pos_x;
    pivot_y_ = pos_y;
}

auto TransformAlignController::pivot_x() const -> double
{
    return pivot_x_;
}

auto TransformAlignController::pivot_y() const -> double
{
    return pivot_y_;
}

void TransformAlignController::reset_pivot()
{
    pivot_x_ = 0.0;
    pivot_y_ = 0.0;
}

} // namespace markamp::canvas
