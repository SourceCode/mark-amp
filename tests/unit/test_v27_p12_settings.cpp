/// @file test_v27_p12_settings.cpp
/// @brief V27 Phase 12: Settings, theme gallery, control family tokens, coordinator.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27SettingsTokens.h"
#include "core/V27SettingsDesignCoordinator.h"
using namespace markamp::ui;
using namespace markamp::core;
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
TEST_CASE("V27 P12: Settings row tokens", "[v27][p12]") {
    REQUIRE(V27SettingsRowTokens::kRowMinHeight > 0);
    REQUIRE(V27SettingsRowTokens::kGroupGap > V27SettingsRowTokens::kHelpTextGap);
    REQUIRE(V27SettingsRowTokens::kResetButtonSize > 0);
}
TEST_CASE("V27 P12: Settings coordinator emoji validation", "[v27][p12]") {
    V27SettingsDesignCoordinator coord;
    coord.register_category({"General", true, true, 15});
    coord.register_category({"Editor", true, true, 20});
    coord.register_category({"Theme", true, false, 8});
    REQUIRE(coord.category_count() == 3);
    REQUIRE(coord.emoji_free_count() == 2);
    REQUIRE_FALSE(coord.all_emoji_free());
    REQUIRE(coord.canonical_icon_count() == 3);
    REQUIRE(coord.total_rows() == 43);
}
