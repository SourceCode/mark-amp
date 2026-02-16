#include "core/IPlugin.h"
#include "core/SettingsCatalog.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SettingsCatalog registration and lookup", "[settings][catalog]")
{
    SettingsCatalog catalog;

    SECTION("empty catalog")
    {
        REQUIRE(catalog.size() == 0);
        REQUIRE(catalog.groups().empty());
        REQUIRE(catalog.find_setting("nonexistent") == nullptr);
    }

    SECTION("register and find a single setting")
    {
        CatalogEntry entry;
        entry.setting_id = "editor.fontSize";
        entry.label = "Font Size";
        entry.description = "Controls font size in pixels";
        entry.group = "Editor";
        entry.subgroup = "Font";
        entry.type = SettingType::Integer;
        entry.default_value = "13";
        entry.min_int = 8;
        entry.max_int = 72;

        catalog.register_setting(entry);
        REQUIRE(catalog.size() == 1);

        const auto* found = catalog.find_setting("editor.fontSize");
        REQUIRE(found != nullptr);
        REQUIRE(found->label == "Font Size");
        REQUIRE(found->group == "Editor");
        REQUIRE(found->subgroup == "Font");
        REQUIRE(found->default_value == "13");
        REQUIRE(found->min_int == 8);
        REQUIRE(found->max_int == 72);
    }

    SECTION("register multiple settings")
    {
        std::vector<CatalogEntry> entries;
        CatalogEntry entry_a;
        entry_a.setting_id = "editor.tabSize";
        entry_a.label = "Tab Size";
        entry_a.group = "Editor";
        entry_a.type = SettingType::Integer;
        entry_a.default_value = "4";
        entries.push_back(entry_a);

        CatalogEntry entry_b;
        entry_b.setting_id = "editor.wordWrap";
        entry_b.label = "Word Wrap";
        entry_b.group = "Editor";
        entry_b.type = SettingType::Boolean;
        entry_b.default_value = "true";
        entries.push_back(entry_b);

        catalog.register_settings(std::move(entries));
        REQUIRE(catalog.size() == 2);
        REQUIRE(catalog.find_setting("editor.tabSize") != nullptr);
        REQUIRE(catalog.find_setting("editor.wordWrap") != nullptr);
    }
}

TEST_CASE("SettingsCatalog group filtering", "[settings][catalog]")
{
    SettingsCatalog catalog;

    CatalogEntry editor_entry;
    editor_entry.setting_id = "editor.fontSize";
    editor_entry.label = "Font Size";
    editor_entry.group = "Editor";
    editor_entry.type = SettingType::Integer;
    editor_entry.default_value = "13";
    catalog.register_setting(editor_entry);

    CatalogEntry appearance_entry;
    appearance_entry.setting_id = "appearance.theme";
    appearance_entry.label = "Theme";
    appearance_entry.group = "Appearance";
    appearance_entry.type = SettingType::String;
    appearance_entry.default_value = "dark";
    catalog.register_setting(appearance_entry);

    CatalogEntry editor_entry2;
    editor_entry2.setting_id = "editor.tabSize";
    editor_entry2.label = "Tab Size";
    editor_entry2.group = "Editor";
    editor_entry2.type = SettingType::Integer;
    editor_entry2.default_value = "4";
    catalog.register_setting(editor_entry2);

    SECTION("groups() returns unique ordered names")
    {
        auto groups = catalog.groups();
        REQUIRE(groups.size() == 2);
        // Groups should be in registration order
        REQUIRE(groups[0] == "Editor");
        REQUIRE(groups[1] == "Appearance");
    }

    SECTION("settings_for_group returns correct entries")
    {
        auto editor_settings = catalog.settings_for_group("Editor");
        REQUIRE(editor_settings.size() == 2);
        REQUIRE(editor_settings[0]->setting_id == "editor.fontSize");
        REQUIRE(editor_settings[1]->setting_id == "editor.tabSize");

        auto appearance_settings = catalog.settings_for_group("Appearance");
        REQUIRE(appearance_settings.size() == 1);
        REQUIRE(appearance_settings[0]->setting_id == "appearance.theme");
    }

    SECTION("settings_for_group returns empty for unknown group")
    {
        auto results = catalog.settings_for_group("Unknown");
        REQUIRE(results.empty());
    }
}

