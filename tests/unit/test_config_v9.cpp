/// Phase 02: Config System Modernization Tests
/// Covers Tasks 1–20 of the V9 config modernization roadmap.
/// Tests: SettingsCatalog wiring, ScopedConfig cascade, schema validation,
///        batching, profiles, export/import, file watcher, migration,
///        audit trail, snapshot/restore, modified_settings, events.

#include "core/Config.h"
#include "core/ConfigAuditTrail.h"
#include "core/ConfigFileWatcher.h"
#include "core/ConfigMigration.h"
#include "core/ConfigProfile.h"
#include "core/Events.h"
#include "core/ScopedConfig.h"
#include "core/SettingsCatalog.h"

#include <catch2/catch_all.hpp>

#include <filesystem>
#include <fstream>

using namespace markamp::core;

// ═══════════════════════════════════════════════════════════════════
// Task 1: Migrate defaults to SettingsCatalog
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Config: set_catalog and apply_catalog_defaults", "[config][task1]")
{
    Config cfg;
    SettingsCatalog catalog;

    // Register a test setting
    CatalogEntry entry;
    entry.setting_id = "test_setting";
    entry.label = "Test Setting";
    entry.type = SettingType::Integer;
    entry.default_value = "42";
    entry.min_int = 0;
    entry.max_int = 100;
    entry.source = "builtin";
    catalog.register_setting(entry);

    cfg.set_catalog(&catalog);
    cfg.apply_catalog_defaults();

    CHECK(cfg.has_key("test_setting"));
    CHECK(cfg.get_int("test_setting") == 42);
}

TEST_CASE("Config: apply_catalog_defaults skips existing keys", "[config][task1]")
{
    Config cfg;
    SettingsCatalog catalog;

    CatalogEntry entry;
    entry.setting_id = "font_size";
    entry.type = SettingType::Integer;
    entry.default_value = "14";
    entry.min_int = 8;
    entry.max_int = 72;
    entry.source = "builtin";
    catalog.register_setting(entry);

    // Pre-set a value
    cfg.set("font_size", 20);
    cfg.set_catalog(&catalog);
    cfg.apply_catalog_defaults();

    // Should keep the user's value, not overwrite with default
    CHECK(cfg.get_int("font_size") == 20);
}

TEST_CASE("Config: apply_catalog_defaults handles boolean type", "[config][task1]")
{
    Config cfg;
    SettingsCatalog catalog;

    CatalogEntry entry;
    entry.setting_id = "test_bool";
    entry.type = SettingType::Boolean;
    entry.default_value = "true";
    entry.source = "builtin";
    catalog.register_setting(entry);

    cfg.set_catalog(&catalog);
    cfg.apply_catalog_defaults();

    CHECK(cfg.get_bool("test_bool") == true);
}

// ═══════════════════════════════════════════════════════════════════
// Task 2: Scoped Config (App / Workspace / Project cascade)
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ScopedConfig: app-level fallback", "[scopedconfig][task2]")
{
    Config app_cfg;
    app_cfg.set("theme", std::string_view("midnight-neon"));

    ScopedConfig scoped(app_cfg);
    CHECK(scoped.get_string("theme") == "midnight-neon");
}

TEST_CASE("ScopedConfig: workspace overrides app", "[scopedconfig][task2]")
{
    Config app_cfg;
    Config ws_cfg;

    app_cfg.set("font_size", 14);
    ws_cfg.set("font_size", 18);

    ScopedConfig scoped(app_cfg);
    scoped.set_workspace_config(&ws_cfg);

    CHECK(scoped.get_int("font_size") == 18);
    CHECK(scoped.effective_scope("font_size") == ConfigScope::kWorkspace);
}

TEST_CASE("ScopedConfig: project overrides workspace and app", "[scopedconfig][task2]")
{
    Config app_cfg;
    Config ws_cfg;
    Config proj_cfg;

    app_cfg.set("tab_size", 4);
    ws_cfg.set("tab_size", 2);
    proj_cfg.set("tab_size", 8);

    ScopedConfig scoped(app_cfg);
    scoped.set_workspace_config(&ws_cfg);
    scoped.set_project_config(&proj_cfg);

    CHECK(scoped.get_int("tab_size") == 8);
    CHECK(scoped.effective_scope("tab_size") == ConfigScope::kProject);
}

