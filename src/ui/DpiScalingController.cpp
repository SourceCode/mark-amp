#include "DpiScalingController.h"

#include <algorithm>
#include <cmath>

namespace markamp::ui
{

void ScaledMetric::compute()
{
    scaled_value = static_cast<int>(std::round(base_value * scale_factor));
}

void DpiScalingController::set_scale_factor(double factor)
{
    scale_factor_ = std::clamp(factor, kMinScale, kMaxScale);
}

auto DpiScalingController::scale_factor() const -> double
{
    return scale_factor_;
}

void DpiScalingController::set_platform(Platform platform)
{
    platform_ = platform;
}

auto DpiScalingController::platform() const -> Platform
{
    return platform_;
}

auto DpiScalingController::platform_default_scale() const -> double
{
    switch (platform_)
    {
        case Platform::kMacOS:
            return 2.0; // Retina default
        case Platform::kWindows:
            return 1.5; // Common Windows HiDPI
        case Platform::kLinux:
            return 1.0; // Most Linux default
    }
    return 1.0;
}

auto DpiScalingController::scale_px(int base_px) const -> int
{
    return static_cast<int>(std::round(base_px * scale_factor_));
}

auto DpiScalingController::scale_font(double base_pt) const -> double
{
    return base_pt * scale_factor_;
}

auto DpiScalingController::scale_padding(int base_px) const -> int
{
    return static_cast<int>(std::round(base_px * scale_factor_));
}

auto DpiScalingController::scale_icon(int base_px) const -> int
{
    return static_cast<int>(std::round(base_px * scale_factor_));
}

auto DpiScalingController::metric(int base_value) const -> ScaledMetric
{
    ScaledMetric result;
    result.base_value = base_value;
    result.scale_factor = scale_factor_;
    result.compute();
    return result;
}

auto DpiScalingController::on_dpi_changed(double new_factor) -> bool
{
    double old_factor = scale_factor_;
    set_scale_factor(new_factor);
    if (std::abs(scale_factor_ - old_factor) > 0.001)
    {
        ++dpi_change_count_;
        return true;
    }
    return false;
}

auto DpiScalingController::dpi_change_count() const -> int
{
    return dpi_change_count_;
}

} // namespace markamp::ui
