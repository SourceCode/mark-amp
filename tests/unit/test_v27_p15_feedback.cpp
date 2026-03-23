/// @file test_v27_p15_feedback.cpp
/// @brief V27 Phase 15: Feedback, status bar, banner tokens.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27FeedbackTokens.h"
using namespace markamp::ui;
TEST_CASE("V27 P15: Feedback severity count", "[v27][p15]") { REQUIRE(v27_feedback_severity_count() == 4); }
TEST_CASE("V27 P15: Status bar tokens", "[v27][p15]") {
    REQUIRE(V27StatusBarTokens::kHeight > 0);
    REQUIRE(V27StatusBarTokens::kProgressHeight > 0);
}
TEST_CASE("V27 P15: Banner tokens", "[v27][p15]") {
    REQUIRE(V27BannerTokens::kHeight > 0);
    REQUIRE(V27BannerTokens::kProgressBarRadius > 0);
}
