#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>

namespace markamp::core
{

/// V22 Phase 14: Transient overlay coordination service.
///
/// Provides standardized metrics for modal dialogs, popovers, notifications,
/// progress feedback, and transient teaching/spotlight overlays.
class TransientOverlayCoordinator
{
public:
    /// Notification level for severity-based presentation.
    enum class NotificationLevel : uint8_t
    {
        kInfo,
        kSuccess,
        kWarning,
        kError,
    };

    /// Modal dialog metrics.
    struct DialogMetrics
    {
        int min_width{400};
        int max_width{600};
        int header_height{44};
        int footer_height{52};
        int body_padding{20};
        int button_height{32};
        int button_min_width{80};
        int button_gap{8};
        float backdrop_opacity{0.5F};
        CornerRadiusToken corner{CornerRadiusToken::kLg};
        ElevationToken elevation{ElevationToken::kOverlay};
        TypeScaleToken title_font{TypeScaleToken::kSubtitle};
        TypeScaleToken body_font{TypeScaleToken::kBody};
    };

    /// Popover / anchored panel metrics.
    struct PopoverMetrics
    {
        int min_width{200};
        int max_width{360};
        int padding{12};
        int arrow_size{8};
        int offset_from_anchor{4};
        CornerRadiusToken corner{CornerRadiusToken::kMd};
        ElevationToken elevation{ElevationToken::kHigh};
        BorderWeightToken border{BorderWeightToken::kThin};
    };

    /// Notification toast metrics.
    struct NotificationMetrics
    {
        int width{360};
        int min_height{48};
        int max_height{120};
        int icon_size{20};
        int padding_h{12};
        int padding_v{8};
        int action_gap{8};
        int dismiss_delay_ms{5000};
        int slide_duration_ms{200};
        CornerRadiusToken corner{CornerRadiusToken::kMd};
        ElevationToken elevation{ElevationToken::kHigh};
        TypeScaleToken title_font{TypeScaleToken::kBody};
        TypeScaleToken message_font{TypeScaleToken::kCaption};
    };

    /// Progress indicator metrics.
    struct ProgressMetrics
    {
        int bar_height{4};
        int bar_border_radius{2};
        int spinner_size{20};
        int spinner_stroke{2};
        int skeleton_row_height{16};
        int skeleton_gap{8};
        int skeleton_animation_ms{1500};
        CornerRadiusToken bar_corner{CornerRadiusToken::kPill};
    };

    TransientOverlayCoordinator() = default;

    [[nodiscard]] auto dialog_metrics() const -> DialogMetrics
    {
        return dialog_;
    }

    [[nodiscard]] auto popover_metrics() const -> PopoverMetrics
    {
        return popover_;
    }

    [[nodiscard]] auto notification_metrics() const -> NotificationMetrics
    {
        return notification_;
    }

    [[nodiscard]] auto progress_metrics() const -> ProgressMetrics
    {
        return progress_;
    }

    /// Get notification dismiss delay based on severity.
    [[nodiscard]] auto dismiss_delay_for_level(NotificationLevel level) const -> int;

private:
    DialogMetrics dialog_;
    PopoverMetrics popover_;
    NotificationMetrics notification_;
    ProgressMetrics progress_;
};

} // namespace markamp::core
