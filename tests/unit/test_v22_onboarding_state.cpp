#include "core/OnboardingStateCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("OnboardingStateCoordinator welcome surface metrics", "[v22][onboarding]")
{
    OnboardingStateCoordinator coord;
    auto metrics = coord.welcome_surface_metrics();

    REQUIRE(metrics.max_content_width > 0);
    REQUIRE(metrics.hero_icon_size > 0);
    REQUIRE(metrics.action_card_width > 0);
    REQUIRE(metrics.action_card_height > 0);
    REQUIRE(metrics.recent_item_height > 0);
    REQUIRE(metrics.recent_list_max > 0);
    REQUIRE(metrics.card_corner == CornerRadiusToken::kLg);
    REQUIRE(metrics.hero_font == TypeScaleToken::kDisplay);
}

TEST_CASE("OnboardingStateCoordinator walkthrough metrics", "[v22][onboarding]")
{
    OnboardingStateCoordinator coord;
    auto metrics = coord.walkthrough_metrics();

    REQUIRE(metrics.card_width > 0);
    REQUIRE(metrics.card_min_height > 0);
    REQUIRE(metrics.step_indicator_size > 0);
    REQUIRE(metrics.card_padding > 0);
    REQUIRE(metrics.card_corner == CornerRadiusToken::kLg);
    REQUIRE(metrics.card_elevation == ElevationToken::kMedium);
}

TEST_CASE("OnboardingStateCoordinator empty state metrics", "[v22][onboarding]")
{
    OnboardingStateCoordinator coord;
    auto metrics = coord.empty_state_metrics();

    REQUIRE(metrics.icon_size > 0);
    REQUIRE(metrics.text_max_width > 0);
    REQUIRE(metrics.icon_opacity > 0.0F);
    REQUIRE(metrics.icon_opacity < 1.0F);
    REQUIRE(metrics.message_opacity > 0.0F);
    REQUIRE(metrics.message_opacity < 1.0F);
}

TEST_CASE("OnboardingStateCoordinator loading state metrics", "[v22][onboarding]")
{
    OnboardingStateCoordinator coord;
    auto metrics = coord.loading_state_metrics();

    REQUIRE(metrics.skeleton_row_height > 0);
    REQUIRE(metrics.skeleton_row_gap > 0);
    REQUIRE(metrics.skeleton_rows > 0);
    REQUIRE(metrics.shimmer_duration_ms > 0);
    REQUIRE(metrics.skeleton_corner == CornerRadiusToken::kSm);
}

TEST_CASE("OnboardingStateCoordinator category-specific messages", "[v22][onboarding]")
{
    OnboardingStateCoordinator coord;

    for (auto cat : {OnboardingStateCoordinator::EmptyStateCategory::kExplorer,
                     OnboardingStateCoordinator::EmptyStateCategory::kSearch,
                     OnboardingStateCoordinator::EmptyStateCategory::kTerminal,
                     OnboardingStateCoordinator::EmptyStateCategory::kGeneric})
    {
        auto msg = coord.empty_message_for_category(cat);
        REQUIRE_FALSE(msg.empty());
    }
}
