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
    return shadow_.enabled;
}

} // namespace markamp::canvas
