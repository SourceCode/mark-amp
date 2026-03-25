#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// V22 Phase 16: Onboarding state coordination service.
///
/// Provides standardized metrics for startup, welcome, walkthrough,
/// empty-panel, loading, and error states.
class OnboardingStateCoordinator
{
public:
    /// Empty state category for surface-specific customization.
    enum class EmptyStateCategory : uint8_t
    {
        kExplorer,
        kSearch,
        kTerminal,
        kGeneric,
    };

    /// Welcome / startup surface metrics.
    struct WelcomeSurfaceMetrics
    {
        int max_content_width{680};
        int hero_icon_size{64};
        int hero_gap{24};
        int action_card_width{200};
        int action_card_height{120};
        int action_card_gap{16};
        int recent_item_height{28};
        int recent_list_max{8};
        CornerRadiusToken card_corner{CornerRadiusToken::kLg};
        ElevationToken card_elevation{ElevationToken::kLow};
        TypeScaleToken hero_font{TypeScaleToken::kDisplay};
        TypeScaleToken subtitle_font{TypeScaleToken::kSubtitle};
        TypeScaleToken action_font{TypeScaleToken::kBody};
    };

    /// Walkthrough / helper card metrics.
    struct WalkthroughMetrics
    {
        int card_width{320};
        int card_min_height{200};
        int step_indicator_size{8};
        int step_indicator_gap{6};
        int card_padding{20};
        int image_height{120};
        CornerRadiusToken card_corner{CornerRadiusToken::kLg};
        ElevationToken card_elevation{ElevationToken::kMedium};
        TypeScaleToken title_font{TypeScaleToken::kSubtitle};
        TypeScaleToken body_font{TypeScaleToken::kBody};
    };

    /// Empty state metrics for panels/surfaces.
    struct EmptyStateMetrics
    {
        int icon_size{48};
        int text_max_width{280};
        int gap{12};
        int action_button_height{32};
        IconMetricToken icon_metric{IconMetricToken::kLarge};
        TypeScaleToken title_font{TypeScaleToken::kSubtitle};
        TypeScaleToken message_font{TypeScaleToken::kBody};
        float icon_opacity{0.5F};
        float message_opacity{0.7F};
    };

    /// Loading / skeleton state metrics.
    struct LoadingStateMetrics
    {
        int skeleton_row_height{16};
        int skeleton_row_gap{8};
        int skeleton_rows{5};
        int shimmer_duration_ms{1500};
        float skeleton_opacity{0.12F};
        CornerRadiusToken skeleton_corner{CornerRadiusToken::kSm};
    };

    OnboardingStateCoordinator() = default;

    [[nodiscard]] auto welcome_surface_metrics() const -> WelcomeSurfaceMetrics
    {
        return welcome_;
    }

    [[nodiscard]] auto walkthrough_metrics() const -> WalkthroughMetrics
    {
        return walkthrough_;
    }

    [[nodiscard]] auto empty_state_metrics() const -> EmptyStateMetrics
    {
        return empty_;
    }

    [[nodiscard]] auto loading_state_metrics() const -> LoadingStateMetrics
    {
        return loading_;
    }

    /// Get category-specific empty state message.
    [[nodiscard]] auto empty_message_for_category(EmptyStateCategory cat) const -> std::string;

private:
    WelcomeSurfaceMetrics welcome_;
    WalkthroughMetrics walkthrough_;
    EmptyStateMetrics empty_;
    LoadingStateMetrics loading_;
};

} // namespace markamp::core
