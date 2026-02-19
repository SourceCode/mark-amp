#include "BreadcrumbModel.h"

#include <algorithm>

namespace markamp::ui
{

void BreadcrumbModel::set_path(std::vector<BreadcrumbSegment> segments)
{
    segments_ = std::move(segments);
}

auto BreadcrumbModel::segments() const -> const std::vector<BreadcrumbSegment>&
{
    return segments_;
}

auto BreadcrumbModel::segment_count() const -> int
{
    return static_cast<int>(segments_.size());
}

void BreadcrumbModel::set_active(int index)
{
    for (int idx = 0; idx < segment_count(); ++idx)
    {
        segments_[static_cast<std::size_t>(idx)].state =
            (idx == index) ? BreadcrumbSegmentState::kActive : BreadcrumbSegmentState::kNormal;
    }
}

void BreadcrumbModel::set_hovered(int index)
{
    if (index >= 0 && index < segment_count())
    {
        auto& seg = segments_[static_cast<std::size_t>(index)];
        if (seg.state != BreadcrumbSegmentState::kActive)
        {
            seg.state = BreadcrumbSegmentState::kHovered;
        }
    }
}

void BreadcrumbModel::clear_hover()
{
    for (auto& seg : segments_)
    {
        if (seg.state == BreadcrumbSegmentState::kHovered)
        {
            seg.state = BreadcrumbSegmentState::kNormal;
        }
    }
}

auto BreadcrumbModel::visible_segments(int max_visible) const -> std::vector<BreadcrumbSegment>
{
    if (max_visible <= 0 || segment_count() <= max_visible)
    {
        return segments_;
    }

    // Middle-truncation: first segment + ellipsis marker + last (max_visible - 2)
    std::vector<BreadcrumbSegment> result;
    result.push_back(segments_.front());

    // Add ellipsis marker
    BreadcrumbSegment ellipsis;
    ellipsis.segment_id = "...";
    ellipsis.label = "…";
    result.push_back(ellipsis);

    // Add last segments to fill remaining slots
    const int tail_count = max_visible - 2;
    const int start_tail = segment_count() - tail_count;
    for (int idx = start_tail; idx < segment_count(); ++idx)
    {
        result.push_back(segments_[static_cast<std::size_t>(idx)]);
    }

    return result;
}

auto BreadcrumbModel::needs_truncation(int max_visible) const -> bool
{
    return segment_count() > max_visible;
}

void BreadcrumbModel::navigate_to(int segment_index)
{
    if (segment_index < 0 || segment_index >= segment_count())
    {
        return;
    }

    // Truncate forward history if we navigated from the middle
    if (history_position_ >= 0 && history_position_ < static_cast<int>(history_.size()) - 1)
    {
        history_.resize(static_cast<std::size_t>(history_position_ + 1));
    }

    history_.push_back(segment_index);
    history_position_ = static_cast<int>(history_.size()) - 1;
    set_active(segment_index);
}

auto BreadcrumbModel::go_back() -> bool
{
    if (!can_go_back())
    {
        return false;
    }
    --history_position_;
    set_active(history_[static_cast<std::size_t>(history_position_)]);
    return true;
}

auto BreadcrumbModel::go_forward() -> bool
{
    if (!can_go_forward())
    {
        return false;
    }
    ++history_position_;
    set_active(history_[static_cast<std::size_t>(history_position_)]);
    return true;
}

auto BreadcrumbModel::can_go_back() const -> bool
{
    return history_position_ > 0;
}

auto BreadcrumbModel::can_go_forward() const -> bool
{
    return history_position_ >= 0 && history_position_ < static_cast<int>(history_.size()) - 1;
}

} // namespace markamp::ui
