/// @file test_v26_p13_feedback.cpp
/// @brief V26 Phase 13: Feedback, status, progress & inline validation tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26DataFeedbackConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P13: Severity badge metrics", "[v26][p13]")
{
    REQUIRE(SeverityBadgeMetrics::kHeight > 0);
    REQUIRE(SeverityBadgeMetrics::kRadius > 0);
    REQUIRE(SeverityBadgeMetrics::kIconSize > 0);
    REQUIRE(SeverityBadgeMetrics::kIconTextGap > 0);
}

TEST_CASE("V26 P13: Progress bar metrics", "[v26][p13]")
{
    REQUIRE(ProgressV26Metrics::kBarHeight > 0);
    REQUIRE(ProgressV26Metrics::kBarRadius > 0);
    REQUIRE(ProgressV26Metrics::kBarMinW > 0);
}

TEST_CASE("V26 P13: Spinner size hierarchy", "[v26][p13]")
{
    REQUIRE(ProgressV26Metrics::kSpinnerSizeSmall < ProgressV26Metrics::kSpinnerSizeMedium);
    REQUIRE(ProgressV26Metrics::kSpinnerSizeMedium < ProgressV26Metrics::kSpinnerSizeLarge);
}

TEST_CASE("V26 P13: Inline validation metrics", "[v26][p13]")
{
    REQUIRE(InlineAlertMetrics::kValidationIconSize > 0);
    REQUIRE(InlineAlertMetrics::kValidationFontPt > 0);
    REQUIRE(InlineAlertMetrics::kValidationGap > 0);
}

TEST_CASE("V26 P13: Banner alert metrics", "[v26][p13]")
{
    REQUIRE(InlineAlertMetrics::kBannerHeight > 0);
    REQUIRE(InlineAlertMetrics::kBannerIconSize > 0);
    REQUIRE(InlineAlertMetrics::kBannerRadius > 0);
}
