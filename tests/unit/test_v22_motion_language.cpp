#include "core/MotionLanguageCoordinator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("MotionLanguageCoordinator instant timing", "[v22][motion]")
{
    MotionLanguageCoordinator coord;
    auto timing = coord.timing_for_intent(MotionLanguageCoordinator::MotionIntent::kInstant);

    REQUIRE(timing.duration_ms == 0);
    REQUIRE(timing.easing == "linear");
}

TEST_CASE("MotionLanguageCoordinator timing escalation", "[v22][motion]")
{
    MotionLanguageCoordinator coord;

    auto micro = coord.timing_for_intent(MotionLanguageCoordinator::MotionIntent::kMicro);
    auto srt = coord.timing_for_intent(MotionLanguageCoordinator::MotionIntent::kShort);
    auto med = coord.timing_for_intent(MotionLanguageCoordinator::MotionIntent::kMedium);
    auto lng = coord.timing_for_intent(MotionLanguageCoordinator::MotionIntent::kLong);
    auto choreo = coord.timing_for_intent(MotionLanguageCoordinator::MotionIntent::kChoreography);

    // Timing should escalate from micro to choreography
    REQUIRE(micro.duration_ms < srt.duration_ms);
    REQUIRE(srt.duration_ms < med.duration_ms);
    REQUIRE(med.duration_ms < lng.duration_ms);
    REQUIRE(lng.duration_ms <= choreo.duration_ms);
}

TEST_CASE("MotionLanguageCoordinator microinteraction metrics", "[v22][motion]")
{
    MotionLanguageCoordinator coord;
    auto metrics = coord.microinteraction_metrics();

    REQUIRE(metrics.hover_delay_ms == 0); // Instant hover
    REQUIRE(metrics.press_scale_percent < 100);
    REQUIRE(metrics.active_duration_ms > 0);
    REQUIRE(metrics.press_opacity > 0.0F);
    REQUIRE(metrics.press_opacity <= 1.0F);
}

TEST_CASE("MotionLanguageCoordinator drag drop metrics", "[v22][motion]")
{
    MotionLanguageCoordinator coord;
    auto metrics = coord.drag_drop_metrics();

    REQUIRE(metrics.drag_threshold_px > 0);
    REQUIRE(metrics.ghost_opacity_percent > 0);
    REQUIRE(metrics.insertion_line_height > 0);
    REQUIRE(metrics.snap_threshold_px > 0);
    REQUIRE(metrics.drag_elevation == ElevationToken::kHigh);
}

TEST_CASE("MotionLanguageCoordinator focus transfer metrics", "[v22][motion]")
{
    MotionLanguageCoordinator coord;
    auto metrics = coord.focus_transfer_metrics();

    REQUIRE(metrics.panel_resize_ms > 0);
    REQUIRE(metrics.collapse_expand_ms > 0);
    REQUIRE(metrics.focus_shift_ms > 0);
    REQUIRE(metrics.mode_transition_ms > 0);
    REQUIRE_FALSE(metrics.resize_easing.empty());
}

TEST_CASE("MotionLanguageCoordinator reduced motion suppression", "[v22][motion]")
{
    // Without reduced motion, nothing is suppressed
    REQUIRE_FALSE(MotionLanguageCoordinator::should_suppress(
        MotionLanguageCoordinator::MotionIntent::kMedium, false));

    // With reduced motion, everything except instant is suppressed
    REQUIRE_FALSE(MotionLanguageCoordinator::should_suppress(
        MotionLanguageCoordinator::MotionIntent::kInstant, true));
    REQUIRE(MotionLanguageCoordinator::should_suppress(
        MotionLanguageCoordinator::MotionIntent::kMicro, true));
    REQUIRE(MotionLanguageCoordinator::should_suppress(
        MotionLanguageCoordinator::MotionIntent::kChoreography, true));
}
