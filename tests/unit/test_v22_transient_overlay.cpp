#include "core/TransientOverlayCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("TransientOverlayCoordinator dialog metrics", "[v22][transient]")
{
    TransientOverlayCoordinator coord;
    auto metrics = coord.dialog_metrics();

    REQUIRE(metrics.min_width > 0);
    REQUIRE(metrics.max_width > metrics.min_width);
    REQUIRE(metrics.header_height > 0);
    REQUIRE(metrics.footer_height > 0);
    REQUIRE(metrics.button_height > 0);
    REQUIRE(metrics.button_min_width > 0);
    REQUIRE(metrics.backdrop_opacity > 0.0F);
    REQUIRE(metrics.corner == CornerRadiusToken::kLg);
    REQUIRE(metrics.elevation == ElevationToken::kOverlay);
}

TEST_CASE("TransientOverlayCoordinator popover metrics", "[v22][transient]")
{
    TransientOverlayCoordinator coord;
    auto metrics = coord.popover_metrics();

    REQUIRE(metrics.min_width > 0);
    REQUIRE(metrics.max_width > metrics.min_width);
    REQUIRE(metrics.padding > 0);
    REQUIRE(metrics.arrow_size > 0);
    REQUIRE(metrics.corner == CornerRadiusToken::kMd);
    REQUIRE(metrics.elevation == ElevationToken::kHigh);
}

TEST_CASE("TransientOverlayCoordinator notification metrics", "[v22][transient]")
{
    TransientOverlayCoordinator coord;
    auto metrics = coord.notification_metrics();

    REQUIRE(metrics.width > 0);
    REQUIRE(metrics.min_height > 0);
    REQUIRE(metrics.max_height > metrics.min_height);
    REQUIRE(metrics.icon_size > 0);
    REQUIRE(metrics.dismiss_delay_ms > 0);
    REQUIRE(metrics.slide_duration_ms > 0);
    REQUIRE(metrics.corner == CornerRadiusToken::kMd);
}

TEST_CASE("TransientOverlayCoordinator progress metrics", "[v22][transient]")
{
    TransientOverlayCoordinator coord;
    auto metrics = coord.progress_metrics();

    REQUIRE(metrics.bar_height > 0);
    REQUIRE(metrics.spinner_size > 0);
    REQUIRE(metrics.spinner_stroke > 0);
    REQUIRE(metrics.skeleton_row_height > 0);
    REQUIRE(metrics.bar_corner == CornerRadiusToken::kPill);
}

TEST_CASE("TransientOverlayCoordinator dismiss delay by severity", "[v22][transient]")
{
    TransientOverlayCoordinator coord;

    // Info and success auto-dismiss
    REQUIRE(coord.dismiss_delay_for_level(TransientOverlayCoordinator::NotificationLevel::kInfo) > 0);
    REQUIRE(coord.dismiss_delay_for_level(TransientOverlayCoordinator::NotificationLevel::kSuccess) > 0);

    // Warning has longer delay
    REQUIRE(coord.dismiss_delay_for_level(TransientOverlayCoordinator::NotificationLevel::kWarning) >
            coord.dismiss_delay_for_level(TransientOverlayCoordinator::NotificationLevel::kInfo));

    // Errors never auto-dismiss
    REQUIRE(coord.dismiss_delay_for_level(TransientOverlayCoordinator::NotificationLevel::kError) == 0);
}
