#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>

namespace markamp::core
{

/// V22 Phase 19: Motion language coordination service.
///
/// Provides standardized motion timing, easing, microinteraction,
/// drag/drop, and focus transfer visual parameters.
class MotionLanguageCoordinator
{
public:
    /// Motion intent for selecting appropriate timing.
    enum class MotionIntent : uint8_t
    {
        kInstant,     /// No animation (reduced motion fallback)
        kMicro,       /// Hover, press, toggle feedback (< 100ms)
        kShort,       /// Menu open, tooltip show (100–200ms)
        kMedium,      /// Panel slide, tab switch (200–350ms)
        kLong,        /// Dialog enter/exit, mode transition (350–500ms)
        kChoreography, /// Multi-element orchestrated transition
    };

    /// Motion timing configuration.
    struct MotionTiming
    {
        int duration_ms{200};
        std::string easing{"cubic-bezier(0.2, 0, 0, 1)"}; /// Material Design 3 standard
    };

    /// Microinteraction feedback metrics.
    struct MicrointeractionMetrics
    {
        int hover_delay_ms{0};          /// Instant hover response
        int press_scale_percent{97};    /// Scale down to 97% on press
        int active_duration_ms{100};
        int completion_flash_ms{300};
        int save_indicator_ms{1500};
        float press_opacity{0.9F};
    };

    /// Drag and drop visual metrics.
    struct DragDropMetrics
    {
        int drag_threshold_px{4};       /// Movement before drag starts
        int ghost_opacity_percent{70};
        int insertion_line_height{2};
        int insertion_line_overshoot{4};
        int snap_threshold_px{8};
        ElevationToken drag_elevation{ElevationToken::kHigh};
        CornerRadiusToken ghost_corner{CornerRadiusToken::kMd};
    };

    /// Focus transfer / resize transition metrics.
    struct FocusTransferMetrics
    {
        int panel_resize_ms{200};
        int collapse_expand_ms{250};
        int focus_shift_ms{150};
        int mode_transition_ms{350};
        std::string resize_easing{"cubic-bezier(0.4, 0, 0.2, 1)"};
    };

    MotionLanguageCoordinator() = default;

    /// Get timing for a given motion intent.
    [[nodiscard]] auto timing_for_intent(MotionIntent intent) const -> MotionTiming;

    [[nodiscard]] auto microinteraction_metrics() const -> MicrointeractionMetrics
    {
        return micro_;
    }

    [[nodiscard]] auto drag_drop_metrics() const -> DragDropMetrics
    {
        return drag_;
    }

    [[nodiscard]] auto focus_transfer_metrics() const -> FocusTransferMetrics
    {
        return focus_;
    }

    /// Check if reduced motion should suppress a given intent.
    [[nodiscard]] static auto should_suppress(MotionIntent intent, bool reduced_motion) -> bool
    {
        if (!reduced_motion) return false;
        return intent != MotionIntent::kInstant;
    }

private:
    MicrointeractionMetrics micro_;
    DragDropMetrics drag_;
    FocusTransferMetrics focus_;
};

} // namespace markamp::core
