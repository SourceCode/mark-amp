/// @file V27AccessibilityDesignCoordinator.cpp
#include "core/V27AccessibilityDesignCoordinator.h"
#include <sstream>
#include <algorithm>
namespace markamp::core
{
void V27AccessibilityDesignCoordinator::register_surface(const AccessibilitySurfacePolicy& policy) {
    surfaces_.push_back(policy);
}
auto V27AccessibilityDesignCoordinator::surface_count() const noexcept -> int {
    return static_cast<int>(surfaces_.size());
}
auto V27AccessibilityDesignCoordinator::focus_ring_count() const noexcept -> int {
    return static_cast<int>(std::ranges::count_if(surfaces_,
        [](const auto& surf) { return surf.has_focus_ring; }));
}
auto V27AccessibilityDesignCoordinator::contrast_pass_count() const noexcept -> int {
    return static_cast<int>(std::ranges::count_if(surfaces_,
        [](const auto& surf) { return surf.meets_contrast; }));
}
auto V27AccessibilityDesignCoordinator::reduced_motion_count() const noexcept -> int {
    return static_cast<int>(std::ranges::count_if(surfaces_,
        [](const auto& surf) { return surf.has_reduced_motion; }));
}
auto V27AccessibilityDesignCoordinator::non_color_cue_count() const noexcept -> int {
    return static_cast<int>(std::ranges::count_if(surfaces_,
        [](const auto& surf) { return surf.has_non_color_cues; }));
}
auto V27AccessibilityDesignCoordinator::all_meet_criteria() const noexcept -> bool {
    return std::ranges::all_of(surfaces_, [](const auto& surf) {
        return surf.has_focus_ring && surf.meets_contrast
            && surf.has_reduced_motion && surf.has_non_color_cues;
    });
}
auto V27AccessibilityDesignCoordinator::summary() const -> std::string {
    std::ostringstream out;
    out << "Accessibility Design Status\n";
    out << "  Surfaces: " << surface_count() << "\n";
    out << "  Focus rings: " << focus_ring_count() << "\n";
    out << "  Contrast pass: " << contrast_pass_count() << "\n";
    out << "  Reduced motion: " << reduced_motion_count() << "\n";
    out << "  All criteria met: " << (all_meet_criteria() ? "Yes" : "No") << "\n";
    return out.str();
}
} // namespace markamp::core