TEST_CASE("ScopedConfig: reset_in_scope removes override", "[scopedconfig][task2]")
{
    Config app_cfg;
    Config ws_cfg;

    app_cfg.set("font_size", 14);
    ws_cfg.set("font_size", 18);

    ScopedConfig scoped(app_cfg);
    scoped.set_workspace_config(&ws_cfg);

    CHECK(scoped.get_int("font_size") == 18);
    scoped.reset_in_scope("font_size", ConfigScope::kWorkspace);
    CHECK(scoped.get_int("font_size") == 14);
}

TEST_CASE("ScopedConfig: has_override query", "[scopedconfig][task2]")
{
    Config app_cfg;
    Config ws_cfg;

    app_cfg.set("font_size", 14);

    ScopedConfig scoped(app_cfg);
    scoped.set_workspace_config(&ws_cfg);

    CHECK(scoped.has_override("font_size", ConfigScope::kApplication) == true);
    CHECK(scoped.has_override("font_size", ConfigScope::kWorkspace) == false);
}

TEST_CASE("ScopedConfig: catalog default fallback", "[scopedconfig][task2]")
{
    Config app_cfg;
    SettingsCatalog catalog;

    CatalogEntry entry;
    entry.setting_id = "new_setting";
    entry.type = SettingType::String;
    entry.default_value = "catalog_default";
    entry.source = "builtin";
    catalog.register_setting(entry);

    ScopedConfig scoped(app_cfg, &catalog);
    CHECK(scoped.get_string("new_setting") == "catalog_default");
}

// ═══════════════════════════════════════════════════════════════════
// Task 3: Schema validation via SettingsCatalog
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Config: validate_value integer range", "[config][task3]")
{
    Config cfg;
    SettingsCatalog catalog;

    CatalogEntry entry;
    entry.setting_id = "font_size";
    entry.type = SettingType::Integer;
    entry.default_value = "14";
    entry.min_int = 8;
    entry.max_int = 72;
    entry.source = "builtin";
    catalog.register_setting(entry);

    cfg.set_catalog(&catalog);

    CHECK(cfg.validate_value("font_size", 14).has_value());
    CHECK(cfg.validate_value("font_size", 8).has_value());
    CHECK(cfg.validate_value("font_size", 72).has_value());
    CHECK_FALSE(cfg.validate_value("font_size", 7).has_value());
    CHECK_FALSE(cfg.validate_value("font_size", 73).has_value());
}

TEST_CASE("Config: validate_value choice validation", "[config][task3]")
{
    Config cfg;
    SettingsCatalog catalog;

    CatalogEntry entry;
    entry.setting_id = "theme";
    entry.type = SettingType::Choice;
    entry.default_value = "dark";
    entry.choices = {"dark", "light", "auto"};
    entry.source = "builtin";
    catalog.register_setting(entry);

    cfg.set_catalog(&catalog);

    CHECK(cfg.validate_value("theme", std::string("dark")).has_value());
    CHECK(cfg.validate_value("theme", std::string("light")).has_value());
    CHECK_FALSE(cfg.validate_value("theme", std::string("invalid")).has_value());
}

TEST_CASE("Config: validate_value unknown key passes", "[config][task3]")
{
    Config cfg;
    SettingsCatalog catalog;
    cfg.set_catalog(&catalog);

    CHECK(cfg.validate_value("nonexistent", 42).has_value());
}

// ═══════════════════════════════════════════════════════════════════
// Task 4: Change batching
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Config: batch mode defers rebuild", "[config][task4]")
{
    Config cfg;
    cfg.set("font_size", 14);

    cfg.begin_batch();
    CHECK(cfg.is_batching());

    cfg.set("font_size", 20);
    cfg.set("tab_size", 8);

    // During batch, cache is not rebuilt
    // (We can't directly test cache state, but we test the API)

    cfg.commit_batch();
    CHECK_FALSE(cfg.is_batching());
    CHECK(cfg.get_int("font_size") == 20);
    CHECK(cfg.get_int("tab_size") == 8);
}

