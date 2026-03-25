/// @file V27AccessibilityDesignCoordinator.h
/// @brief V27 Phase 18 — Accessibility visuals redesign coordinator.
#pragma once
#include <string>
#include <vector>
namespace markamp::core
{
struct AccessibilitySurfacePolicy {
    std::string name;
    bool has_focus_ring{false};
    bool meets_contrast{false};
    bool has_reduced_motion{false};
    bool has_non_color_cues{false};
};
class V27AccessibilityDesignCoordinator {
public:
    V27AccessibilityDesignCoordinator() = default;
    void register_surface(const AccessibilitySurfacePolicy& policy);
    [[nodiscard]] auto surface_count() const noexcept -> int;
    [[nodiscard]] auto focus_ring_count() const noexcept -> int;
    [[nodiscard]] auto contrast_pass_count() const noexcept -> int;
    [[nodiscard]] auto reduced_motion_count() const noexcept -> int;
    [[nodiscard]] auto non_color_cue_count() const noexcept -> int;
    [[nodiscard]] auto all_meet_criteria() const noexcept -> bool;
    [[nodiscard]] auto summary() const -> std::string;
private:
    std::vector<AccessibilitySurfacePolicy> surfaces_;
};
} // namespace markamp::core