TEST_CASE("SettingsCatalog keyword search", "[settings][catalog]")
{
    SettingsCatalog catalog;

    CatalogEntry entry_a;
    entry_a.setting_id = "editor.fontSize";
    entry_a.label = "Font Size";
    entry_a.description = "Controls the editor font size in pixels";
    entry_a.group = "Editor";
    entry_a.keywords = {"typeface", "text"};
    entry_a.type = SettingType::Integer;
    entry_a.default_value = "13";
    catalog.register_setting(entry_a);

    CatalogEntry entry_b;
    entry_b.setting_id = "editor.fontFamily";
    entry_b.label = "Font Family";
    entry_b.description = "Controls the editor font family";
    entry_b.group = "Editor";
    entry_b.keywords = {"typeface", "monospace"};
    entry_b.type = SettingType::String;
    entry_b.default_value = "Menlo";
    catalog.register_setting(entry_b);

    CatalogEntry entry_c;
    entry_c.setting_id = "appearance.theme";
    entry_c.label = "Color Theme";
    entry_c.description = "Specifies the color theme for the editor";
    entry_c.group = "Appearance";
    entry_c.keywords = {"dark", "light"};
    entry_c.type = SettingType::String;
    entry_c.default_value = "midnight-neon";
    catalog.register_setting(entry_c);

    SECTION("search by label")
    {
        auto results = catalog.search("Font");
        REQUIRE(results.size() == 2);
    }

    SECTION("search by description")
    {
        auto results = catalog.search("pixels");
        REQUIRE(results.size() == 1);
        REQUIRE(results[0]->setting_id == "editor.fontSize");
    }

    SECTION("search by keyword")
    {
        auto results = catalog.search("typeface");
        REQUIRE(results.size() == 2);
    }

    SECTION("search is case-insensitive")
    {
        auto results = catalog.search("FONT");
        REQUIRE(results.size() == 2);
    }

    SECTION("search with no matches")
    {
        auto results = catalog.search("nonexistent_query_xyz");
        REQUIRE(results.empty());
    }
}

TEST_CASE("SettingsCatalog plugin ingestion", "[settings][catalog]")
{
    SettingsCatalog catalog;

    std::vector<SettingContribution> contributions;
    SettingContribution contrib;
    contrib.id = "plugin.myPlugin.enabled";
    contrib.label = "Enable My Plugin";
    contrib.description = "Toggle the my-plugin extension";
    contrib.category = "Plugins";
    contrib.type = SettingType::Boolean;
    contrib.default_value = "true";
    contributions.push_back(contrib);

    catalog.ingest_plugin_settings(contributions);

    REQUIRE(catalog.size() == 1);
    const auto* found = catalog.find_setting("plugin.myPlugin.enabled");
    REQUIRE(found != nullptr);
    REQUIRE(found->label == "Enable My Plugin");
    REQUIRE(found->group == "Plugins");
    REQUIRE(found->source == "plugin");
}

TEST_CASE("SettingsCatalog builtins registration", "[settings][catalog]")
{
    SettingsCatalog catalog;
    catalog.register_builtins();

    // Builtins should register many settings
    REQUIRE(catalog.size() > 50);

    // Check a few well-known builtins exist
    REQUIRE(catalog.find_setting("editor.fontSize") != nullptr);
    REQUIRE(catalog.find_setting("editor.tabSize") != nullptr);
    REQUIRE(catalog.find_setting("editor.wordWrap") != nullptr);

    // Check groups are populated
    auto groups = catalog.groups();
    REQUIRE(groups.size() >= 3);
}
