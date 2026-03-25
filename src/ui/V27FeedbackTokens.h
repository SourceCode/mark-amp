/// @file V27FeedbackTokens.h
/// @brief V27 Phase 15 — Feedback, status, progress, validation, messaging.
#pragma once
#include <cstdint>
namespace markamp::ui
{
enum class V27FeedbackSeverity { kInfo, kSuccess, kWarning, kError };
[[nodiscard]] constexpr auto v27_feedback_severity_count() noexcept -> int { return 4; }
struct V27FeedbackSeverityTokens {
    static constexpr int kIconSize = 16;
    static constexpr int kInlinePaddingH = 8;
    static constexpr int kInlinePaddingV = 4;
    static constexpr int kInlineRadius = 4;
    static constexpr int kBorderWidth = 1;
};
struct V27StatusBarTokens {
    static constexpr int kHeight = 22;
    static constexpr int kItemPaddingH = 6;
    static constexpr int kItemGap = 2;
    static constexpr int kIconSize = 14;
    static constexpr int kBadgeSize = 16;
    static constexpr int kSpinnerSize = 14;
    static constexpr int kProgressHeight = 2;
};
struct V27BannerTokens {
    static constexpr int kHeight = 32;
    static constexpr int kPaddingH = 12;
    static constexpr int kIconSize = 16;
    static constexpr int kIconGap = 8;
    static constexpr int kActionGap = 12;
    static constexpr int kActionButtonHeight = 24;
    static constexpr int kProgressBarHeight = 3;
    static constexpr int kProgressBarRadius = 2;
};
struct V27ProgressTokens {
    static constexpr int kBarHeight = 4;
    static constexpr int kBarRadius = 2;
    static constexpr int kIndeterminatePulseWidth = 60;
    static constexpr int kIndeterminateCycleMs = 1200;
    static constexpr int kLabelGap = 8;
    static constexpr int kCircularDiameter = 20;
    static constexpr int kCircularStroke = 2;
};
struct V27InlineValidationTokens {
    static constexpr int kIconSize = 14;
    static constexpr int kIconGap = 4;
    static constexpr int kMessagePaddingH = 8;
    static constexpr int kMessagePaddingV = 4;
    static constexpr int kFieldUnderlineWidth = 2;
    static constexpr int kFieldRadius = 4;
    static constexpr int kShakeAmplitude = 4;
    static constexpr int kShakeDurationMs = 300;
};
} // namespace markamp::ui
