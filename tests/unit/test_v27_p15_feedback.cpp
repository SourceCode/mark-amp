/// @file test_v27_p15_feedback.cpp
/// @brief V27 Phase 15: Feedback, status bar, banner, progress, validation tokens, coordinator.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27FeedbackTokens.h"
#include "core/V27FeedbackDesignCoordinator.h"
using namespace markamp::ui;
using namespace markamp::core;
TEST_CASE("V27 P15: Feedback severity count", "[v27][p15]") { REQUIRE(v27_feedback_severity_count() == 4); }
TEST_CASE("V27 P15: Status bar tokens", "[v27][p15]") {
    REQUIRE(V27StatusBarTokens::kHeight > 0);
    REQUIRE(V27StatusBarTokens::kProgressHeight > 0);
}
TEST_CASE("V27 P15: Banner tokens", "[v27][p15]") {
    REQUIRE(V27BannerTokens::kHeight > 0);
    REQUIRE(V27BannerTokens::kProgressBarRadius > 0);
}
TEST_CASE("V27 P15: Progress tokens", "[v27][p15]") {
    REQUIRE(V27ProgressTokens::kBarHeight > 0);
    REQUIRE(V27ProgressTokens::kBarRadius > 0);
    REQUIRE(V27ProgressTokens::kIndeterminateCycleMs > 0);
    REQUIRE(V27ProgressTokens::kCircularDiameter > V27ProgressTokens::kCircularStroke);
}
TEST_CASE("V27 P15: Inline validation tokens", "[v27][p15]") {
    REQUIRE(V27InlineValidationTokens::kIconSize > 0);
    REQUIRE(V27InlineValidationTokens::kFieldUnderlineWidth > 0);
    REQUIRE(V27InlineValidationTokens::kShakeDurationMs > 0);
}
TEST_CASE("V27 P15: Feedback coordinator registration", "[v27][p15]") {
    V27FeedbackDesignCoordinator coord;
    coord.register_surface({"Settings Validation", FeedbackSurfaceType::kInlineValidation, true, true, true});
    coord.register_surface({"Status Bar", FeedbackSurfaceType::kStatusBar, true, true, true});
    coord.register_surface({"Editor Banner", FeedbackSurfaceType::kBanner, false, true, false});
    REQUIRE(coord.surface_count() == 3);
    REQUIRE(coord.severity_redesigned_count() == 2);
    REQUIRE_FALSE(coord.all_emoji_free());
    REQUIRE(coord.count_by_type(FeedbackSurfaceType::kInlineValidation) == 1);
    REQUIRE(coord.count_by_type(FeedbackSurfaceType::kStatusBar) == 1);
}
