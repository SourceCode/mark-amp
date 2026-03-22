/// @file test_v23_persistence_completion.cpp
/// @brief V23 Phase 04 — Tests for PersistenceCompletionAuditor.

#include "core/PersistenceCompletionAuditor.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Label tests
// ============================================================================

TEST_CASE("PersistenceCapability — labels", "[v23][p04][label]")
{
    CHECK(std::string(persistence_capability_label(PersistenceCapability::kSave)) == "Save");
    CHECK(std::string(persistence_capability_label(PersistenceCapability::kAutosave)) == "Autosave");
    CHECK(std::string(persistence_capability_label(PersistenceCapability::kRecoveryJournal)) == "RecoveryJournal");
    CHECK(std::string(persistence_capability_label(PersistenceCapability::kRollback)) == "Rollback");
    CHECK(std::string(persistence_capability_label(PersistenceCapability::kImportExport)) == "ImportExport");
}

TEST_CASE("PersistenceArtifactKind — labels", "[v23][p04][label]")
{
    CHECK(std::string(persistence_artifact_label(PersistenceArtifactKind::kTextFile)) == "TextFile");
    CHECK(std::string(persistence_artifact_label(PersistenceArtifactKind::kNotebook)) == "Notebook");
    CHECK(std::string(persistence_artifact_label(PersistenceArtifactKind::kAll)) == "All");
}

// ============================================================================
// Coverage item
// ============================================================================

TEST_CASE("PersistenceCoverageItem — completeness", "[v23][p04][item]")
{
    PersistenceCoverageItem item;

    SECTION("Complete when authoritative + failure handling")
    {
        item.is_authoritative = true;
        item.has_failure_handling = true;
        CHECK(item.is_complete());
    }

    SECTION("Incomplete without authoritative")
    {
        item.has_failure_handling = true;
        CHECK(!item.is_complete());
    }

    SECTION("Incomplete without failure handling")
    {
        item.is_authoritative = true;
        CHECK(!item.is_complete());
    }
}

// ============================================================================
// Registration and queries
// ============================================================================

TEST_CASE("PersistenceCompletionAuditor — registration", "[v23][p04][auditor]")
{
    PersistenceCompletionAuditor auditor;
    CHECK(auditor.item_count() == 0);

    PersistenceCoverageItem item;
    item.capability = PersistenceCapability::kSave;
    auditor.add_item(std::move(item));

    CHECK(auditor.item_count() == 1);
}

TEST_CASE("PersistenceCompletionAuditor — query by capability", "[v23][p04][query]")
{
    PersistenceCompletionAuditor auditor;

    PersistenceCoverageItem save;
    save.capability = PersistenceCapability::kSave;
    auditor.add_item(std::move(save));

    PersistenceCoverageItem autosave;
    autosave.capability = PersistenceCapability::kAutosave;
    auditor.add_item(std::move(autosave));

    CHECK(auditor.items_by_capability(PersistenceCapability::kSave).size() == 1);
    CHECK(auditor.items_by_capability(PersistenceCapability::kAutosave).size() == 1);
    CHECK(auditor.items_by_capability(PersistenceCapability::kRollback).empty());
}

TEST_CASE("PersistenceCompletionAuditor — query by artifact", "[v23][p04][query]")
{
    PersistenceCompletionAuditor auditor;

    PersistenceCoverageItem text;
    text.artifact_kind = PersistenceArtifactKind::kTextFile;
    auditor.add_item(std::move(text));

    PersistenceCoverageItem notebook;
    notebook.artifact_kind = PersistenceArtifactKind::kNotebook;
    auditor.add_item(std::move(notebook));

    CHECK(auditor.items_by_artifact(PersistenceArtifactKind::kTextFile).size() == 1);
    CHECK(auditor.items_by_artifact(PersistenceArtifactKind::kNotebook).size() == 1);
}

TEST_CASE("PersistenceCompletionAuditor — complete vs incomplete", "[v23][p04][query]")
{
    PersistenceCompletionAuditor auditor;

    PersistenceCoverageItem complete;
    complete.is_authoritative = true;
    complete.has_failure_handling = true;
    auditor.add_item(std::move(complete));

    PersistenceCoverageItem incomplete;
    incomplete.is_authoritative = false;
    auditor.add_item(std::move(incomplete));

    CHECK(auditor.complete_items().size() == 1);
    CHECK(auditor.incomplete_items().size() == 1);
}

