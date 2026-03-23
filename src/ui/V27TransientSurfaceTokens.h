/// @file V27TransientSurfaceTokens.h
/// @brief V27 Phase 13 — Dialogs, popovers, tooltips, notifications.
#pragma once
#include <cstdint>
namespace markamp::ui
{
struct V27ModalTokens {
    static constexpr int kMinWidth = 360;
    static constexpr int kMaxWidth = 600;
    static constexpr int kPaddingH = 24;
    static constexpr int kPaddingV = 20;
    static constexpr int kTitleGap = 12;
    static constexpr int kBodyGap = 16;
    static constexpr int kFooterGap = 20;
    static constexpr int kFooterButtonGap = 8;
    static constexpr int kRadius = 8;
    static constexpr int kShadowBlur = 16;
    static constexpr int kBackdropAlpha = 128;
};
struct V27TooltipTokens {
    static constexpr int kMaxWidth = 300;
    static constexpr int kPaddingH = 8;
    static constexpr int kPaddingV = 4;
    static constexpr int kRadius = 4;
    static constexpr int kAnchorOffset = 4;
    static constexpr int kArrowSize = 6;
    static constexpr int kDelayMs = 600;
    static constexpr int kFadeMs = 150;
};
struct V27NotificationTokens {
    static constexpr int kToastWidth = 360;
    static constexpr int kToastMinHeight = 40;
    static constexpr int kToastPaddingH = 12;
    static constexpr int kToastPaddingV = 8;
    static constexpr int kToastRadius = 6;
    static constexpr int kToastGap = 8;
    static constexpr int kToastIconSize = 16;
    static constexpr int kToastMaxVisible = 3;
    static constexpr int kToastDurationMs = 5000;
    static constexpr int kToastShadowBlur = 8;
};
} // namespace markamp::ui
