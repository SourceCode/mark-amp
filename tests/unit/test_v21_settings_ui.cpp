/// @file test_v21_settings_ui.cpp
/// @brief V21 Phase 09 — Tests for Settings UI, Persistence & JSON Editor.

#include "core/SettingsStagedContract.h"
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SettingsStagedContract — stage and dirty detection", "[v21][p09][staged]") {
    SettingsStagedContract contract;
    contract.stage_change("editor.fontSize", "14", "16");
    contract.stage_change("editor.tabSize", "4", "4"); // Not dirty

    CHECK(contract.total_tracked() == 2);
    CHECK(contract.dirty_count() == 1);
    CHECK(contract.has_dirty());

    auto* s = contract.get_staged("editor.fontSize");
    REQUIRE(s != nullptr);
    CHECK(s->is_dirty());
    CHECK(s->staged_value == "16");
}

TEST_CASE("SettingsStagedContract — apply all", "[v21][p09][apply]") {
    SettingsStagedContract contract;
    std::vector<std::pair<std::string, std::string>> applied_log;
    contract.set_apply_callback([&](const std::string& id, const std::string& val) {
        applied_log.emplace_back(id, val);
    });

    contract.stage_change("editor.fontSize", "14", "16");
    contract.stage_change("editor.tabSize", "4", "2");

    auto applied = contract.apply_all();
    CHECK(applied.size() == 2);
    CHECK(applied_log.size() == 2);
    CHECK(!contract.has_dirty()); // All applied
}

TEST_CASE("SettingsStagedContract — cancel and revert", "[v21][p09][staged]") {
    SettingsStagedContract contract;
    contract.stage_change("editor.fontSize", "14", "16");
    contract.stage_change("editor.tabSize", "4", "2");

    CHECK(contract.has_dirty());
    contract.cancel_all();
    CHECK(!contract.has_dirty());

    // Re-stage and revert single
    contract.stage_change("editor.fontSize", "14", "16");
    contract.revert("editor.fontSize");
    CHECK(!contract.get_staged("editor.fontSize")->is_dirty());
}

TEST_CASE("SettingsStagedContract — JSON validation", "[v21][p09][json]") {
    auto valid = SettingsStagedContract::validate_json(R"({"key": "value"})");
    CHECK(valid.is_valid);

    auto empty = SettingsStagedContract::validate_json("");
    CHECK(!empty.is_valid);

    auto unbalanced = SettingsStagedContract::validate_json(R"({"key": "value")");
    CHECK(!unbalanced.is_valid);

    auto extra_close = SettingsStagedContract::validate_json(R"(})");
    CHECK(!extra_close.is_valid);
}

TEST_CASE("SettingsStagedContract — import/export", "[v21][p09][io]") {
    SettingsStagedContract contract;
    contract.stage_change("editor.fontSize", "14", "16");
    contract.stage_change("editor.tabSize", "4", "2");

    auto exported = contract.export_staged();
    CHECK(exported.size() == 2);

    SettingsStagedContract other;
    other.import_values(exported);
    CHECK(other.total_tracked() == 2);
}

TEST_CASE("SettingsStagedContract — search", "[v21][p09][search]") {
    SettingsStagedContract contract;
    contract.register_searchable("editor.fontSize", "Font Size", "Editor");
    contract.register_searchable("editor.tabSize", "Tab Size", "Editor");
    contract.register_searchable("theme.name", "Color Theme", "Appearance");

    auto results = contract.search("Font");
    REQUIRE(results.size() == 1);
    CHECK(results[0].setting_id == "editor.fontSize");

    auto category_results = contract.search("Editor");
    CHECK(category_results.size() == 2);

    auto no_results = contract.search("nonexistent");
    CHECK(no_results.empty());
}

TEST_CASE("Integration — staged settings lifecycle", "[v21][p09][integration]") {
    SettingsStagedContract contract;
    std::vector<std::string> applied_ids;
    contract.set_apply_callback([&](const std::string& id, const std::string&) {
        applied_ids.push_back(id);
    });

    // Register searchable + stage changes
    contract.register_searchable("editor.fontSize", "Font Size", "Editor");
    contract.stage_change("editor.fontSize", "14", "16");
    contract.stage_change("editor.tabSize", "4", "2");

    // Dirty state
    CHECK(contract.has_dirty());
    CHECK(contract.dirty_count() == 2);

    // Search while dirty
    auto results = contract.search("Font");
    CHECK(results.size() == 1);

    // Apply
    auto applied = contract.apply_all();
    CHECK(applied.size() == 2);
    CHECK(applied_ids.size() == 2);
    CHECK(!contract.has_dirty());

    // Verify JSON validation works
    CHECK(SettingsStagedContract::validate_json(R"({"fontSize": 16})").is_valid);
}
