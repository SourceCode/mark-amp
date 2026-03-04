#include "CompactLayoutModel.h"

#include <algorithm>

namespace markamp::ui
{

auto LayoutRule::behavior_name() const -> std::string
{
    switch (compact_behavior)
    {
        case OverflowBehavior::kTruncate:
            return "truncate";
        case OverflowBehavior::kCollapse:
            return "collapse";
        case OverflowBehavior::kOverflow:
            return "overflow";
        case OverflowBehavior::kWrap:
            return "wrap";
    }
    return "unknown";
}

auto CompactLayoutModel::mode_for_width(int width_px) -> LayoutWidthMode
{
    if (width_px < 640)
    {
        return LayoutWidthMode::kCompact;
    }
    if (width_px > 1024)
    {
        return LayoutWidthMode::kWide;
    }
    return LayoutWidthMode::kNormal;
}

auto CompactLayoutModel::mode_name(LayoutWidthMode mode) -> std::string
{
    switch (mode)
    {
        case LayoutWidthMode::kCompact:
            return "compact";
        case LayoutWidthMode::kNormal:
            return "normal";
        case LayoutWidthMode::kWide:
            return "wide";
    }
    return "unknown";
}

void CompactLayoutModel::set_width(int width_px)
{
    current_width_ = width_px;
}

auto CompactLayoutModel::current_mode() const -> LayoutWidthMode
{
    if (current_width_ < compact_breakpoint_)
    {
        return LayoutWidthMode::kCompact;
    }
    if (current_width_ > wide_breakpoint_)
    {
        return LayoutWidthMode::kWide;
    }
    return LayoutWidthMode::kNormal;
}

auto CompactLayoutModel::current_width() const -> int
{
    return current_width_;
}

void CompactLayoutModel::set_compact_breakpoint(int width_px)
{
    compact_breakpoint_ = width_px;
}

void CompactLayoutModel::set_wide_breakpoint(int width_px)
{
    wide_breakpoint_ = width_px;
}

auto CompactLayoutModel::compact_breakpoint() const -> int
{
    return compact_breakpoint_;
}

auto CompactLayoutModel::wide_breakpoint() const -> int
{
    return wide_breakpoint_;
}

void CompactLayoutModel::add_rule(const LayoutRule& rule)
{
    rules_.push_back(rule);
}

auto CompactLayoutModel::all_rules() const -> const std::vector<LayoutRule>&
{
    return rules_;
}

auto CompactLayoutModel::rules_by_priority() const -> std::vector<LayoutRule>
{
    auto sorted = rules_;
    std::sort(sorted.begin(),
              sorted.end(),
              [](const LayoutRule& left, const LayoutRule& right)
              { return left.priority < right.priority; });
    return sorted;
}

auto CompactLayoutModel::visible_clusters() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    const auto mode = current_mode();
    for (const auto& rule : rules_)
    {
        if (mode == LayoutWidthMode::kCompact &&
            rule.compact_behavior == OverflowBehavior::kCollapse)
        {
            continue; // Hidden in compact
        }
        result.push_back(rule.cluster_id);
    }
    return result;
}

auto CompactLayoutModel::behavior_for(const std::string& cluster_id) const -> OverflowBehavior
{
    const auto mode = current_mode();
    for (const auto& rule : rules_)
    {
        if (rule.cluster_id == cluster_id)
        {
            if (mode == LayoutWidthMode::kCompact)
            {
                return rule.compact_behavior;
            }
            return OverflowBehavior::kTruncate; // Normal/wide: no overflow
        }
    }
    return OverflowBehavior::kTruncate;
}

auto CompactLayoutModel::rule_count() const -> int
{
    return static_cast<int>(rules_.size());
}

} // namespace markamp::ui
