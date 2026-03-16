// ============================================================================
// File: src/canvas/CanvasObjectAligner.cpp
// Phase 12: Canvas Advanced Objects — alignment and distribution
// ============================================================================
#include "canvas/CanvasObjectAligner.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace markamp::canvas
{

CanvasObjectAligner::CanvasObjectAligner(Board& board)
    : board_(board)
{
}

// ── Alignment ─────────────────────────────────────────────────────

auto CanvasObjectAligner::align(const std::vector<ObjectId>& ids, AlignDirection direction)
    -> AlignResult
{
    if (ids.size() < 2)
    {
        return {false, 0};
    }
    return align_to_reference(ids, ids.front(), direction);
}

auto CanvasObjectAligner::align_to_reference(const std::vector<ObjectId>& ids,
                                             ObjectId reference_id,
                                             AlignDirection direction) -> AlignResult
{
    const auto align_data = collect_align_data(ids);
    if (align_data.size() < 2)
    {
        return {false, 0};
    }

    // Find reference object data
    const auto ref_it = std::ranges::find_if(align_data,
                                             [reference_id](const ObjectAlignData& entry)
                                             { return entry.obj_id == reference_id; });
    if (ref_it == align_data.end())
    {
        return {false, 0};
    }

    size_t moved = 0;
    for (const auto& entry : align_data)
    {
        if (entry.obj_id == reference_id)
        {
            continue;
        }

        double new_x = entry.center_x;
        double new_y = entry.center_y;

        switch (direction)
        {
            case AlignDirection::kLeft:
                new_x = ref_it->left + entry.width / 2.0;
                break;
            case AlignDirection::kRight:
                new_x = ref_it->right - entry.width / 2.0;
                break;
            case AlignDirection::kTop:
                new_y = ref_it->top + entry.height / 2.0;
                break;
            case AlignDirection::kBottom:
                new_y = ref_it->bottom - entry.height / 2.0;
                break;
            case AlignDirection::kCenterHorizontal:
                new_x = ref_it->center_x;
                break;
            case AlignDirection::kCenterVertical:
                new_y = ref_it->center_y;
                break;
        }

        if (std::abs(new_x - entry.center_x) > 0.01 || std::abs(new_y - entry.center_y) > 0.01)
        {
            move_object_to(entry.obj_id, new_x, new_y);
            ++moved;
        }
    }

    return {moved > 0, moved};
}

// ── Distribution ──────────────────────────────────────────────────

auto CanvasObjectAligner::distribute(const std::vector<ObjectId>& ids, DistributeMode mode)
    -> AlignResult
{
    if (ids.size() < 3)
    {
        return {false, 0};
    }

    auto align_data = collect_align_data(ids);

    // Sort by position along the distribution axis
    const bool horizontal =
        (mode == DistributeMode::kHorizontal || mode == DistributeMode::kEqualSpacingH);

    if (horizontal)
    {
        std::ranges::sort(align_data,
                          [](const ObjectAlignData& lhs, const ObjectAlignData& rhs)
                          { return lhs.center_x < rhs.center_x; });
    }
    else
    {
        std::ranges::sort(align_data,
                          [](const ObjectAlignData& lhs, const ObjectAlignData& rhs)
                          { return lhs.center_y < rhs.center_y; });
    }

    size_t moved = 0;
    const auto obj_count = align_data.size();

    if (horizontal)
    {
        const double first_x = align_data.front().center_x;
        const double last_x = align_data.back().center_x;
        const double step = (last_x - first_x) / static_cast<double>(obj_count - 1);

        for (size_t idx = 1; idx + 1 < obj_count; ++idx)
        {
            const double target_x = first_x + step * static_cast<double>(idx);
            if (std::abs(target_x - align_data[idx].center_x) > 0.01)
            {
                move_object_to(align_data[idx].obj_id, target_x, align_data[idx].center_y);
                ++moved;
            }
        }
    }
    else
    {
        const double first_y = align_data.front().center_y;
        const double last_y = align_data.back().center_y;
        const double step = (last_y - first_y) / static_cast<double>(obj_count - 1);

        for (size_t idx = 1; idx + 1 < obj_count; ++idx)
        {
            const double target_y = first_y + step * static_cast<double>(idx);
            if (std::abs(target_y - align_data[idx].center_y) > 0.01)
            {
                move_object_to(align_data[idx].obj_id, align_data[idx].center_x, target_y);
                ++moved;
            }
        }
    }

    return {moved > 0, moved};
}

auto CanvasObjectAligner::distribute_with_spacing(const std::vector<ObjectId>& ids,
                                                  DistributeMode mode,
                                                  double spacing) -> AlignResult
{
    if (ids.size() < 2)
    {
        return {false, 0};
    }

    auto align_data = collect_align_data(ids);
    const bool horizontal =
        (mode == DistributeMode::kHorizontal || mode == DistributeMode::kEqualSpacingH);

    if (horizontal)
    {
        std::ranges::sort(align_data,
                          [](const ObjectAlignData& lhs, const ObjectAlignData& rhs)
                          { return lhs.left < rhs.left; });
    }
    else
    {
        std::ranges::sort(align_data,
                          [](const ObjectAlignData& lhs, const ObjectAlignData& rhs)
                          { return lhs.top < rhs.top; });
    }

    size_t moved = 0;
    double current_pos = horizontal ? align_data.front().left : align_data.front().top;

    for (auto& entry : align_data)
    {
        double new_center = 0.0;
        if (horizontal)
        {
            new_center = current_pos + entry.width / 2.0;
            if (std::abs(new_center - entry.center_x) > 0.01)
            {
                move_object_to(entry.obj_id, new_center, entry.center_y);
                ++moved;
            }
            current_pos += entry.width + spacing;
        }
        else
        {
            new_center = current_pos + entry.height / 2.0;
            if (std::abs(new_center - entry.center_y) > 0.01)
            {
                move_object_to(entry.obj_id, entry.center_x, new_center);
                ++moved;
            }
            current_pos += entry.height + spacing;
        }
    }

    return {moved > 0, moved};
}

// ── Smart Suggestions ─────────────────────────────────────────────

auto CanvasObjectAligner::suggest_alignments(const std::vector<ObjectId>& ids,
                                             double tolerance) const
    -> std::vector<AlignmentSuggestion>
{
    const auto align_data = collect_align_data(ids);
    if (align_data.size() < 2)
    {
        return {};
    }

    std::vector<AlignmentSuggestion> suggestions;

    // Check each alignment axis
    auto check_axis = [&](AlignDirection dir, auto getter)
    {
        double total = 0.0;
        for (const auto& entry : align_data)
        {
            total += getter(entry);
        }
        const double avg = total / static_cast<double>(align_data.size());

        double max_dev = 0.0;
        size_t near_count = 0;
        for (const auto& entry : align_data)
        {
            const double dev = std::abs(getter(entry) - avg);
            max_dev = std::max(max_dev, dev);
            if (dev <= tolerance)
            {
                ++near_count;
            }
        }

        if (near_count >= 2 && max_dev <= tolerance * 3.0)
        {
            suggestions.push_back({dir, avg, near_count, max_dev});
        }
    };

    check_axis(AlignDirection::kLeft, [](const ObjectAlignData& entry) { return entry.left; });
    check_axis(AlignDirection::kRight, [](const ObjectAlignData& entry) { return entry.right; });
    check_axis(AlignDirection::kTop, [](const ObjectAlignData& entry) { return entry.top; });
    check_axis(AlignDirection::kBottom, [](const ObjectAlignData& entry) { return entry.bottom; });
    check_axis(AlignDirection::kCenterHorizontal,
               [](const ObjectAlignData& entry) { return entry.center_x; });
    check_axis(AlignDirection::kCenterVertical,
               [](const ObjectAlignData& entry) { return entry.center_y; });

    return suggestions;
}

// ── Utility ───────────────────────────────────────────────────────

auto CanvasObjectAligner::direction_name(AlignDirection direction) -> std::string
{
    switch (direction)
    {
        case AlignDirection::kLeft:
            return "Align Left";
        case AlignDirection::kRight:
            return "Align Right";
        case AlignDirection::kTop:
            return "Align Top";
        case AlignDirection::kBottom:
            return "Align Bottom";
        case AlignDirection::kCenterHorizontal:
            return "Center Horizontally";
        case AlignDirection::kCenterVertical:
            return "Center Vertically";
    }
    return "Unknown";
}

auto CanvasObjectAligner::distribute_mode_name(DistributeMode mode) -> std::string
{
    switch (mode)
    {
        case DistributeMode::kHorizontal:
            return "Distribute Horizontally";
        case DistributeMode::kVertical:
            return "Distribute Vertically";
        case DistributeMode::kEqualSpacingH:
            return "Equal Spacing Horizontal";
        case DistributeMode::kEqualSpacingV:
            return "Equal Spacing Vertical";
    }
    return "Unknown";
}

// ── Private Helpers ───────────────────────────────────────────────

auto CanvasObjectAligner::collect_align_data(const std::vector<ObjectId>& ids) const
    -> std::vector<ObjectAlignData>
{
    std::vector<ObjectAlignData> result;
    result.reserve(ids.size());

    for (const auto obj_id : ids)
    {
        const auto* obj = board_.get_object(obj_id);
        if (obj == nullptr)
        {
            continue;
        }

        const auto bounds = obj->world_bounds();
        ObjectAlignData entry{};
        entry.obj_id = obj_id;
        entry.center_x = bounds.center().x;
        entry.center_y = bounds.center().y;
        entry.width = bounds.width();
        entry.height = bounds.height();
        entry.left = bounds.min_x;
        entry.right = bounds.max_x;
        entry.top = bounds.min_y;
        entry.bottom = bounds.max_y;
        result.push_back(entry);
    }

    return result;
}

auto CanvasObjectAligner::move_object_to(ObjectId obj_id, double new_x, double new_y) -> void
{
    auto* obj = board_.get_object_mut(obj_id);
    if (obj != nullptr)
    {
        auto xform = obj->transform();
        xform.tx = new_x;
        xform.ty = new_y;
        obj->set_transform(xform);
    }
}

// (#82) Compute combined bounding box of a set of objects.
auto CanvasObjectAligner::compute_bounds(const std::vector<ObjectId>& ids) const -> AABB
{
    const auto data = collect_align_data(ids);
    if (data.empty())
    {
        return {0.0, 0.0, 0.0, 0.0};
    }
    double min_x = data.front().left;
    double min_y = data.front().top;
    double max_x = data.front().right;
    double max_y = data.front().bottom;
    for (const auto& entry : data)
    {
        min_x = std::min(min_x, entry.left);
        min_y = std::min(min_y, entry.top);
        max_x = std::max(max_x, entry.right);
        max_y = std::max(max_y, entry.bottom);
    }
    return {min_x, min_y, max_x, max_y};
}

} // namespace markamp::canvas