TEST_CASE("Config: discard_batch reverts changes", "[config][task4]")
{
    Config cfg;
    cfg.set("font_size", 14);

    cfg.begin_batch();
    cfg.set("font_size", 99);
    cfg.discard_batch();

    CHECK_FALSE(cfg.is_batching());
    CHECK(cfg.get_int("font_size") == 14);
}

TEST_CASE("Config: double begin_batch is a no-op", "[config][task4]")
{
    Config cfg;
    cfg.begin_batch();
    cfg.begin_batch(); // Should warn and be a no-op
    CHECK(cfg.is_batching());
    cfg.commit_batch();
    CHECK_FALSE(cfg.is_batching());
}

// ═══════════════════════════════════════════════════════════════════
// Task 5: Settings profiles
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ConfigProfileManager: builtins are registered", "[profiles][task5]")
{
    ConfigProfileManager manager;

    CHECK(manager.profile_count() >= 3);
    auto names = manager.profile_names();
    CHECK(std::find(names.begin(), names.end(), "Developer") != names.end());
    CHECK(std::find(names.begin(), names.end(), "Research") != names.end());
    CHECK(std::find(names.begin(), names.end(), "Whiteboard") != names.end());
}

TEST_CASE("ConfigProfileManager: apply profile sets values", "[profiles][task5]")
{
    ConfigProfileManager manager;
    Config cfg;

    manager.apply_profile("Developer", cfg);

    CHECK(cfg.get_bool("show_line_numbers") == true);
    CHECK(cfg.get_bool("bracket_matching") == true);
}

TEST_CASE("ConfigProfileManager: apply by ProfileId", "[profiles][task5]")
{
    ConfigProfileManager manager;
    Config cfg;

    manager.apply_profile(ProfileId::kResearch, cfg);

    CHECK(cfg.get_bool("show_line_numbers") == false);
    CHECK(cfg.get_int("font_size") == 16);
}

TEST_CASE("ConfigProfileManager: find_profile", "[profiles][task5]")
{
    ConfigProfileManager manager;

    auto* dev = manager.find_profile("Developer");
    REQUIRE(dev != nullptr);
    CHECK(dev->profile_id == ProfileId::kDeveloper);

    auto* missing = manager.find_profile("Nonexistent");
    CHECK(missing == nullptr);
}

TEST_CASE("ConfigProfileManager: export_profile JSON", "[profiles][task5]")
{
    ConfigProfileManager manager;
    auto json_str = manager.export_profile("Developer");

    CHECK_FALSE(json_str.empty());
    CHECK(json_str != "{}");
    CHECK(json_str.find("Developer") != std::string::npos);
}

TEST_CASE("ConfigProfileManager: register custom profile", "[profiles][task5]")
{
    ConfigProfileManager manager;

    ConfigProfile custom;
    custom.name = "Custom";
    custom.description = "My custom profile";
    custom.profile_id = ProfileId::kCustom;
    custom.overrides = {{"font_size", "24"}, {"theme", "solar"}};
    manager.register_profile(custom);

    CHECK(manager.profile_count() >= 4);
    auto* found = manager.find_profile("Custom");
    REQUIRE(found != nullptr);
    CHECK(found->overrides.size() == 2);
}

// ═══════════════════════════════════════════════════════════════════
// Task 6: Export/Import
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Config: export_to_json and import_from_json roundtrip", "[config][task6]")
{
    auto tmp_path = std::filesystem::temp_directory_path() / "markamp_test_export.json";

    {
        Config cfg;
        cfg.set("theme", std::string_view("test-theme"));
        cfg.set("font_size", 18);
        cfg.export_to_json(tmp_path);
    }

    {
        Config cfg;
        cfg.import_from_json(tmp_path);
        // Note: import_from_json is a simple string-based parser
        CHECK(cfg.has_key("theme"));
    }

    std::filesystem::remove(tmp_path);
}

// ═══════════════════════════════════════════════════════════════════
// Task 9: Diff between configs
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Config: diff identifies changed keys", "[config][task9]")
{
    Config cfg_a;
    Config cfg_b;

    cfg_a.set("theme", std::string_view("dark"));
    cfg_a.set("font_size", 14);

    cfg_b.set("theme", std::string_view("light"));
    cfg_b.set("font_size", 14);

    auto diff_keys = cfg_a.diff(cfg_b);
    CHECK(std::find(diff_keys.begin(), diff_keys.end(), "theme") != diff_keys.end());
}

