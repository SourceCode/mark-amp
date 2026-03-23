/// @file V27MotionSystem.h
/// @brief V27 Phase 19 — Motion, micro-interactions, icon-driven controls.
#pragma once
#include <cstdint>
namespace markamp::ui
{
enum class V27MotionTier { kInstant, kMicro, kFast, kStandard, kGentle };
[[nodiscard]] constexpr auto v27_motion_tier_count() noexcept -> int { return 5; }
[[nodiscard]] constexpr auto v27_motion_tier_ms(V27MotionTier tier) noexcept -> int {
    switch (tier) {
    case V27MotionTier::kInstant:  return 0;
    case V27MotionTier::kMicro:   return 50;
    case V27MotionTier::kFast:    return 100;
    case V27MotionTier::kStandard:return 200;
    case V27MotionTier::kGentle:  return 350;
    }
    return 200;
}
enum class V27EasingCurve { kLinear, kEaseOut, kEaseInOut, kSpring, kBounce };
[[nodiscard]] constexpr auto v27_easing_curve_count() noexcept -> int { return 5; }
struct V27MicroInteractionTokens {
    static constexpr int kHoverScaleMs = 80;
    static constexpr int kPressScaleMs = 40;
    static constexpr int kDragStartMs = 100;
    static constexpr int kResizeMs = 0;
    static constexpr int kFocusTransferMs = 100;
    static constexpr int kMenuOpenMs = 150;
    static constexpr int kMenuCloseMs = 100;
    static constexpr int kTabSwitchMs = 100;
    static constexpr int kPanelToggleMs = 200;
    static constexpr int kDialogOpenMs = 200;
    static constexpr int kDialogCloseMs = 150;
};
struct V27IconControlTokens {
    static constexpr int kMinLabelWidth = 0;     ///< 0 = icon-only allowed
    static constexpr int kIconOnlySize = 28;      ///< Icon-only control size
    static constexpr int kIconLabelGap = 6;       ///< Gap between icon and label
    static constexpr int kBadgeOffsetX = -4;      ///< Badge X offset (from top-right)
    static constexpr int kBadgeOffsetY = -4;      ///< Badge Y offset
    static constexpr int kChipIconSize = 12;      ///< Icon in chip/tag
    static constexpr int kChipCloseSize = 10;     ///< Close button in chip
};
} // namespace markamp::ui