// ============================================================================
// Gap report
// ============================================================================

TEST_CASE("PersistenceCompletionAuditor — gap report", "[v23][p04][report]")
{
    PersistenceCompletionAuditor auditor;

    PersistenceCoverageItem complete;
    complete.is_authoritative = true;
    complete.is_transactional = true;
    complete.has_failure_handling = true;
    complete.has_journal_support = true;
    auditor.add_item(std::move(complete));

    PersistenceCoverageItem incomplete;
    incomplete.is_authoritative = true;
    auditor.add_item(std::move(incomplete));

    auto report = auditor.gap_report();
    CHECK(report.total_items == 2);
    CHECK(report.complete == 1);
    CHECK(report.incomplete == 1);
    CHECK(report.authoritative == 2);
    CHECK(report.transactional == 1);
    CHECK(report.with_journal == 1);
    CHECK(report.has_gaps());
}

TEST_CASE("PersistenceGapReport — empty is 100%", "[v23][p04][report]")
{
    PersistenceGapReport report;
    CHECK(report.coverage_pct() == 100.0);
    CHECK(!report.has_gaps());
}

// ============================================================================
// Clear and export
// ============================================================================

TEST_CASE("PersistenceCompletionAuditor — clear", "[v23][p04][auditor]")
{
    PersistenceCompletionAuditor auditor;
    PersistenceCoverageItem item;
    auditor.add_item(std::move(item));
    auditor.clear();
    CHECK(auditor.item_count() == 0);
}

TEST_CASE("PersistenceCompletionAuditor — JSON export", "[v23][p04][export]")
{
    PersistenceCompletionAuditor auditor;

    PersistenceCoverageItem item;
    item.capability = PersistenceCapability::kSave;
    item.is_authoritative = true;
    item.has_failure_handling = true;
    auditor.add_item(std::move(item));

    auto json = auditor.export_json();
    CHECK(json.find("\"complete\": 1") != std::string::npos);
    CHECK(json.find("\"Save\"") != std::string::npos);
}

TEST_CASE("PersistenceCompletionAuditor — Markdown export", "[v23][p04][export]")
{
    PersistenceCompletionAuditor auditor;

    PersistenceCoverageItem gap;
    gap.capability = PersistenceCapability::kRecoveryJournal;
    auditor.add_item(std::move(gap));

    auto md = auditor.export_markdown();
    CHECK(md.find("V23 Persistence Completion Report") != std::string::npos);
    CHECK(md.find("Incomplete Capabilities") != std::string::npos);
}

// ============================================================================
// Integration
// ============================================================================

TEST_CASE("Integration — full persistence auditor lifecycle", "[v23][p04][integration]")
{
    PersistenceCompletionAuditor auditor;

    // Complete save capabilities
    for (auto cap : {PersistenceCapability::kSave, PersistenceCapability::kSaveAll, PersistenceCapability::kAutosave})
    {
        PersistenceCoverageItem item;
        item.capability = cap;
        item.artifact_kind = PersistenceArtifactKind::kAll;
        item.is_authoritative = true;
        item.is_transactional = true;
        item.has_failure_handling = true;
        item.has_journal_support = true;
        auditor.add_item(std::move(item));
    }

    // Incomplete recovery
    PersistenceCoverageItem recovery;
    recovery.capability = PersistenceCapability::kRecoveryJournal;
    recovery.is_authoritative = true;
    // Missing failure handling
    auditor.add_item(std::move(recovery));

    // Incomplete rollback
    PersistenceCoverageItem rollback;
    rollback.capability = PersistenceCapability::kRollback;
    // Not authoritative
    auditor.add_item(std::move(rollback));

    CHECK(auditor.item_count() == 5);
    CHECK(auditor.complete_items().size() == 3);
    CHECK(auditor.incomplete_items().size() == 2);

    auto report = auditor.gap_report();
    CHECK(report.complete == 3);
    CHECK(report.incomplete == 2);
    CHECK(report.coverage_pct() == 60.0);
}