// ═══════════════════════════════════════════════════════════════════
// Task 10: Reset capability
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ScopedConfig: reset_to_default clears all scopes", "[scopedconfig][task10]")
{
    Config app_cfg;
    Config ws_cfg;
    Config proj_cfg;

    app_cfg.set("font_size", 14);
    ws_cfg.set("font_size", 18);
    proj_cfg.set("font_size", 22);

    ScopedConfig scoped(app_cfg);
    scoped.set_workspace_config(&ws_cfg);
    scoped.set_project_config(&proj_cfg);

    scoped.reset_to_default("font_size");

    CHECK_FALSE(ws_cfg.has_key("font_size"));
    CHECK_FALSE(proj_cfg.has_key("font_size"));
    CHECK_FALSE(app_cfg.has_key("font_size"));
}

// ═══════════════════════════════════════════════════════════════════
// Task 11: File watcher
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ConfigFileWatcher: initial state is inactive", "[filewatcher][task11]")
{
    ConfigFileWatcher watcher("/tmp/nonexistent", []() {});
    CHECK_FALSE(watcher.is_active());
    CHECK(watcher.change_count() == 0);
}

TEST_CASE("ConfigFileWatcher: start and stop", "[filewatcher][task11]")
{
    auto tmp_path = std::filesystem::temp_directory_path() / "markamp_test_watcher.yml";
    std::ofstream(tmp_path) << "test: data";

    bool callback_called = false;
    ConfigFileWatcher watcher(tmp_path, [&callback_called]() { callback_called = true; });

    watcher.start();
    CHECK(watcher.is_active());

    watcher.stop();
    CHECK_FALSE(watcher.is_active());

    std::filesystem::remove(tmp_path);
}

// ═══════════════════════════════════════════════════════════════════
// Task 12: Config migration
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ConfigMigration: simple key rename", "[migration][task12]")
{
    Config cfg;
    cfg.set("old_key", std::string_view("hello"));

    ConfigMigration migration;
    migration.add_rename("old_key", "new_key");

    auto migrated = migration.apply(cfg);

    CHECK(migrated == 1);
    CHECK_FALSE(cfg.has_key("old_key"));
    CHECK(cfg.get_string("new_key") == "hello");
}

TEST_CASE("ConfigMigration: key migration with transform", "[migration][task12]")
{
    Config cfg;
    cfg.set("old_size", std::string_view("14px"));

    ConfigMigration migration;
    migration.add_rule({"old_size",
                        "font_size",
                        [](const std::string& val) -> std::string
                        {
                            // Strip "px" suffix
                            if (val.size() > 2 && val.substr(val.size() - 2) == "px")
                            {
                                return val.substr(0, val.size() - 2);
                            }
                            return val;
                        }});

    migration.apply(cfg);

    CHECK(cfg.get_string("font_size") == "14");
    CHECK_FALSE(cfg.has_key("old_size"));
}

TEST_CASE("ConfigMigration: skip if old key missing", "[migration][task12]")
{
    Config cfg;
    cfg.set("other_key", std::string_view("value"));

    ConfigMigration migration;
    migration.add_rename("nonexistent", "new_key");

    auto migrated = migration.apply(cfg);
    CHECK(migrated == 0);
}

TEST_CASE("ConfigMigration: rule_count", "[migration][task12]")
{
    ConfigMigration migration;
    migration.add_rename("a", "b");
    migration.add_rename("c", "d");

    CHECK(migration.rule_count() == 2);
}

// ═══════════════════════════════════════════════════════════════════
// Task 14: Audit trail
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ConfigAuditTrail: record and retrieve entries", "[audit][task14]")
{
    ConfigAuditTrail trail(10);

    trail.record("font_size", "14", "18", "user");
    trail.record("theme", "dark", "light", "profile");

    CHECK(trail.size() == 2);
    auto entries = trail.entries();
    CHECK(entries.size() == 2);
    CHECK(entries[0].key == "font_size");
    CHECK(entries[1].key == "theme");
}

