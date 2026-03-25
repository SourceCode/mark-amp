/// @file test_v23_lifecycle_completion.cpp
/// @brief V23 Phase 03 — Tests for LifecycleCompletionTracker.

#include "core/LifecycleCompletionTracker.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Label tests
// ============================================================================

TEST_CASE("ArtifactKind — labels", "[v23][p03][label]")
{
    CHECK(std::string(artifact_kind_label(ArtifactKind::kTextFile)) == "TextFile");
    CHECK(std::string(artifact_kind_label(ArtifactKind::kTextFile)) == "TextFile");
}

TEST_CASE("LifecycleStage — labels", "[v23][p03][label]")
{
    CHECK(std::string(lifecycle_stage_label(LifecycleStage::kCreation)) == "Creation");
    CHECK(std::string(lifecycle_stage_label(LifecycleStage::kOpen)) == "Open");
    CHECK(std::string(lifecycle_stage_label(LifecycleStage::kSave)) == "Save");
    CHECK(std::string(lifecycle_stage_label(LifecycleStage::kRestore)) == "Restore");
    CHECK(std::string(lifecycle_stage_label(LifecycleStage::kDuplicate)) == "Duplicate");
}

// ============================================================================
// Coverage item
// ============================================================================

TEST_CASE("LifecycleCoverageItem — completeness", "[v23][p03][item]")
{
    LifecycleCoverageItem item;

    SECTION("Complete when all flags set")
    {
        item.is_authoritative = true;
        item.is_shell_owned = true;
        item.has_stable_id = true;
        CHECK(item.is_complete());
    }

    SECTION("Incomplete when missing authoritative")
    {
        item.is_shell_owned = true;
        item.has_stable_id = true;
        CHECK(!item.is_complete());
    }

    SECTION("Incomplete when missing shell ownership")
    {
        item.is_authoritative = true;
        item.has_stable_id = true;
        CHECK(!item.is_complete());
    }
}

// ============================================================================
// Registration and queries
// ============================================================================

TEST_CASE("LifecycleCompletionTracker — registration", "[v23][p03][tracker]")
{
    LifecycleCompletionTracker tracker;
    CHECK(tracker.item_count() == 0);

    LifecycleCoverageItem item;
    item.artifact_kind = ArtifactKind::kTextFile;
    item.stage = LifecycleStage::kCreation;
    tracker.add_item(std::move(item));

    CHECK(tracker.item_count() == 1);
}

TEST_CASE("LifecycleCompletionTracker — query by kind", "[v23][p03][query]")
{
    LifecycleCompletionTracker tracker;

    LifecycleCoverageItem text;
    text.artifact_kind = ArtifactKind::kTextFile;
    tracker.add_item(std::move(text));

    LifecycleCoverageItem workspace_item;
    workspace_item.artifact_kind = ArtifactKind::kTextFile;
    tracker.add_item(std::move(workspace_item));

    CHECK(tracker.items_by_kind(ArtifactKind::kTextFile).size() == 2);
}

TEST_CASE("LifecycleCompletionTracker — query by stage", "[v23][p03][query]")
{
    LifecycleCompletionTracker tracker;

    LifecycleCoverageItem create;
    create.stage = LifecycleStage::kCreation;
    tracker.add_item(std::move(create));

    LifecycleCoverageItem save;
    save.stage = LifecycleStage::kSave;
    tracker.add_item(std::move(save));

    CHECK(tracker.items_by_stage(LifecycleStage::kCreation).size() == 1);
    CHECK(tracker.items_by_stage(LifecycleStage::kSave).size() == 1);
}

TEST_CASE("LifecycleCompletionTracker — complete vs incomplete", "[v23][p03][query]")
{
    LifecycleCompletionTracker tracker;

    LifecycleCoverageItem complete;
    complete.is_authoritative = true;
    complete.is_shell_owned = true;
    complete.has_stable_id = true;
    tracker.add_item(std::move(complete));

    LifecycleCoverageItem incomplete;
    incomplete.is_authoritative = false;
    tracker.add_item(std::move(incomplete));

    CHECK(tracker.complete_items().size() == 1);
    CHECK(tracker.incomplete_items().size() == 1);
}

TEST_CASE("LifecycleCompletionTracker — gaps_for_kind", "[v23][p03][query]")
{
    LifecycleCompletionTracker tracker;

    LifecycleCoverageItem gap;
    gap.artifact_kind = ArtifactKind::kTextFile;
    gap.is_authoritative = false;
    tracker.add_item(std::move(gap));

    LifecycleCoverageItem ok;
    ok.artifact_kind = ArtifactKind::kTextFile;
    ok.is_authoritative = true;
    ok.is_shell_owned = true;
    ok.has_stable_id = true;
    tracker.add_item(std::move(ok));

    CHECK(tracker.gaps_for_kind(ArtifactKind::kTextFile).size() == 1);
}

