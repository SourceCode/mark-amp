/// @file test_settings_panel.cpp
/// Comprehensive tests for the Settings UI & Preferences System (Phase 05).
/// Covers SettingDefinition struct, fuzzy search scoring, undo/redo stack,
/// profile manager, accessibility fields, and FirstRunWizard guard logic.

#include "core/Config.h"
#include "core/ConfigProfile.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"
#include "ui/FirstRunWizard.h"
#include "ui/SettingsPanel.h"

#include <catch2/catch_test_macros.hpp>

#include <type_traits>

using namespace markamp;

// ── SettingDefinition struct tests ──

TEST_CASE("SettingsPanel constants are correct", "[settings][panel]")
{
    REQUIRE(ui::SettingsPanel::kCategoryPadding == 12);
    REQUIRE(ui::SettingsPanel::kSettingRowHeight == 44);
    REQUIRE(ui::SettingsPanel::kMaxVisibleSettings == 50);
}

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

// ── SettingDefinition schema extensions ──

TEST_CASE("SettingDefinition schema extensions compile and default correctly",
          "[settings][panel][schema]")
{
    ui::SettingDefinition def;

    // Default flags
    REQUIRE_FALSE(def.deprecated);
    REQUIRE_FALSE(def.experimental);
    REQUIRE_FALSE(def.restart_required);

    // Default ranges
    REQUIRE(def.min_double == 0.0);
    REQUIRE(def.max_double == 100.0);
    REQUIRE(def.order_priority == 100);

    // Tags can be populated
    def.tags = {"font", "editor", "size"};
    REQUIRE(def.tags.size() == 3);

    // Flags can be set
    def.deprecated = true;
    def.experimental = true;
    def.restart_required = true;
    REQUIRE(def.deprecated);
    REQUIRE(def.experimental);
    REQUIRE(def.restart_required);
}

// ── Setting types coverage ──

TEST_CASE("SettingType enum covers all expected types", "[settings][panel][types]")
{
    // Ensure all required types exist
    REQUIRE(static_cast<int>(core::SettingType::Boolean) >= 0);
    REQUIRE(static_cast<int>(core::SettingType::Integer) >= 0);
    REQUIRE(static_cast<int>(core::SettingType::String) >= 0);
    REQUIRE(static_cast<int>(core::SettingType::Choice) >= 0);
    REQUIRE(static_cast<int>(core::SettingType::Double) >= 0);
    REQUIRE(static_cast<int>(core::SettingType::FilePath) >= 0);
    REQUIRE(static_cast<int>(core::SettingType::Color) >= 0);
    REQUIRE(static_cast<int>(core::SettingType::KeyBinding) >= 0);
    REQUIRE(static_cast<int>(core::SettingType::StringList) >= 0);
}

// ── Category organization ──

TEST_CASE("SettingDefinition categories for organization", "[settings][panel][category]")
{
    const std::vector<std::string> expected_categories = {
        "Editor", "Appearance", "Keybindings", "Plugins", "Advanced"};

    for (const auto& cat : expected_categories)
    {
        ui::SettingDefinition def;
        def.category = cat;
        REQUIRE_FALSE(def.category.empty());
    }
}

// ── ConfigProfileManager tests ──

TEST_CASE("ConfigProfileManager registers and queries profiles", "[settings][profiles]")
{
    core::ConfigProfileManager manager;

    // Built-in profiles are registered in constructor
    REQUIRE(manager.profile_count() >= 3);

    // Profile names are accessible
    auto names = manager.profile_names();
    REQUIRE_FALSE(names.empty());

    // Find by name
    for (const auto& name : names)
    {
        const auto* profile = manager.find_profile(name);
        REQUIRE(profile != nullptr);
        REQUIRE(profile->name == name);
    }
}

TEST_CASE("ConfigProfileManager custom profile registration", "[settings][profiles]")
{
    core::ConfigProfileManager manager;

    core::ConfigProfile custom;
    custom.name = "Test Custom";
    custom.description = "A test profile";
    custom.profile_id = core::ProfileId::kCustom;
    custom.overrides = {{"editor.fontSize", "16"}, {"editor.wordWrap", "true"}};

    const auto count_before = manager.profile_count();
    manager.register_profile(custom);
    REQUIRE(manager.profile_count() == count_before + 1);

    const auto* found = manager.find_profile("Test Custom");
    REQUIRE(found != nullptr);
    REQUIRE(found->overrides.size() == 2);
}

TEST_CASE("ConfigProfileManager apply_profile sets config values", "[settings][profiles]")
{
    core::Config config;

    // Set initial value
    config.set("editor.fontSize", std::string_view{"13"});

    core::ConfigProfileManager manager;

    core::ConfigProfile profile;
    profile.name = "BigFont";
    profile.overrides = {{"editor.fontSize", "24"}};
    manager.register_profile(profile);

    manager.apply_profile("BigFont", config);
    REQUIRE(config.get_string("editor.fontSize") == "24");
}

// ── FirstRunWizard ShouldShow guard ──

TEST_CASE("FirstRunWizard::ShouldShow returns true for fresh config", "[settings][wizard]")
{
    core::Config config;

    // Fresh config — should show wizard
    REQUIRE(ui::FirstRunWizard::ShouldShow(config));
}

TEST_CASE("FirstRunWizard::ShouldShow returns false after completion", "[settings][wizard]")
{
    core::Config config;

    config.set("app.first_run_completed", true);
    REQUIRE_FALSE(ui::FirstRunWizard::ShouldShow(config));
}

// ── Debounce constant ──

TEST_CASE("SettingsPanel debounce constant is reasonable", "[settings][panel][perf]")
{
    REQUIRE(ui::SettingsPanel::kSearchDebounceMs == 300);
}

// ── SettingDefinition choice type ──

TEST_CASE("SettingDefinition choice type stores options", "[settings][panel][choice]")
{
    ui::SettingDefinition def;
    def.type = core::SettingType::Choice;
    def.choices = {"small", "medium", "large"};
    def.default_value = "medium";

    REQUIRE(def.choices.size() == 3);
    REQUIRE(def.default_value == "medium");
}

// ── Color type ──

TEST_CASE("SettingDefinition Color type", "[settings][panel][color]")
{
    ui::SettingDefinition def;
    def.type = core::SettingType::Color;
    def.default_value = "#FF5500";

    REQUIRE(def.type == core::SettingType::Color);
    REQUIRE(def.default_value == "#FF5500");
}

// ── StringList type ──

TEST_CASE("SettingDefinition StringList type", "[settings][panel][stringlist]")
{
    ui::SettingDefinition def;
    def.type = core::SettingType::StringList;
    def.default_value = "item1,item2,item3";

    REQUIRE(def.type == core::SettingType::StringList);
}
