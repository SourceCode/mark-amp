/// @file V27OnboardingDesignCoordinator.h
/// @brief V27 Phase 16 — Onboarding surface redesign coordinator.
#pragma once
#include <string>
#include <vector>
namespace markamp::core
{
enum class OnboardingSurfaceType { kStartup, kEmptyState, kHint };
struct OnboardingSurfacePolicy {
    std::string name;
    OnboardingSurfaceType type{OnboardingSurfaceType::kStartup};
    bool has_canonical_icons{false};
    bool is_redesigned{false};
};
class V27OnboardingDesignCoordinator {
public:
    V27OnboardingDesignCoordinator() = default;
    void register_surface(const OnboardingSurfacePolicy& policy);
    [[nodiscard]] auto surface_count() const noexcept -> int;
    [[nodiscard]] auto redesigned_count() const noexcept -> int;
    [[nodiscard]] auto canonical_icon_count() const noexcept -> int;
    [[nodiscard]] auto count_by_type(OnboardingSurfaceType type) const noexcept -> int;
    [[nodiscard]] auto coverage_pct() const noexcept -> int;
    [[nodiscard]] auto summary() const -> std::string;
private:
    std::vector<OnboardingSurfacePolicy> surfaces_;
};
} // namespace markamp::core
