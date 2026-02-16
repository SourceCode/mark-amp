/// @file test_settings_panel.cpp
/// Compile-only verification of SettingsPanel staged-edit API.
/// Full widget-level testing requires a running wxWidgets event loop,
/// so these tests verify the API surface exists at the header level.

#include "ui/SettingsPanel.h"

#include <catch2/catch_test_macros.hpp>

#include <type_traits>

using namespace markamp;

// Verify the SettingsPanel class is defined and its constants are correct
TEST_CASE("SettingsPanel constants are correct", "[settings][panel]")
{
    REQUIRE(ui::SettingsPanel::kCategoryPadding == 12);
    REQUIRE(ui::SettingsPanel::kSettingRowHeight == 44);
    REQUIRE(ui::SettingsPanel::kMaxVisibleSettings == 50);
}

// Verify SettingDefinition struct compiles and has expected fields
TEST_CASE("SettingDefinition struct fields", "[settings][panel]")
{
    ui::SettingDefinition def;
    def.setting_id = "editor.fontSize";
    def.label = "Font Size";
    def.description = "Controls font size";
    def.category = "Editor";
    def.type = core::SettingType::Integer;
    def.default_value = "13";
    def.choices = {};
    def.min_int = 8;
    def.max_int = 72;

    REQUIRE(def.setting_id == "editor.fontSize");
    REQUIRE(def.label == "Font Size");
    REQUIRE(def.type == core::SettingType::Integer);
    REQUIRE(def.min_int == 8);
}
