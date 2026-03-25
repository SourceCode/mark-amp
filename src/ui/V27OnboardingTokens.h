/// @file V27OnboardingTokens.h
/// @brief V27 Phase 16 — Startup, welcome, onboarding, empty states.
#pragma once
#include <cstdint>
namespace markamp::ui
{
struct V27StartupTokens {
    static constexpr int kMaxWidth = 600;
    static constexpr int kLogoSize = 48;
    static constexpr int kTitleGap = 24;
    static constexpr int kSectionGap = 32;
    static constexpr int kRecentItemHeight = 32;
    static constexpr int kRecentIconSize = 16;
    static constexpr int kRecentIconGap = 8;
    static constexpr int kActionButtonHeight = 36;
    static constexpr int kActionButtonGap = 8;
    static constexpr int kActionButtonPaddingH = 16;
};
enum class V27EmptyStateVariant { kFullPage, kPanel, kCompact };
[[nodiscard]] constexpr auto v27_empty_state_variant_count() noexcept -> int { return 3; }
struct V27EmptyStateTemplateTokens {
    static constexpr int kFullPageIconSize = 48;
    static constexpr int kPanelIconSize = 32;
    static constexpr int kCompactIconSize = 24;
    static constexpr int kTitleGap = 12;
    static constexpr int kBodyGap = 8;
    static constexpr int kActionGap = 20;
    static constexpr int kMaxWidth = 320;
};
struct V27OnboardingHintTokens {
    static constexpr int kHintMaxWidth = 280;
    static constexpr int kHintPaddingH = 12;
    static constexpr int kHintPaddingV = 8;
    static constexpr int kHintRadius = 6;
    static constexpr int kHintArrowSize = 6;
    static constexpr int kHintIconSize = 16;
    static constexpr int kHintIconGap = 8;
    static constexpr int kStepIndicatorSize = 8;
    static constexpr int kStepIndicatorGap = 4;
};
struct V27WelcomeAnimationTokens {
    static constexpr int kFadeInMs = 300;
    static constexpr int kStaggerDelayMs = 60;
    static constexpr int kLogoPulseMs = 800;
    static constexpr int kRecentListDelayMs = 150;
    static constexpr int kActionRevealMs = 200;
    static constexpr int kSkeletonShimmerMs = 1200;
};
} // namespace markamp::ui
