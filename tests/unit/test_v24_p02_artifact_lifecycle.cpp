/// @file test_v24_p02_artifact_lifecycle.cpp
/// @brief V24 Phase 02 tests: Artifact lifecycle validation, creation routing,
///        metadata standardization, and state transition correctness.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/ArtifactRegistry.h"
#include "../../src/core/ArtifactCreationService.h"
#include "../../src/core/ArtifactLifecycleValidator.h"
#include "../../src/core/Config.h"

using namespace markamp::core;

// ════════════════════════════════════════════════════════════════
// P02-T01: Replace pseudo-untitled paths
// ════════════════════════════════════════════════════════════════

TEST_CASE("P02-T01 unsaved artifacts have no file path", "[v24][p02]") {
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord r;
    r.kind = ArtifactKind::kTextFile;
    r.display_name = "Untitled-1";
    r.language_id = "markdown";
    r.source = "command";
    auto id = registry.register_artifact(r);

    auto* found = registry.find(id);
    REQUIRE(found != nullptr);
    REQUIRE(found->is_unsaved());
    REQUIRE_FALSE(found->has_path());
}

TEST_CASE("P02-T01 promote unsaved to saved with path", "[v24][p02]") {
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord r;
    r.kind = ArtifactKind::kTextFile;
    r.display_name = "Test";
    r.source = "test";
    auto id = registry.register_artifact(r);

    REQUIRE(registry.promote_to_saved(id, "/path/to/test.md"));
    auto* found = registry.find(id);
    REQUIRE(found->is_saved());
    REQUIRE(found->has_path());
    REQUIRE(*found->file_path == "/path/to/test.md");
}

// ════════════════════════════════════════════════════════════════
// P02-T02: Route creation through single service
// ════════════════════════════════════════════════════════════════

TEST_CASE("P02-T02 creation service routes all types", "[v24][p02]") {
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;

    ArtifactCreationService service(bus, registry, config);
    auto text = service.create_text_file("doc.md", "markdown", "command");
    REQUIRE(text.ok());
    auto nb = service.create_text_file("notebook.ipynb", "palette");
    REQUIRE(nb.ok());
    auto cv = service.create_text_file("board.canvas", "context-menu");
    REQUIRE(cv.ok());

    REQUIRE(service.creation_count() == 3);
    REQUIRE(registry.count() == 3);
}

// ════════════════════════════════════════════════════════════════
// P02-T03: Standardize initial metadata
// ════════════════════════════════════════════════════════════════

TEST_CASE("P02-T03 set_initial_metadata fills missing fields", "[v24][p02]") {
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord r;
    r.kind = ArtifactKind::kTextFile;
    auto id = registry.register_artifact(r);

    REQUIRE(registry.set_initial_metadata(id, "markdown", "command", "Untitled-1"));
    auto* found = registry.find(id);
    REQUIRE(found->language_id == "markdown");
    REQUIRE(found->source == "command");
    REQUIRE(found->display_name == "Untitled-1");
}

TEST_CASE("P02-T03 set_initial_metadata does not overwrite existing", "[v24][p02]") {
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord r;
    r.kind = ArtifactKind::kTextFile;
    r.language_id = "python";
    r.source = "palette";
    r.display_name = "Script";
    auto id = registry.register_artifact(r);

    REQUIRE(registry.set_initial_metadata(id, "cpp", "menu", "Override"));
    auto* found = registry.find(id);
    REQUIRE(found->language_id == "python");  // not overwritten
    REQUIRE(found->source == "palette");       // not overwritten
    REQUIRE(found->display_name == "Script");  // not overwritten
}

// ════════════════════════════════════════════════════════════════
// P02-T04: Artifact lifecycle validator
// ════════════════════════════════════════════════════════════════

TEST_CASE("P02-T04 validator detects missing metadata", "[v24][p02]") {
    EventBus bus;
    ArtifactRegistry registry(bus);

    // Register an artifact with missing required fields
    ArtifactRecord r;
    r.kind = ArtifactKind::kTextFile;
    // Intentionally empty: display_name, language_id, source
    registry.register_artifact(r);

    ArtifactLifecycleValidator validator;
    auto result = validator.validate(registry);
    REQUIRE(result.total_artifacts == 1);
    REQUIRE_FALSE(result.is_clean());
    REQUIRE(result.violation_count() >= 3); // missing name, lang, source
}

TEST_CASE("P02-T04 validator passes clean artifacts", "[v24][p02]") {
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord r;
    r.kind = ArtifactKind::kTextFile;
    r.display_name = "Test.md";
    r.language_id = "markdown";
    r.source = "command";
    registry.register_artifact(r);

    ArtifactLifecycleValidator validator;
    auto result = validator.validate(registry);
    REQUIRE(result.is_clean());
    REQUIRE(result.valid_artifacts == 1);
}

// ════════════════════════════════════════════════════════════════
// P02-T05: State transitions and lifecycle smoke
// ════════════════════════════════════════════════════════════════

TEST_CASE("P02-T05 valid state transitions", "[v24][p02]") {
    using S = ArtifactLifecycleState;
    REQUIRE(ArtifactLifecycleValidator::is_valid_transition(S::kUnsaved, S::kSaving));
    REQUIRE(ArtifactLifecycleValidator::is_valid_transition(S::kSaving, S::kSaved));
    REQUIRE(ArtifactLifecycleValidator::is_valid_transition(S::kSaved, S::kDirty));
    REQUIRE(ArtifactLifecycleValidator::is_valid_transition(S::kDirty, S::kSaving));
    REQUIRE(ArtifactLifecycleValidator::is_valid_transition(S::kSaveError, S::kSaving));
    // Invalid transitions
    REQUIRE_FALSE(ArtifactLifecycleValidator::is_valid_transition(S::kUnsaved, S::kSaved));
    REQUIRE_FALSE(ArtifactLifecycleValidator::is_valid_transition(S::kDeleted, S::kSaved));
    REQUIRE_FALSE(ArtifactLifecycleValidator::is_valid_transition(S::kSaved, S::kUnsaved));
}

TEST_CASE("P02-T05 lifecycle validator export", "[v24][p02]") {
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord r;
    r.kind = ArtifactKind::kTextFile;
    r.display_name = "Test.nb";
    r.source = "command";
    registry.register_artifact(r);

    ArtifactLifecycleValidator validator;
    auto result = validator.validate(registry);

    auto json = ArtifactLifecycleValidator::export_json(result);
    REQUIRE(json.find("\"total\": 1") != std::string::npos);

    auto md = ArtifactLifecycleValidator::export_markdown(result);
    REQUIRE(md.find("Artifact Lifecycle Validation") != std::string::npos);
}