// ============================================================================
// Gap report
// ============================================================================

TEST_CASE("LifecycleCompletionTracker — gap report", "[v23][p03][report]")
{
    LifecycleCompletionTracker tracker;

    LifecycleCoverageItem complete;
    complete.artifact_kind = ArtifactKind::kTextFile;
    complete.is_authoritative = true;
    complete.is_shell_owned = true;
    complete.has_stable_id = true;
    tracker.add_item(std::move(complete));

    LifecycleCoverageItem ws_gap;
    ws_gap.artifact_kind = ArtifactKind::kTextFile;
    tracker.add_item(std::move(ws_gap));

    LifecycleCoverageItem session_gap;
    session_gap.artifact_kind = ArtifactKind::kTextFile;
    tracker.add_item(std::move(session_gap));

    auto report = tracker.gap_report();
    CHECK(report.total_items == 3);
    CHECK(report.complete == 1);
    CHECK(report.incomplete == 2);
    CHECK(report.text_file_gaps == 2);
    CHECK(report.has_gaps());
    CHECK(report.coverage_pct() < 50.0);
}

TEST_CASE("LifecycleGapReport — empty is 100%", "[v23][p03][report]")
{
    LifecycleGapReport report;
    CHECK(report.coverage_pct() == 100.0);
    CHECK(!report.has_gaps());
}

// ============================================================================
// Clear and export
// ============================================================================

TEST_CASE("LifecycleCompletionTracker — clear", "[v23][p03][tracker]")
{
    LifecycleCompletionTracker tracker;
    LifecycleCoverageItem item;
    tracker.add_item(std::move(item));
    tracker.clear();
    CHECK(tracker.item_count() == 0);
}

TEST_CASE("LifecycleCompletionTracker — JSON export", "[v23][p03][export]")
{
    LifecycleCompletionTracker tracker;

    LifecycleCoverageItem item;
    item.artifact_kind = ArtifactKind::kTextFile;
    item.stage = LifecycleStage::kSave;
    item.is_authoritative = true;
    item.is_shell_owned = true;
    item.has_stable_id = true;
    tracker.add_item(std::move(item));

    auto json = tracker.export_json();
    CHECK(json.find("\"complete\": 1") != std::string::npos);
    CHECK(json.find("\"TextFile\"") != std::string::npos);
}

TEST_CASE("LifecycleCompletionTracker — Markdown export", "[v23][p03][export]")
{
    LifecycleCompletionTracker tracker;

    LifecycleCoverageItem gap;
    gap.artifact_kind = ArtifactKind::kTextFile;
    gap.stage = LifecycleStage::kRestore;
    tracker.add_item(std::move(gap));

    auto md = tracker.export_markdown();
    CHECK(md.find("V23 Lifecycle Completion Report") != std::string::npos);
    CHECK(md.find("Incomplete Items") != std::string::npos);
}

// ============================================================================
// Integration
// ============================================================================

TEST_CASE("Integration — full lifecycle tracker lifecycle", "[v23][p03][integration]")
{
    LifecycleCompletionTracker tracker;

    // Complete text file lifecycle
    for (auto stage : {LifecycleStage::kCreation, LifecycleStage::kOpen, LifecycleStage::kSave, LifecycleStage::kRestore})
    {
        LifecycleCoverageItem item;
        item.artifact_kind = ArtifactKind::kTextFile;
        item.stage = stage;
        item.is_authoritative = true;
        item.is_shell_owned = true;
        item.has_stable_id = true;
        tracker.add_item(std::move(item));
    }

    // Incomplete lifecycle item
    LifecycleCoverageItem ws_create;
    ws_create.artifact_kind = ArtifactKind::kTextFile;
    ws_create.stage = LifecycleStage::kCreation;
    ws_create.is_authoritative = true;
    ws_create.is_shell_owned = true;
    ws_create.has_stable_id = true;
    tracker.add_item(std::move(ws_create));

    LifecycleCoverageItem ws_save;
    ws_save.artifact_kind = ArtifactKind::kTextFile;
    ws_save.stage = LifecycleStage::kSave;
    // Missing shell ownership
    ws_save.is_authoritative = true;
    tracker.add_item(std::move(ws_save));

    CHECK(tracker.item_count() == 6);
    CHECK(tracker.complete_items().size() == 5);
    CHECK(tracker.incomplete_items().size() == 1);

    auto report = tracker.gap_report();
    CHECK(report.text_file_gaps == 1);
    CHECK(report.text_file_gaps >= 0);

    auto json = tracker.export_json();
    CHECK(!json.empty());
    auto md = tracker.export_markdown();
    CHECK(!md.empty());
}
