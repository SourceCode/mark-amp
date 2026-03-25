/// @file V27OnboardingDesignCoordinator.cpp
#include "core/V27OnboardingDesignCoordinator.h"
#include <sstream>
#include <algorithm>
namespace markamp::core
{
void V27OnboardingDesignCoordinator::register_surface(const OnboardingSurfacePolicy& policy) {
    surfaces_.push_back(policy);
}
auto V27OnboardingDesignCoordinator::surface_count() const noexcept -> int {
    return static_cast<int>(surfaces_.size());
}
auto V27OnboardingDesignCoordinator::redesigned_count() const noexcept -> int {
    return static_cast<int>(std::ranges::count_if(surfaces_,
        [](const auto& surf) { return surf.is_redesigned; }));
}
auto V27OnboardingDesignCoordinator::canonical_icon_count() const noexcept -> int {
    return static_cast<int>(std::ranges::count_if(surfaces_,
        [](const auto& surf) { return surf.has_canonical_icons; }));
}
auto V27OnboardingDesignCoordinator::count_by_type(OnboardingSurfaceType type) const noexcept -> int {
    return static_cast<int>(std::ranges::count_if(surfaces_,
        [type](const auto& surf) { return surf.type == type; }));
}
auto V27OnboardingDesignCoordinator::coverage_pct() const noexcept -> int {
    if (surfaces_.empty()) return 0;
    return (redesigned_count() * 100) / surface_count();
}
auto V27OnboardingDesignCoordinator::summary() const -> std::string {
    std::ostringstream out;
    out << "Onboarding Design Status\n";
    out << "  Surfaces: " << surface_count() << "\n";
    out << "  Redesigned: " << redesigned_count() << "\n";
    out << "  Coverage: " << coverage_pct() << "%\n";
    return out.str();
}
} // namespace markamp::core
