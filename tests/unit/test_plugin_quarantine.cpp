/// test_plugin_quarantine.cpp — V7 Phase 25: Plugin quarantine tests

#include "core/PluginQuarantine.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// Basic quarantine operations
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("PluginQuarantine: not quarantined by default", "[quarantine]")
{
    PluginQuarantine quarantine;
    REQUIRE_FALSE(quarantine.is_quarantined("test.plugin"));
}

TEST_CASE("PluginQuarantine: quarantine and check", "[quarantine]")
{
    PluginQuarantine quarantine;
    quarantine.quarantine("bad.plugin", "Crashed 3 times", 3);
    REQUIRE(quarantine.is_quarantined("bad.plugin"));
    REQUIRE_FALSE(quarantine.is_quarantined("good.plugin"));
}

TEST_CASE("PluginQuarantine: get_entry returns details", "[quarantine]")
{
    PluginQuarantine quarantine;
    quarantine.quarantine("bad.plugin", "Crashed 3 times", 3, true);

    auto entry = quarantine.get_entry("bad.plugin");
    REQUIRE(entry.has_value());
    REQUIRE(entry->plugin_id == "bad.plugin");
    REQUIRE(entry->reason == "Crashed 3 times");
    REQUIRE(entry->crash_count == 3);
    REQUIRE(entry->manual == true);
    REQUIRE(entry->quarantined_at_ms > 0);
}

TEST_CASE("PluginQuarantine: get_entry returns nullopt for non-quarantined", "[quarantine]")
{
    PluginQuarantine quarantine;
    auto entry = quarantine.get_entry("nonexistent");
    REQUIRE_FALSE(entry.has_value());
}

TEST_CASE("PluginQuarantine: unquarantine removes entry", "[quarantine]")
{
    PluginQuarantine quarantine;
    quarantine.quarantine("bad.plugin", "test");
    REQUIRE(quarantine.is_quarantined("bad.plugin"));

    quarantine.unquarantine("bad.plugin");
    REQUIRE_FALSE(quarantine.is_quarantined("bad.plugin"));
}

TEST_CASE("PluginQuarantine: all_entries returns all", "[quarantine]")
{
    PluginQuarantine quarantine;
    quarantine.quarantine("plugin.a", "reason a");
    quarantine.quarantine("plugin.b", "reason b");

    auto entries = quarantine.all_entries();
    REQUIRE(entries.size() == 2);
}

TEST_CASE("PluginQuarantine: clear removes all entries", "[quarantine]")
{
    PluginQuarantine quarantine;
    quarantine.quarantine("plugin.a", "test");
    quarantine.quarantine("plugin.b", "test");
    quarantine.clear();

    REQUIRE_FALSE(quarantine.is_quarantined("plugin.a"));
    REQUIRE_FALSE(quarantine.is_quarantined("plugin.b"));
    REQUIRE(quarantine.all_entries().empty());
}

// ══════════════════════════════════════════════════════════════════════════════
// Crash counting and auto-quarantine
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("PluginQuarantine: crash count starts at zero", "[quarantine]")
{
    PluginQuarantine quarantine;
    REQUIRE(quarantine.crash_count("test.plugin") == 0);
}

TEST_CASE("PluginQuarantine: record_crash increments count", "[quarantine]")
{
    PluginQuarantine quarantine;
    quarantine.record_crash("test.plugin", 5);
    REQUIRE(quarantine.crash_count("test.plugin") == 1);
    quarantine.record_crash("test.plugin", 5);
    REQUIRE(quarantine.crash_count("test.plugin") == 2);
}

TEST_CASE("PluginQuarantine: auto-quarantine on threshold", "[quarantine]")
{
    PluginQuarantine quarantine;
    REQUIRE_FALSE(quarantine.record_crash("test.plugin", 3));
    REQUIRE_FALSE(quarantine.record_crash("test.plugin", 3));
    REQUIRE(quarantine.record_crash("test.plugin", 3)); // 3rd crash triggers quarantine

    REQUIRE(quarantine.is_quarantined("test.plugin"));
    auto entry = quarantine.get_entry("test.plugin");
    REQUIRE(entry.has_value());
    REQUIRE_FALSE(entry->manual);
    REQUIRE(entry->crash_count == 3);
}

TEST_CASE("PluginQuarantine: no double auto-quarantine", "[quarantine]")
{
    PluginQuarantine quarantine;
    quarantine.record_crash("test.plugin", 2);
    REQUIRE(quarantine.record_crash("test.plugin", 2));       // Quarantined!
    REQUIRE_FALSE(quarantine.record_crash("test.plugin", 2)); // Already quarantined
}

// ══════════════════════════════════════════════════════════════════════════════
// Persistence
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("PluginQuarantine: save to file", "[quarantine]")
{
    auto temp_path = std::filesystem::temp_directory_path() / "test_quarantine.json";

    PluginQuarantine quarantine;
    quarantine.set_persistence_path(temp_path);
    quarantine.quarantine("test.plugin", "test reason");

    auto result = quarantine.save();
    REQUIRE(result.has_value());
    REQUIRE(std::filesystem::exists(temp_path));

    // Cleanup
    std::filesystem::remove(temp_path);
}

TEST_CASE("PluginQuarantine: save fails without path", "[quarantine]")
{
    PluginQuarantine quarantine;
    auto result = quarantine.save();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::IoError);
}
