#include "MinimapModel.h"

namespace markamp::canvas
{

void MinimapModel::set_viewport(double vp_x, double vp_y, double vp_w, double vp_h)
{
    vp_x_ = vp_x;
    vp_y_ = vp_y;
    vp_w_ = (vp_w > 0.0) ? vp_w : 1.0;
    vp_h_ = (vp_h > 0.0) ? vp_h : 1.0;
}

auto MinimapModel::viewport_x() const -> double
{
    return vp_x_;
}
auto MinimapModel::viewport_y() const -> double
{
    return vp_y_;
}
auto MinimapModel::viewport_w() const -> double
{
    return vp_w_;
}
auto MinimapModel::viewport_h() const -> double
{
    return vp_h_;
}

void MinimapModel::set_mode(MinimapMode mode)
{
    mode_ = mode;
}
auto MinimapModel::mode() const -> MinimapMode
{
    return mode_;
}

void MinimapModel::set_markers(std::vector<FrameMarker> markers)
{
    markers_ = std::move(markers);
}
auto MinimapModel::markers() const -> const std::vector<FrameMarker>&
{
    return markers_;
}

void MinimapModel::push_location()
{
    history_.push_back({vp_x_, vp_y_, vp_w_, vp_h_});
}

void MinimapModel::navigate_back()
{
    if (!history_.empty())
    {
        const auto& snap = history_.back();
        vp_x_ = snap.snap_x;
        vp_y_ = snap.snap_y;
        vp_w_ = snap.snap_w;
        vp_h_ = snap.snap_h;
        history_.pop_back();
    }
}

auto MinimapModel::history_depth() const -> int
{
    return static_cast<int>(history_.size());
}

} // namespace markamp::canvas
