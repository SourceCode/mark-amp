/// @file V27MotionDesignCoordinator.h
/// @brief V27 Phase 19 — Motion and micro-interaction redesign coordinator.
#pragma once
#include <string>
#include <vector>
#include "ui/V27MotionSystem.h"
namespace markamp::core
{
struct MotionSurfacePolicy {
    std::string name;
    markamp::ui::V27MotionTier tier{markamp::ui::V27MotionTier::kStandard};
    markamp::ui::V27EasingCurve easing{markamp::ui::V27EasingCurve::kEaseOut};
    bool has_reduced_motion_fallback{false};
};
class V27MotionDesignCoordinator {
public:
    V27MotionDesignCoordinator() = default;
    void register_surface(const MotionSurfacePolicy& policy);
    [[nodiscard]] auto surface_count() const noexcept -> int;
    [[nodiscard]] auto reduced_motion_count() const noexcept -> int;
    [[nodiscard]] auto count_by_tier(markamp::ui::V27MotionTier tier) const noexcept -> int;
    [[nodiscard]] auto all_have_fallback() const noexcept -> bool;
    [[nodiscard]] auto summary() const -> std::string;
private:
    std::vector<MotionSurfacePolicy> surfaces_;
};
} // namespace markamp::core