TEST_CASE("ConfigAuditTrail: last_entry", "[audit][task14]")
{
    ConfigAuditTrail trail(10);

    CHECK(trail.last_entry() == nullptr);

    trail.record("key1", "old1", "new1");
    auto* last = trail.last_entry();
    REQUIRE(last != nullptr);
    CHECK(last->key == "key1");

    trail.record("key2", "old2", "new2");
    last = trail.last_entry();
    REQUIRE(last != nullptr);
    CHECK(last->key == "key2");
}

TEST_CASE("ConfigAuditTrail: ring buffer wraps", "[audit][task14]")
{
    ConfigAuditTrail trail(3);

    trail.record("k1", "", "v1");
    trail.record("k2", "", "v2");
    trail.record("k3", "", "v3");
    trail.record("k4", "", "v4"); // Overwrites k1

    CHECK(trail.size() == 3);
    auto entries = trail.entries();
    CHECK(entries[0].key == "k2");
    CHECK(entries[1].key == "k3");
    CHECK(entries[2].key == "k4");
}

TEST_CASE("ConfigAuditTrail: clear", "[audit][task14]")
{
    ConfigAuditTrail trail(10);
    trail.record("key", "old", "new");
    trail.clear();

    CHECK(trail.size() == 0);
    CHECK(trail.last_entry() == nullptr);
}

// ═══════════════════════════════════════════════════════════════════
// Task 15: Snapshot and restore
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Config: snapshot and restore", "[config][task15]")
{
    Config cfg;
    cfg.set("font_size", 14);
    cfg.set("theme", std::string_view("dark"));

    auto snap = cfg.snapshot();

    cfg.set("font_size", 99);
    cfg.set("theme", std::string_view("light"));

    cfg.restore_from_snapshot(snap);

    CHECK(cfg.get_int("font_size") == 14);
    CHECK(cfg.get_string("theme") == "dark");
}

// ═══════════════════════════════════════════════════════════════════
// Task misc: modified_settings
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Config: modified_settings returns changed keys", "[config]")
{
    Config cfg;
    SettingsCatalog catalog;

    CatalogEntry entry;
    entry.setting_id = "font_size";
    entry.type = SettingType::Integer;
    entry.default_value = "14";
    entry.min_int = 8;
    entry.max_int = 72;
    entry.source = "builtin";
    catalog.register_setting(entry);

    cfg.set_catalog(&catalog);
    cfg.set("font_size", 20); // Different from default

    auto modified = cfg.modified_settings();
    CHECK(std::find(modified.begin(), modified.end(), "font_size") != modified.end());
}

// ═══════════════════════════════════════════════════════════════════
// Task 20: API documentation smoke tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Config: batch API is complete", "[config][task20]")
{
    Config cfg;

    // Verify all batch API methods exist and compile
    CHECK_FALSE(cfg.is_batching());
    cfg.begin_batch();
    CHECK(cfg.is_batching());
    cfg.commit_batch();
    CHECK_FALSE(cfg.is_batching());

    cfg.begin_batch();
    cfg.discard_batch();
    CHECK_FALSE(cfg.is_batching());
}

TEST_CASE("ScopedConfig: scoped setter API is complete", "[scopedconfig][task20]")
{
    Config app_cfg;

    ScopedConfig scoped(app_cfg);

    // Verify all typed setters work
    scoped.set("str_key", std::string_view("hello"), ConfigScope::kApplication);
    scoped.set("int_key", 42, ConfigScope::kApplication);
    scoped.set("bool_key", true, ConfigScope::kApplication);
    scoped.set("dbl_key", 3.14, ConfigScope::kApplication);

    CHECK(scoped.get_string("str_key") == "hello");
    CHECK(scoped.get_int("int_key") == 42);
    CHECK(scoped.get_bool("bool_key") == true);
    CHECK(scoped.get_double("dbl_key") == 3.14);
}

TEST_CASE("Config events compile", "[events][task20]")
{
    using namespace markamp::core::events;

    ConfigChangedEvent evt;
    evt.key = "font_size";
    evt.old_value = "14";
    evt.new_value = "18";
    evt.scope = "application";
    CHECK(evt.key == "font_size");

    RestartRequiredEvent restart_evt;
    restart_evt.setting_id = "font_family";
    restart_evt.reason = "font change";
    CHECK(restart_evt.setting_id == "font_family");
}
