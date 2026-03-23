/// @file test_v26_p11_dialogs.cpp
/// @brief V26 Phase 11: Dialogs, popovers, tooltips, notifications & overlays tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26DialogOverlayConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P11: Dialog shell sizing", "[v26][p11]")
{
    REQUIRE(DialogV26Metrics::kMinWidth > 0);
    REQUIRE(DialogV26Metrics::kMaxWidth > DialogV26Metrics::kMinWidth);
    REQUIRE(DialogV26Metrics::kMinHeight > 0);
}

TEST_CASE("V26 P11: Dialog chrome", "[v26][p11]")
{
    REQUIRE(DialogV26Metrics::kBorderRadius > 0);
    REQUIRE(DialogV26Metrics::kShadowBlur > 0);
    REQUIRE(DialogV26Metrics::kBackdropAlpha > 0.0F);
    REQUIRE(DialogV26Metrics::kBackdropAlpha < 1.0F);
}

TEST_CASE("V26 P11: Dialog footer buttons", "[v26][p11]")
{
    REQUIRE(DialogV26Metrics::kButtonHeight > 0);
    REQUIRE(DialogV26Metrics::kButtonGap > 0);
    REQUIRE(DialogV26Metrics::kFooterHeight > DialogV26Metrics::kButtonHeight);
}

TEST_CASE("V26 P11: Popover metrics", "[v26][p11]")
{
    REQUIRE(PopoverV26Metrics::kMaxWidth > 0);
    REQUIRE(PopoverV26Metrics::kBorderRadius > 0);
    REQUIRE(PopoverV26Metrics::kArrowSize > 0);
    REQUIRE(PopoverV26Metrics::kAnchorGap > 0);
}

TEST_CASE("V26 P11: Tooltip metrics", "[v26][p11]")
{
    REQUIRE(TooltipV26Metrics::kMaxWidth > 0);
    REQUIRE(TooltipV26Metrics::kDelayMs > 0);
    REQUIRE(TooltipV26Metrics::kFadeMs > 0);
    REQUIRE(TooltipV26Metrics::kOpacity > 0.5F);
}

TEST_CASE("V26 P11: Toast metrics", "[v26][p11]")
{
    REQUIRE(ToastV26Metrics::kWidth > 0);
    REQUIRE(ToastV26Metrics::kDismissMs > 0);
    REQUIRE(ToastV26Metrics::kMaxVisible > 0);
    REQUIRE(ToastV26Metrics::kMaxVisible <= 5);
    REQUIRE(ToastV26Metrics::kStackGap > 0);
}
