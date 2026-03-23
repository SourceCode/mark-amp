/// @file V26DialogOverlayConvergence.h
/// @brief V26 Phase 11: Dialogs, popovers, tooltips, notifications & overlay convergence.
///
/// Defines premium presentation metrics for modal dialogs, popovers, tooltips,
/// toast notifications, and all overlay UI layers.
#pragma once

#include <cstdint>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 11: Dialogs, Popovers, Tooltips, Notifications & Overlays
// ═══════════════════════════════════════════════════════════════════════════════

/// Modal dialog shell metrics.
struct DialogV26Metrics
{
    // ── Sizing ───────────────────────────────────────────────────────
    static constexpr int kMinWidth             = 360;
    static constexpr int kMaxWidth             = 600;
    static constexpr int kMinHeight            = 200;
    static constexpr int kHeaderHeight         = 48;
    static constexpr int kFooterHeight         = 52;

    // ── Chrome ───────────────────────────────────────────────────────
    static constexpr int kBorderRadius         = 8;
    static constexpr int kPaddingH             = 24;
    static constexpr int kPaddingV             = 20;
    static constexpr int kShadowBlur           = 24;
    static constexpr int kShadowOffsetY        = 8;
    static constexpr float kBackdropAlpha      = 0.45F;

    // ── Typography ───────────────────────────────────────────────────
    static constexpr int kTitleFontPt          = 16;
    static constexpr int kBodyFontPt           = 13;

    // ── Footer buttons ───────────────────────────────────────────────
    static constexpr int kButtonHeight         = 32;
    static constexpr int kButtonPaddingH       = 16;
    static constexpr int kButtonGap            = 8;
    static constexpr int kButtonRadius         = 4;
};

/// Popover metrics (lightweight floating panels attached to anchors).
struct PopoverV26Metrics
{
    static constexpr int kMaxWidth             = 320;
    static constexpr int kMaxHeight            = 400;
    static constexpr int kPaddingH             = 12;
    static constexpr int kPaddingV             = 8;
    static constexpr int kBorderRadius         = 6;
    static constexpr int kShadowBlur           = 12;
    static constexpr int kArrowSize            = 8;
    static constexpr int kAnchorGap            = 4;    ///< Gap between anchor and popover
    static constexpr int kBorderW              = 1;
};

/// Tooltip metrics.
struct TooltipV26Metrics
{
    static constexpr int kMaxWidth             = 280;
    static constexpr int kPaddingH             = 8;
    static constexpr int kPaddingV             = 4;
    static constexpr int kBorderRadius         = 4;
    static constexpr int kFontPt               = 11;
    static constexpr int kDelayMs              = 500;  ///< Show delay
    static constexpr int kFadeMs               = 150;  ///< Fade-in duration
    static constexpr float kOpacity            = 0.95F;
};

/// Toast notification metrics.
struct ToastV26Metrics
{
    static constexpr int kWidth                = 360;
    static constexpr int kMinHeight            = 48;
    static constexpr int kPaddingH             = 16;
    static constexpr int kPaddingV             = 12;
    static constexpr int kBorderRadius         = 6;
    static constexpr int kIconSize             = 20;
    static constexpr int kIconTextGap          = 10;
    static constexpr int kActionGap            = 8;
    static constexpr int kShadowBlur           = 8;
    static constexpr int kDismissMs            = 5000;  ///< Auto-dismiss delay
    static constexpr int kSlideMs              = 200;   ///< Slide-in duration
    static constexpr int kStackGap             = 8;     ///< Gap between stacked toasts
    static constexpr int kMaxVisible           = 3;     ///< Max simultaneous toasts
};

} // namespace markamp::ui
