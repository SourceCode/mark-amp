#include "ConnectorModel.h"

namespace markamp::canvas
{

void ConnectorModel::set_source(AnchorPoint anchor)
{
    source_ = std::move(anchor);
}
void ConnectorModel::set_target(AnchorPoint anchor)
{
    target_ = std::move(anchor);
}
auto ConnectorModel::source() const -> const AnchorPoint&
{
    return source_;
}
auto ConnectorModel::target() const -> const AnchorPoint&
{
    return target_;
}

auto ConnectorModel::is_attached() const -> bool
{
    return !source_.object_id.empty() && !target_.object_id.empty();
}

void ConnectorModel::set_routing(RoutingMode mode)
{
    routing_ = mode;
}
auto ConnectorModel::routing() const -> RoutingMode
{
    return routing_;
}

void ConnectorModel::add_bend(BendPoint point)
{
    bends_.push_back(point);
}

void ConnectorModel::remove_bend(int index)
{
    if (index >= 0 && index < static_cast<int>(bends_.size()))
    {
        bends_.erase(bends_.begin() + index);
    }
}

void ConnectorModel::clear_bends()
{
    bends_.clear();
}
auto ConnectorModel::bends() const -> const std::vector<BendPoint>&
{
    return bends_;
}
auto ConnectorModel::bend_count() const -> int
{
    return static_cast<int>(bends_.size());
}

void ConnectorModel::set_label(ConnectorLabel label)
{
    label_ = std::move(label);
}
auto ConnectorModel::label() const -> const ConnectorLabel&
{
    return label_;
}
auto ConnectorModel::has_label() const -> bool
{
    return !label_.text.empty();
}

} // namespace markamp::canvas
