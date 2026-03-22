#include "core/AccessibilityCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AccessibilityCoordinator focus ring metrics", "[v22][accessibility]")
{
    AccessibilityCoordinator coord;
    auto metrics = coord.focus_ring_metrics();

    REQUIRE(metrics.ring_width > 0);
    REQUIRE(metrics.ring_offset > 0);
    REQUIRE(metrics.ring_opacity > 0.0F);
}

TEST_CASE("AccessibilityCoordinator hit target metrics", "[v22][accessibility]")
{
    AccessibilityCoordinator coord;
    auto metrics = coord.hit_target_metrics();

    REQUIRE(metrics.min_pointer_target >= 24);
    REQUIRE(metrics.min_touch_target >= 44);
    REQUIRE(metrics.min_touch_target > metrics.min_pointer_target);
    REQUIRE(metrics.splitter_grab_width > 0);
}

TEST_CASE("AccessibilityCoordinator reduced motion config", "[v22][accessibility]")
{
    AccessibilityCoordinator coord;
    auto config = coord.reduced_motion_config();

    REQUIRE(config.respect_system_preference);
    REQUIRE(config.max_transition_ms == 0); // Instant when reduced motion on
    REQUIRE(config.disable_parallax);
    REQUIRE(config.disable_auto_scroll);
}

TEST_CASE("AccessibilityCoordinator low vision metrics", "[v22][accessibility]")
{
    AccessibilityCoordinator coord;
    auto metrics = coord.low_vision_metrics();

    REQUIRE(metrics.min_text_size > 0);
    REQUIRE(metrics.min_line_height_factor > 100); // At least 100% line height
    REQUIRE(metrics.require_non_color_differentiators);
    REQUIRE(metrics.icon_min_size > 0);
}

TEST_CASE("AccessibilityCoordinator WCAG contrast ratios", "[v22][accessibility]")
{
    AccessibilityCoordinator coord;

    REQUIRE(coord.min_contrast_ratio(AccessibilityCoordinator::ContrastLevel::kAA) == 4.5F);
    REQUIRE(coord.min_contrast_ratio(AccessibilityCoordinator::ContrastLevel::kAAA) == 7.0F);
    REQUIRE(coord.min_contrast_ratio(AccessibilityCoordinator::ContrastLevel::kAALarge) == 3.0F);

    // AAA is stricter than AA
    REQUIRE(coord.min_contrast_ratio(AccessibilityCoordinator::ContrastLevel::kAAA) >
            coord.min_contrast_ratio(AccessibilityCoordinator::ContrastLevel::kAA));
}
