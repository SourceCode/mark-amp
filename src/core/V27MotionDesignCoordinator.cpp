/// @file V27MotionDesignCoordinator.cpp
#include "core/V27MotionDesignCoordinator.h"
#include <sstream>
#include <algorithm>
namespace markamp::core
{
void V27MotionDesignCoordinator::register_surface(const MotionSurfacePolicy& policy) {
    surfaces_.push_back(policy);
}
auto V27MotionDesignCoordinator::surface_count() const noexcept -> int {
    return static_cast<int>(surfaces_.size());
}
auto V27MotionDesignCoordinator::reduced_motion_count() const noexcept -> int {
    return static_cast<int>(std::ranges::count_if(surfaces_,
        [](const auto& surf) { return surf.has_reduced_motion_fallback; }));
}
auto V27MotionDesignCoordinator::count_by_tier(markamp::ui::V27MotionTier tier) const noexcept -> int {
    return static_cast<int>(std::ranges::count_if(surfaces_,
        [tier](const auto& surf) { return surf.tier == tier; }));
}
auto V27MotionDesignCoordinator::all_have_fallback() const noexcept -> bool {
    return std::ranges::all_of(surfaces_,
        [](const auto& surf) { return surf.has_reduced_motion_fallback; });
}
auto V27MotionDesignCoordinator::summary() const -> std::string {
    std::ostringstream out;
    out << "Motion Design Status\n";
    out << "  Surfaces: " << surface_count() << "\n";
    out << "  Reduced motion fallbacks: " << reduced_motion_count() << "\n";
    out << "  All have fallback: " << (all_have_fallback() ? "Yes" : "No") << "\n";
    return out.str();
}
} // namespace markamp::core
