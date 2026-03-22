/// @file test_v24_p03_persistence.cpp
/// @brief V24 Phase 03 tests: Persistence, autosave, restore, recovery.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/RecoveryUXService.h"
#include "../../src/core/ArtifactRegistry.h"
#include "../../src/core/PersistenceOutcome.h"
#include "../../src/core/SessionRestoreService.h"
#include "../../src/core/ArtifactLifecycleValidator.h"
#include "../../src/core/Config.h"

using namespace markamp::core;

// P03-T01: Unified save semantics
TEST_CASE("P03-T01 save outcome tracks results", "[v24][p03]") {
    SaveOutcome outcome;
    outcome.artifact_id = {"art-1"};
    outcome.success = true;
    outcome.file_path = "/path/test.md";
    outcome.bytes_written = 1024;
    REQUIRE(outcome.ok());
}

TEST_CASE("P03-T01 persistence state transitions on save", "[v24][p03]") {
    using S = ArtifactLifecycleState;
    // Verify expected save-related transitions
    REQUIRE(ArtifactLifecycleValidator::is_valid_transition(S::kUnsaved, S::kSaving));
    REQUIRE(ArtifactLifecycleValidator::is_valid_transition(S::kSaving, S::kSaved));
    REQUIRE(ArtifactLifecycleValidator::is_valid_transition(S::kDirty, S::kSaving));
    REQUIRE(ArtifactLifecycleValidator::is_valid_transition(S::kSaving, S::kSaveError));
    REQUIRE(ArtifactLifecycleValidator::is_valid_transition(S::kSaveError, S::kSaving));
}

// P03-T02: Per-type autosave
TEST_CASE("P03-T02 autosave policy defaults", "[v24][p03]") {
    AutosavePolicy policy;
    REQUIRE(policy.is_enabled());
    REQUIRE(policy.interval_seconds == 30);
    REQUIRE(policy.save_on_focus_loss);
}

// P03-T03: Recovery UX service
TEST_CASE("P03-T03 recovery service accepts and discards", "[v24][p03]") {
    RecoveryUXService svc;
    RecoverableArtifact r1;
    r1.artifact_id = {"a1"};
    r1.display_name = "Doc1";
    RecoverableArtifact r2;
    r2.artifact_id = {"a2"};
    r2.display_name = "Doc2";
    svc.add_recoverable(r1);
    svc.add_recoverable(r2);

    REQUIRE(svc.total_count() == 2);
    REQUIRE(svc.pending_count() == 2);

    REQUIRE(svc.accept({"a1"}));
    REQUIRE(svc.discard({"a2"}));
    REQUIRE(svc.accepted_count() == 1);
    REQUIRE(svc.discarded_count() == 1);
    REQUIRE(svc.pending_count() == 0);
}

TEST_CASE("P03-T03 recovery accept all", "[v24][p03]") {
    RecoveryUXService svc;
    RecoverableArtifact r1;
    r1.artifact_id = {"a1"}; r1.display_name = "D1";
    RecoverableArtifact r2;
    r2.artifact_id = {"a2"}; r2.display_name = "D2";
    RecoverableArtifact r3;
    r3.artifact_id = {"a3"}; r3.display_name = "D3";
    svc.add_recoverable(r1);
    svc.add_recoverable(r2);
    svc.add_recoverable(r3);

    svc.accept_all();
    REQUIRE(svc.accepted_count() == 3);

    auto res = svc.result();
    REQUIRE(res.action == RecoveryAction::kAcceptAll);
    REQUIRE(res.any_accepted());
}

TEST_CASE("P03-T03 recovery discard all", "[v24][p03]") {
    RecoveryUXService svc;
    RecoverableArtifact r;
    r.artifact_id = {"a1"}; r.display_name = "D1";
    svc.add_recoverable(r);
    svc.discard_all();

    auto res = svc.result();
    REQUIRE(res.action == RecoveryAction::kDiscardAll);
    REQUIRE_FALSE(res.any_accepted());
}

// P03-T04: Normalize recent items
TEST_CASE("P03-T04 session restore captures state", "[v24][p03]") {
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    SessionRestoreService svc(bus, registry, config);

    // Capture empty session
    auto session = svc.capture_session();
    REQUIRE(session.empty());

    // Register an artifact and capture again
    ArtifactRecord rec;
    rec.kind = ArtifactKind::kTextFile;
    rec.display_name = "file.md";
    rec.source = "test";
    registry.register_artifact(rec);
    session = svc.capture_session();
    REQUIRE(session.size() == 1);
}

// P03-T05: Persistence regression tests
TEST_CASE("P03-T05 save outcome failure tracking", "[v24][p03]") {
    SaveOutcome outcome;
    outcome.artifact_id = {"art-2"};
    outcome.success = false;
    outcome.error_message = "Permission denied";
    outcome.error_code = "PERMISSION_DENIED";
    REQUIRE_FALSE(outcome.ok());
    REQUIRE_FALSE(outcome.is_retriable());
}

TEST_CASE("P03-T05 recovery clear", "[v24][p03]") {
    RecoveryUXService svc;
    RecoverableArtifact r;
    r.artifact_id = {"a1"}; r.display_name = "D1";
    svc.add_recoverable(r);
    svc.clear();
    REQUIRE(svc.total_count() == 0);
}

TEST_CASE("P03-T05 selective recovery result", "[v24][p03]") {
    RecoveryUXService svc;
    RecoverableArtifact r1;
    r1.artifact_id = {"a1"}; r1.display_name = "D1";
    RecoverableArtifact r2;
    r2.artifact_id = {"a2"}; r2.display_name = "D2";
    svc.add_recoverable(r1);
    svc.add_recoverable(r2);
    svc.accept({"a1"});
    auto res = svc.result();
    REQUIRE(res.action == RecoveryAction::kSelective);
    REQUIRE(res.was_acted_upon());
}
