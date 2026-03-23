/// @file test_v27_p12_settings.cpp
/// @brief V27 Phase 12: Settings, theme gallery, control family tokens.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27SettingsTokens.h"
using namespace markamp::ui;
TEST_CASE("V27 P12: Settings shell tokens", "[v27][p12]") {
    REQUIRE(V27SettingsShellTokens::kCategoryRailWidth > 0);
    REQUIRE(V27SettingsShellTokens::kSearchHeaderHeight > V27SettingsShellTokens::kSearchInputHeight);
}
TEST_CASE("V27 P12: Theme gallery tokens", "[v27][p12]") {
    REQUIRE(V27ThemeGalleryTokens::kCardWidth > 0);
    REQUIRE(V27ThemeGalleryTokens::kCardHeight > V27ThemeGalleryTokens::kPreviewHeight);
    REQUIRE(V27ThemeGalleryTokens::kCardRadius > 0);
}
TEST_CASE("V27 P12: Control family tokens", "[v27][p12]") {
    REQUIRE(V27ControlFamilyTokens::kToggleWidth > V27ControlFamilyTokens::kToggleHeight);
    REQUIRE(V27ControlFamilyTokens::kChipRadius > 0);
    REQUIRE(V27ControlFamilyTokens::kButtonHeight >= V27ControlFamilyTokens::kButtonHeightSm);
}
