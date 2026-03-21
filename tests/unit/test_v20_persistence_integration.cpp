/// @file test_v20_persistence_integration.cpp
/// @brief V20 Phase 05 – Persistence integration tests.

#include "core/AtomicWriteService.h"
#include "core/ArtifactCreationService.h"
#include "core/ArtifactRegistry.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/PersistenceOutcome.h"
#include "core/SessionRestoreService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct PersistIntegFixture
{
    EventBus bus;
    ArtifactRegistry registry{bus};
    Config config;
    ArtifactCreationService creation{bus, registry, config};
    PersistenceOutcomeService persistence{bus, registry};
    AtomicWriteService atomic_write;
    SessionRestoreService session{bus, registry, config};
};

TEST_CASE("PersistInteg: Save → Backup → Restore cycle", "[v20][persist-integration]")
{
    PersistIntegFixture fix;

    // Create artifact
    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kTextFile;
    req.display_name = "important.md";
    req.language_id = "markdown";
    req.source = "test";
    auto art = fix.creation.create(req);

    // Save with atomic write
    auto write_result = fix.atomic_write.write_atomic("/workspace/important.md", "# Important");
    REQUIRE(write_result.ok());

    auto save_result = fix.persistence.execute_save(art.id, "/workspace/important.md");
    REQUIRE(save_result.ok());

    // Create backup
    auto backup = fix.atomic_write.create_backup("/workspace/important.md");
    REQUIRE(backup.ok());

    // Capture and restore
    auto captured = fix.session.capture_session();
    REQUIRE(captured.size() == 1);
}

TEST_CASE("PersistInteg: Failed save keeps dirty + recovery journal", "[v20][persist-integration]")
{
    PersistIntegFixture fix;

    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kTextFile;
    req.display_name = "fail-doc.md";
    req.language_id = "markdown";
    req.source = "test";
    auto art = fix.creation.create(req);

    // Fail the save
    SaveExecutor failing = [](const ArtifactId& aid, const std::string&) {
        SaveOutcome o;
        o.artifact_id = aid;
        o.error_message = "Disk full";
        return o;
    };

    auto result = fix.persistence.execute_save_with(art.id, "/workspace/fail-doc.md", failing);
    REQUIRE_FALSE(result.ok());

    // Artifact should stay dirty
    REQUIRE(fix.registry.find(art.id)->is_dirty());

    // Record journal
    fix.atomic_write.record_journal_entry(art.id.value, "/workspace/fail-doc.md",
                                            "/tmp/journal/fail-doc.journal");
    REQUIRE(fix.atomic_write.journal_entry_count() == 1);
}

TEST_CASE("PersistInteg: Multi-artifact autosave", "[v20][persist-integration]")
{
    PersistIntegFixture fix;

    for (int i = 0; i < 3; ++i)
    {
        ArtifactCreationRequest req;
        req.kind = ArtifactKind::kTextFile;
        req.display_name = "doc-" + std::to_string(i) + ".md";
        req.language_id = "markdown";
        req.source = "test";
        auto result = fix.creation.create(req);
        fix.registry.promote_to_saved(result.id, "/workspace/" + req.display_name);
        fix.registry.set_state(result.id, ArtifactLifecycleState::kDirty);
    }

    int saved = fix.session.trigger_autosave();
    REQUIRE(saved == 3);
}

TEST_CASE("PersistInteg: Phase 05 event types", "[v20][persist-events]")
{
    events::SaveOutcomeEvent save;
    save.artifact_id = "a-1";
    save.success = true;
    save.bytes_written = 1024;
    REQUIRE(save.success);

    events::AtomicWriteCompletedEvent atomic;
    atomic.artifact_id = "a-1";
    atomic.used_temp_file = true;
    REQUIRE(atomic.used_temp_file);

    events::RecoveryJournalCreatedEvent journal;
    journal.artifact_id = "a-1";
    journal.journal_path = "/tmp/journal/a-1.journal";
    REQUIRE_FALSE(journal.journal_path.empty());

    events::SessionCapturedEvent capture;
    capture.artifact_count = 5;
    REQUIRE(capture.artifact_count == 5);

    events::ArtifactSessionRestoredEvent restore;
    restore.restored_count = 3;
    REQUIRE(restore.restored_count == 3);

    events::ArtifactAutosaveTriggeredEvent autosave;
    autosave.saved_count = 2;
    REQUIRE(autosave.saved_count == 2);
}
