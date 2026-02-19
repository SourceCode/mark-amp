#include "ResponsiveModel.h"

#include <cmath>

namespace markamp::ui
{

void ResponsiveModel::set_scale(ScaleInfo info)
{
    scale_ = info;
}
auto ResponsiveModel::scale() const -> const ScaleInfo&
{
    return scale_;
}

auto ResponsiveModel::effective_padding() const -> int
{
    return static_cast<int>(std::round(scale_.base_padding * scale_.scale_factor));
}

auto ResponsiveModel::effective_icon_size() const -> int
{
    return static_cast<int>(std::round(scale_.base_icon_size * scale_.scale_factor));
}

void ResponsiveModel::set_window_width(int pixels)
{
    window_width_ = pixels;
}
auto ResponsiveModel::window_width() const -> int
{
    return window_width_;
}

void ResponsiveModel::set_compact_threshold(int pixels)
{
    compact_threshold_ = pixels;
}
auto ResponsiveModel::compact_threshold() const -> int
{
    return compact_threshold_;
}

auto ResponsiveModel::is_compact() const -> bool
{
    return window_width_ < compact_threshold_;
}

void ResponsiveModel::set_toolbar_item_count(int count)
{
    toolbar_item_count_ = (count < 0) ? 0 : count;
}

auto ResponsiveModel::visible_toolbar_items() const -> int
{
    if (!is_compact())
    {
        return toolbar_item_count_;
    }
    // In compact mode, show roughly half the items (minimum 3)
    const int half = toolbar_item_count_ / 2;
    return (half < 3 && toolbar_item_count_ >= 3) ? 3 : half;
}

auto ResponsiveModel::overflow_count() const -> int
{
    return toolbar_item_count_ - visible_toolbar_items();
}

void ResponsiveModel::set_platform(Platform platform)
{
    platform_ = platform;
}
auto ResponsiveModel::platform() const -> Platform
{
    return platform_;
}

auto ResponsiveModel::close_button_side() const -> std::string
{
    return (platform_ == Platform::kMacOS) ? "left" : "right";
}

auto ResponsiveModel::modifier_label() const -> std::string
{
    return (platform_ == Platform::kMacOS) ? "⌘" : "Ctrl";
}

} // namespace markamp::ui
