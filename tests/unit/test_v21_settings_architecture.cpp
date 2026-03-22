/// @file test_v21_settings_architecture.cpp
/// @brief V21 Phase 08 — Tests for Settings Architecture & Schema Consolidation.

#include "core/SettingsArchitectureAuditor.h"
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SettingScope — labels", "[v21][p08][settings]") {
    CHECK(std::string(setting_scope_label(SettingScope::kUser)) == "User");
    CHECK(std::string(setting_scope_label(SettingScope::kWorkspace)) == "Workspace");
}

TEST_CASE("SettingApplicationMode — labels", "[v21][p08][settings]") {
    CHECK(std::string(application_mode_label(SettingApplicationMode::kLive)) == "Live");
    CHECK(std::string(application_mode_label(SettingApplicationMode::kOnRestart)) == "OnRestart");
}

TEST_CASE("SettingsArchitectureAuditor — catalog registration", "[v21][p08][catalog]") {
    SettingsArchitectureAuditor auditor;
    auditor.register_setting({.setting_id = "editor.fontSize", .label = "Font Size",
        .category = "Editor", .default_value = "14", .application_mode = SettingApplicationMode::kLive,
        .deep_link = "settings://editor.fontSize"});
    auditor.register_setting({.setting_id = "editor.tabSize", .label = "Tab Size",
        .category = "Editor", .default_value = "4", .deep_link = "settings://editor.tabSize"});
    auditor.register_setting({.setting_id = "theme.name", .label = "Theme",
        .category = "Appearance", .default_value = "Dark+",
        .application_mode = SettingApplicationMode::kOnRestart,
        .deep_link = "settings://theme.name"});

    CHECK(auditor.setting_count() == 3);
    CHECK(auditor.settings_for_category("Editor").size() == 2);
    CHECK(auditor.categories().size() == 2);

    auto* s = auditor.get_setting("editor.fontSize");
    REQUIRE(s != nullptr);
    CHECK(s->default_value == "14");
}

TEST_CASE("SettingsArchitectureAuditor — scope precedence", "[v21][p08][scope]") {
    SettingsArchitectureAuditor auditor;
    auditor.register_setting({.setting_id = "editor.fontSize", .default_value = "14"});

    CHECK(auditor.effective_value("editor.fontSize") == "14");
    CHECK(auditor.effective_scope("editor.fontSize") == SettingScope::kDefault);

    auditor.set_value("editor.fontSize", SettingScope::kUser, "16");
    CHECK(auditor.effective_value("editor.fontSize") == "16");
    CHECK(auditor.effective_scope("editor.fontSize") == SettingScope::kUser);

    auditor.set_value("editor.fontSize", SettingScope::kWorkspace, "12");
    CHECK(auditor.effective_value("editor.fontSize") == "12"); // Workspace wins
    CHECK(auditor.effective_scope("editor.fontSize") == SettingScope::kWorkspace);

    auditor.set_value("editor.fontSize", SettingScope::kProject, "18");
    CHECK(auditor.effective_value("editor.fontSize") == "18"); // Project wins
}

TEST_CASE("SettingsArchitectureAuditor — ownership tracking", "[v21][p08][ownership]") {
    SettingsArchitectureAuditor auditor;
    auditor.record_ownership("editor.fontSize", "SettingsStateOwner", false);
    auditor.record_ownership("theme.name", "ThemeEngine", true); // Direct write!

    auto writers = auditor.direct_writers();
    REQUIRE(writers.size() == 1);
    CHECK(writers[0].setting_id == "theme.name");
    CHECK(writers[0].owner == "ThemeEngine");
}

TEST_CASE("SettingsArchitectureAuditor — deep-link routing", "[v21][p08][deeplink]") {
    SettingsArchitectureAuditor auditor;
    auditor.register_setting({.setting_id = "editor.fontSize", .deep_link = "settings://editor.fontSize"});
    auditor.register_setting({.setting_id = "editor.tabSize"}); // No deep link

    CHECK(auditor.resolve_deep_link("editor.fontSize") == "settings://editor.fontSize");
    CHECK(auditor.resolve_deep_link("editor.tabSize").empty());
    CHECK(auditor.settings_missing_deep_link().size() == 1);
}

TEST_CASE("SettingsArchitectureAuditor — application mode", "[v21][p08][settings]") {
    SettingsArchitectureAuditor auditor;
    auditor.register_setting({.setting_id = "editor.fontSize",
        .application_mode = SettingApplicationMode::kLive});
    auditor.register_setting({.setting_id = "theme.name",
        .application_mode = SettingApplicationMode::kOnRestart});

    CHECK(auditor.live_settings().size() == 1);
    CHECK(auditor.restart_required_settings().size() == 1);
}

TEST_CASE("SettingsArchitectureAuditor — diagnostics", "[v21][p08][settings]") {
    SettingsArchitectureAuditor auditor;
    auditor.register_setting({.setting_id = "editor.fontSize", .deep_link = "settings://editor.fontSize"});
    auditor.register_setting({.setting_id = "old.setting", .is_deprecated = true});
    auditor.record_ownership("editor.fontSize", "SomePanel", true);

    auto diags = auditor.diagnose();
    bool found_deprecated = false, found_direct = false, found_no_link = false;
    for (const auto& d : diags) {
        if (d.is_deprecated) found_deprecated = true;
        if (d.is_direct_write) found_direct = true;
        if (d.is_missing_deep_link) found_no_link = true;
    }
    CHECK(found_deprecated);
    CHECK(found_direct);
    CHECK(found_no_link); // old.setting has no deep link
}

TEST_CASE("Integration — settings architecture lifecycle", "[v21][p08][integration]") {
    SettingsArchitectureAuditor auditor;

    // Register catalog
    auditor.register_setting({.setting_id = "editor.fontSize", .category = "Editor",
        .default_value = "14", .application_mode = SettingApplicationMode::kLive,
        .deep_link = "settings://editor.fontSize"});
    auditor.register_setting({.setting_id = "editor.tabSize", .category = "Editor",
        .default_value = "4", .application_mode = SettingApplicationMode::kLive,
        .deep_link = "settings://editor.tabSize"});

    // Set scoped values
    auditor.set_value("editor.fontSize", SettingScope::kUser, "16");
    auditor.set_value("editor.fontSize", SettingScope::kWorkspace, "12");
    CHECK(auditor.effective_value("editor.fontSize") == "12");

    // Record ownership
    auditor.record_ownership("editor.fontSize", "SettingsStateOwner", false);

    // Diagnostics clean
    auto diags = auditor.diagnose();
    bool found_critical = false;
    for (const auto& d : diags)
        if (d.is_direct_write || d.is_orphaned) found_critical = true;
    CHECK(!found_critical);
}
