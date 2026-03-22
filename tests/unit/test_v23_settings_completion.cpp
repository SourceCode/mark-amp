/// @file test_v23_settings_completion.cpp
/// @brief V23 Phase 10 — Tests for SettingsCompletionAuditor.

#include "core/SettingsCompletionAuditor.h"
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SettingsCapabilityArea — labels", "[v23][p10][label]")
{
    CHECK(std::string(settings_capability_label(SettingsCapabilityArea::kStateOwnership)) == "StateOwnership");
    CHECK(std::string(settings_capability_label(SettingsCapabilityArea::kDeepLinks)) == "DeepLinks");
    CHECK(std::string(settings_capability_label(SettingsCapabilityArea::kWorkspaceScope)) == "WorkspaceScope");
    CHECK(std::string(settings_capability_label(SettingsCapabilityArea::kMigration)) == "Migration");
}

TEST_CASE("SettingsCompletionItem — completeness", "[v23][p10][item]")
{
    SettingsCompletionItem item;
    CHECK(!item.is_complete());
    item.is_sole_authority = true;
    item.has_error_handling = true;
    CHECK(item.is_complete());
}

TEST_CASE("SettingsCompletionAuditor — registration", "[v23][p10][auditor]")
{
    SettingsCompletionAuditor auditor;
    SettingsCompletionItem item;
    auditor.add_item(std::move(item));
    CHECK(auditor.item_count() == 1);
}

TEST_CASE("SettingsCompletionAuditor — query by area", "[v23][p10][query]")
{
    SettingsCompletionAuditor auditor;
    SettingsCompletionItem ownership;
    ownership.area = SettingsCapabilityArea::kStateOwnership;
    auditor.add_item(std::move(ownership));
    SettingsCompletionItem deep;
    deep.area = SettingsCapabilityArea::kDeepLinks;
    auditor.add_item(std::move(deep));

    CHECK(auditor.items_by_area(SettingsCapabilityArea::kStateOwnership).size() == 1);
    CHECK(auditor.items_by_area(SettingsCapabilityArea::kMigration).empty());
}

TEST_CASE("SettingsCompletionAuditor — complete vs incomplete", "[v23][p10][query]")
{
    SettingsCompletionAuditor auditor;
    SettingsCompletionItem ok;
    ok.is_sole_authority = true;
    ok.has_error_handling = true;
    auditor.add_item(std::move(ok));
    SettingsCompletionItem gap;
    auditor.add_item(std::move(gap));

    CHECK(auditor.complete_items().size() == 1);
    CHECK(auditor.incomplete_items().size() == 1);
}

TEST_CASE("SettingsCompletionAuditor — report", "[v23][p10][report]")
{
    SettingsCompletionAuditor auditor;
    SettingsCompletionItem ok;
    ok.is_sole_authority = true;
    ok.has_error_handling = true;
    auditor.add_item(std::move(ok));

    auto r = auditor.report();
    CHECK(r.total == 1);
    CHECK(r.complete == 1);
    CHECK(!r.has_gaps());
    CHECK(r.coverage_pct() == 100.0);
}

TEST_CASE("SettingsCompletionAuditor — clear", "[v23][p10][auditor]")
{
    SettingsCompletionAuditor auditor;
    SettingsCompletionItem item;
    auditor.add_item(std::move(item));
    auditor.clear();
    CHECK(auditor.item_count() == 0);
}

TEST_CASE("SettingsCompletionAuditor — export", "[v23][p10][export]")
{
    SettingsCompletionAuditor auditor;
    SettingsCompletionItem item;
    item.is_sole_authority = true;
    item.has_error_handling = true;
    auditor.add_item(std::move(item));
    CHECK(auditor.export_json().find("\"complete\": 1") != std::string::npos);
    CHECK(auditor.export_markdown().find("Settings Completion") != std::string::npos);
}

TEST_CASE("Integration — settings auditor lifecycle", "[v23][p10][integration]")
{
    SettingsCompletionAuditor auditor;

    for (auto area : {SettingsCapabilityArea::kStateOwnership, SettingsCapabilityArea::kImportExport,
                      SettingsCapabilityArea::kSchemaValidation})
    {
        SettingsCompletionItem item;
        item.area = area;
        item.is_sole_authority = true;
        item.has_ui_sync = true;
        item.has_error_handling = true;
        auditor.add_item(std::move(item));
    }

    SettingsCompletionItem deep_gap;
    deep_gap.area = SettingsCapabilityArea::kDeepLinks;
    auditor.add_item(std::move(deep_gap));

    SettingsCompletionItem migration_gap;
    migration_gap.area = SettingsCapabilityArea::kMigration;
    auditor.add_item(std::move(migration_gap));

    CHECK(auditor.item_count() == 5);
    CHECK(auditor.complete_items().size() == 3);
    CHECK(auditor.incomplete_items().size() == 2);
    auto r = auditor.report();
    CHECK(r.coverage_pct() == 60.0);
    CHECK(r.has_gaps());
}
