#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>

namespace markamp::core
{

/// V22 Phase 17: Accessibility coordination service.
///
/// Provides standardized metrics for contrast quality, focus rings,
/// hit targets, reduced motion, and low-vision readability.
class AccessibilityCoordinator
{
public:
    /// WCAG contrast level requirement.
    enum class ContrastLevel : uint8_t
    {
        kAA,       /// 4.5:1 normal text, 3:1 large text
        kAAA,      /// 7:1 normal text, 4.5:1 large text
        kAALarge,  /// 3:1 for large text (>= 18pt or 14pt bold)
    };

    /// Focus ring metrics.
    struct FocusRingMetrics
    {
        int ring_width{2};
        int ring_offset{2};             /// Offset from element edge
        int ring_corner_extra{2};       /// Extra corner radius beyond element
        float ring_opacity{1.0F};
        bool use_double_ring{false};    /// Double ring for high-contrast mode
        int inner_ring_width{1};
    };

    /// Hit target metrics (pointer and touch).
    struct HitTargetMetrics
    {
        int min_pointer_target{24};     /// Minimum clickable area
        int min_touch_target{44};       /// Minimum touch area (WCAG 2.5.8)
        int interactive_padding{4};     /// Extra padding around interactive elements
        int splitter_grab_width{8};     /// Effective grab width for splitters
    };

    /// Reduced motion configuration.
    struct ReducedMotionConfig
    {
        bool respect_system_preference{true};
        int max_transition_ms{0};       /// 0 = instant when reduced motion is on
        bool disable_parallax{true};
        bool disable_auto_scroll{true};
        bool simplify_loading_animations{true};
    };

    /// Low-vision readability metrics.
    struct LowVisionMetrics
    {
        int min_text_size{11};          /// Minimum font size in px
        int min_line_height_factor{140}; /// Minimum line-height as % of font size
        int min_letter_spacing{0};      /// Minimum letter spacing in px * 100
        bool require_non_color_differentiators{true}; /// Icons, patterns, text in addition to color
        int icon_min_size{14};          /// Minimum icon size
    };

    AccessibilityCoordinator() = default;

    [[nodiscard]] auto focus_ring_metrics() const -> FocusRingMetrics
    {
        return focus_;
    }

    [[nodiscard]] auto hit_target_metrics() const -> HitTargetMetrics
    {
        return hit_;
    }

    [[nodiscard]] auto reduced_motion_config() const -> ReducedMotionConfig
    {
        return motion_;
    }

    [[nodiscard]] auto low_vision_metrics() const -> LowVisionMetrics
    {
        return vision_;
    }

    /// Get minimum contrast ratio for a given level.
    [[nodiscard]] auto min_contrast_ratio(ContrastLevel level) const -> float;

private:
    FocusRingMetrics focus_;
    HitTargetMetrics hit_;
    ReducedMotionConfig motion_;
    LowVisionMetrics vision_;
};

} // namespace markamp::core
