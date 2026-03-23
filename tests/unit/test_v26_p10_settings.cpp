/// @file test_v26_p10_settings.cpp
/// @brief V26 Phase 10: Settings, preferences, theme gallery and control styling tests.
#include <catch2/catch_test_macros.hpp>
#include "ui/V26ControlConvergence.h"

using namespace markamp::ui;

TEST_CASE("V26 P10: Preference row dimensions", "[v26][p10]")
{
    REQUIRE(PreferenceRowMetrics::kRowMinHeight > 0);
    REQUIRE(PreferenceRowMetrics::kRowPaddingH > 0);
    REQUIRE(PreferenceRowMetrics::kLabelFontPt > PreferenceRowMetrics::kHelpFontPt);
}

TEST_CASE("V26 P10: Control alignment", "[v26][p10]")
{
    REQUIRE(PreferenceRowMetrics::kControlMinW < PreferenceRowMetrics::kControlMaxW);
    REQUIRE(PreferenceRowMetrics::kControlHeight > 0);
    REQUIRE(PreferenceRowMetrics::kToggleWidth > PreferenceRowMetrics::kToggleHeight);
}

TEST_CASE("V26 P10: Category header", "[v26][p10]")
{
    REQUIRE(PreferenceRowMetrics::kCategoryHeaderH > 0);
    REQUIRE(PreferenceRowMetrics::kCategoryFontPt > PreferenceRowMetrics::kLabelFontPt);
    REQUIRE(PreferenceRowMetrics::kCategoryGap > 0);
}

TEST_CASE("V26 P10: Theme gallery card sizing", "[v26][p10]")
{
    REQUIRE(ThemeGalleryCardMetrics::kCardWidth > 0);
    REQUIRE(ThemeGalleryCardMetrics::kCardHeight > 0);
    REQUIRE(ThemeGalleryCardMetrics::kPreviewHeight < ThemeGalleryCardMetrics::kCardHeight);
    REQUIRE(ThemeGalleryCardMetrics::kFooterHeight + ThemeGalleryCardMetrics::kPreviewHeight <=
            ThemeGalleryCardMetrics::kCardHeight + 20); // allow for borders
}

TEST_CASE("V26 P10: Theme gallery card presentation", "[v26][p10]")
{
    REQUIRE(ThemeGalleryCardMetrics::kCardRadius > 0);
    REQUIRE(ThemeGalleryCardMetrics::kHoverScale > 1.0F);
    REQUIRE(ThemeGalleryCardMetrics::kInactiveOpacity < 1.0F);
    REQUIRE(ThemeGalleryCardMetrics::kActiveBorderW > 0);
}

TEST_CASE("V26 P10: Control style input metrics", "[v26][p10]")
{
    REQUIRE(ControlStyleMetrics::kInputHeight == 28);
    REQUIRE(ControlStyleMetrics::kInputFocusBorderW > ControlStyleMetrics::kInputBorderW);
    REQUIRE(ControlStyleMetrics::kInputRadius > 0);
}

TEST_CASE("V26 P10: Control style checkbox and slider", "[v26][p10]")
{
    REQUIRE(ControlStyleMetrics::kCheckboxSize > 0);
    REQUIRE(ControlStyleMetrics::kSliderThumbSize > ControlStyleMetrics::kSliderTrackH);
}

TEST_CASE("V26 P10: Focus ring metrics", "[v26][p10]")
{
    REQUIRE(ControlStyleMetrics::kFocusRingWidth > 0);
    REQUIRE(ControlStyleMetrics::kFocusRingOffset > 0);
    REQUIRE(ControlStyleMetrics::kFocusRingRadius > ControlStyleMetrics::kInputRadius);
}

TEST_CASE("V26 P10: Search bar metrics", "[v26][p10]")
{
    REQUIRE(SearchBarMetrics::kHeight > 0);
    REQUIRE(SearchBarMetrics::kFocusBorderW > SearchBarMetrics::kBorderW);
    REQUIRE(SearchBarMetrics::kRadius > 0);
}
