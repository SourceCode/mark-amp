/// @file test_v24_p13_settings_completion.cpp
/// @brief V24 Phase 13 tests: Settings catalog, workspace scope, search.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/SettingsCatalog.h"

using namespace markamp::core;

// P13-T01: Settings catalog registration
TEST_CASE("P13-T01 catalog entry defaults", "[v24][p13]") {
    CatalogEntry entry;
    entry.setting_id = "editor.fontSize";
    entry.label = "Font Size";
    entry.group = "Editor";
    entry.scope = ConfigScope::kApplication;
    REQUIRE(entry.setting_id == "editor.fontSize");
    REQUIRE(entry.scope == ConfigScope::kApplication);
}

TEST_CASE("P13-T01 register and find setting", "[v24][p13]") {
    SettingsCatalog catalog;
    CatalogEntry e;
    e.setting_id = "editor.fontSize";
    e.label = "Font Size";
    e.group = "Editor";
    e.default_value = "14";
    catalog.register_setting(e);
    REQUIRE(catalog.size() == 1);
    auto* found = catalog.find_setting("editor.fontSize");
    REQUIRE(found != nullptr);
    REQUIRE(found->label == "Font Size");
}

// P13-T02: Workspace vs application scope
TEST_CASE("P13-T02 config scope values", "[v24][p13]") {
    REQUIRE(ConfigScope::kApplication != ConfigScope::kWorkspace);
    REQUIRE(ConfigScope::kWorkspace != ConfigScope::kProject);
}

TEST_CASE("P13-T02 workspace scoped setting", "[v24][p13]") {
    SettingsCatalog catalog;
    CatalogEntry e;
    e.setting_id = "workspace.path";
    e.label = "Workspace Path";
    e.group = "Workspace";
    e.scope = ConfigScope::kWorkspace;
    catalog.register_setting(e);
    auto* found = catalog.find_setting("workspace.path");
    REQUIRE(found->scope == ConfigScope::kWorkspace);
}

// P13-T03: Settings groups
TEST_CASE("P13-T03 settings grouped query", "[v24][p13]") {
    SettingsCatalog catalog;
    CatalogEntry e1; e1.setting_id = "editor.font"; e1.label = "Font"; e1.group = "Editor";
    CatalogEntry e2; e2.setting_id = "editor.tab"; e2.label = "Tab Size"; e2.group = "Editor";
    CatalogEntry e3; e3.setting_id = "theme.name"; e3.label = "Theme"; e3.group = "Appearance";
    catalog.register_setting(e1);
    catalog.register_setting(e2);
    catalog.register_setting(e3);

    auto editor = catalog.settings_for_group("Editor");
    REQUIRE(editor.size() == 2);
    auto groups = catalog.groups();
    REQUIRE(groups.size() == 2);
}

TEST_CASE("P13-T03 settings search", "[v24][p13]") {
    SettingsCatalog catalog;
    CatalogEntry e; e.setting_id = "editor.fontSize"; e.label = "Font Size"; e.group = "Editor";
    catalog.register_setting(e);
    auto results = catalog.search("font");
    REQUIRE(results.size() >= 1);
}

// P13-T04: Setting metadata
TEST_CASE("P13-T04 deprecated and experimental flags", "[v24][p13]") {
    CatalogEntry e;
    e.setting_id = "old.setting";
    e.deprecated = true;
    e.experimental = false;
    e.restart_required = true;
    REQUIRE(e.deprecated);
    REQUIRE(e.restart_required);
}

TEST_CASE("P13-T04 register batch settings", "[v24][p13]") {
    SettingsCatalog catalog;
    std::vector<CatalogEntry> entries;
    CatalogEntry e1; e1.setting_id = "a"; e1.label = "A"; e1.group = "G";
    CatalogEntry e2; e2.setting_id = "b"; e2.label = "B"; e2.group = "G";
    entries.push_back(e1);
    entries.push_back(e2);
    catalog.register_settings(entries);
    REQUIRE(catalog.size() == 2);
}

// P13-T05: Not found
TEST_CASE("P13-T05 find missing setting", "[v24][p13]") {
    SettingsCatalog catalog;
    REQUIRE(catalog.find_setting("missing.key") == nullptr);
}
