/// @file test_v25_p13_settings_schema.cpp
/// @brief V25 Phase 13: Settings schema consolidator tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P13: Register settings entries", "[v25][p13]")
{
    SettingsSchemaConsolidator schema;
    SettingsSchemaEntry e;
    e.key = "editor.fontSize";
    e.default_value = "14";
    REQUIRE(schema.register_entry(e));
    REQUIRE(schema.entry_count() == 1);
}

TEST_CASE("V25 P13: Detect duplicate keys", "[v25][p13]")
{
    SettingsSchemaConsolidator schema;
    SettingsSchemaEntry e1; e1.key = "editor.fontSize";
    SettingsSchemaEntry e2; e2.key = "editor.fontSize";
    schema.register_entry(e1);
    schema.register_entry(e2);
    REQUIRE(schema.has_duplicates());
    REQUIRE(schema.duplicate_entries().size() >= 1);
}

TEST_CASE("V25 P13: Lookup by key", "[v25][p13]")
{
    SettingsSchemaConsolidator schema;
    SettingsSchemaEntry e; e.key = "editor.tabSize"; e.default_value = "4";
    schema.register_entry(e);
    auto* found = schema.get_entry("editor.tabSize");
    REQUIRE(found != nullptr);
    REQUIRE(found->default_value == "4");
}
