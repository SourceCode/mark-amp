#include "FillEffectsController.h"

#include <algorithm>

namespace markamp::canvas
{

auto FillSpec::type_name() const -> std::string
{
    switch (type)
    {
        case FillType::kNone:
            return "none";
        case FillType::kSolid:
            return "solid";
        case FillType::kLinearGradient:
            return "linear_gradient";
        case FillType::kRadialGradient:
            return "radial_gradient";
    }
    return "unknown";
}

void FillEffectsController::set_fill(const FillSpec& fill)
{
    fill_ = fill;
}

auto FillEffectsController::fill() const -> const FillSpec&
{
    return fill_;
}

void FillEffectsController::set_shadow(const ShadowSpec& shadow)
{
    shadow_ = shadow;
}

auto FillEffectsController::shadow() const -> const ShadowSpec&
{
    return shadow_;
}

void FillEffectsController::set_shadow_enabled(bool enabled)
{
    shadow_.enabled = enabled;
}

auto FillEffectsController::shadow_enabled() const -> bool
{
    return shadow_.enabled;
}

void FillEffectsController::set_opacity(double opacity)
{
    opacity_ = std::clamp(opacity, 0.0, 1.0);
}

auto FillEffectsController::opacity() const -> double
{
    return opacity_;
}

auto FillEffectsController::is_fill_visible() const -> bool
{
    return fill_.type != FillType::kNone && opacity_ > 0.0;
}

auto FillEffectsController::has_effects() const -> bool
{
    return shadow_.enabled || fill_.type == FillType::kLinearGradient ||
           fill_.type == FillType::kRadialGradient;
}

auto FillEffectsController::compute_gradient_color(double position) const -> uint32_t
{
    // Clamp position to [0, 1]
    position = std::clamp(position, 0.0, 1.0);

    // Interpolate between gradient_start and gradient_end (RGBA packed as uint32).
    const auto start_r = static_cast<uint8_t>((fill_.gradient_start.color_rgba >> 24U) & 0xFFU);
    const auto start_g = static_cast<uint8_t>((fill_.gradient_start.color_rgba >> 16U) & 0xFFU);
    const auto start_b = static_cast<uint8_t>((fill_.gradient_start.color_rgba >> 8U) & 0xFFU);
    const auto start_a = static_cast<uint8_t>(fill_.gradient_start.color_rgba & 0xFFU);

    const auto end_r = static_cast<uint8_t>((fill_.gradient_end.color_rgba >> 24U) & 0xFFU);
    const auto end_g = static_cast<uint8_t>((fill_.gradient_end.color_rgba >> 16U) & 0xFFU);
    const auto end_b = static_cast<uint8_t>((fill_.gradient_end.color_rgba >> 8U) & 0xFFU);
    const auto end_a = static_cast<uint8_t>(fill_.gradient_end.color_rgba & 0xFFU);

    const auto mixed_r = static_cast<uint8_t>(
        start_r + position * (static_cast<double>(end_r) - static_cast<double>(start_r)));
    const auto mixed_g = static_cast<uint8_t>(
        start_g + position * (static_cast<double>(end_g) - static_cast<double>(start_g)));
    const auto mixed_b = static_cast<uint8_t>(
        start_b + position * (static_cast<double>(end_b) - static_cast<double>(start_b)));
    const auto mixed_a = static_cast<uint8_t>(
        start_a + position * (static_cast<double>(end_a) - static_cast<double>(start_a)));

    return (static_cast<uint32_t>(mixed_r) << 24U) |
           (static_cast<uint32_t>(mixed_g) << 16U) |
           (static_cast<uint32_t>(mixed_b) << 8U) |
           static_cast<uint32_t>(mixed_a);
}

} // namespace markamp::canvas
