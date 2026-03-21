/// @file test_v20_artifact_registry.cpp
/// @brief V20 Phase 01 – ArtifactRegistry unit tests.

#include "core/ArtifactRegistry.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("ArtifactRegistry: construction", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    REQUIRE(registry.count() == 0);
    REQUIRE(registry.active_artifact().empty());
    REQUIRE(registry.all_artifacts().empty());
}

// ============================================================================
// Registration
// ============================================================================

TEST_CASE("ArtifactRegistry: register text file", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.kind = ArtifactKind::kTextFile;
    record.display_name = "test.md";
    record.language_id = "markdown";

    auto artifact_id = registry.register_artifact(record);
    REQUIRE_FALSE(artifact_id.empty());
    REQUIRE(registry.count() == 1);
}

TEST_CASE("ArtifactRegistry: register notebook", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.kind = ArtifactKind::kNotebook;
    record.display_name = "analysis.markamp-nb";

    auto artifact_id = registry.register_artifact(record);
    REQUIRE_FALSE(artifact_id.empty());
    REQUIRE(registry.count_by_kind(ArtifactKind::kNotebook) == 1);
}

TEST_CASE("ArtifactRegistry: register canvas", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.kind = ArtifactKind::kCanvas;
    record.display_name = "whiteboard.markamp-canvas";

    auto artifact_id = registry.register_artifact(record);
    REQUIRE_FALSE(artifact_id.empty());
    REQUIRE(registry.count_by_kind(ArtifactKind::kCanvas) == 1);
}

TEST_CASE("ArtifactRegistry: register multiple artifacts", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord r1;
    r1.kind = ArtifactKind::kTextFile;
    r1.display_name = "file1.md";

    ArtifactRecord r2;
    r2.kind = ArtifactKind::kNotebook;
    r2.display_name = "notebook1.markamp-nb";

    ArtifactRecord r3;
    r3.kind = ArtifactKind::kCanvas;
    r3.display_name = "board1.markamp-canvas";

    registry.register_artifact(r1);
    registry.register_artifact(r2);
    registry.register_artifact(r3);

    REQUIRE(registry.count() == 3);
    REQUIRE(registry.count_by_kind(ArtifactKind::kTextFile) == 1);
    REQUIRE(registry.count_by_kind(ArtifactKind::kNotebook) == 1);
    REQUIRE(registry.count_by_kind(ArtifactKind::kCanvas) == 1);
}

// ============================================================================
// Unregistration
// ============================================================================

TEST_CASE("ArtifactRegistry: unregister artifact", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.display_name = "temp.md";
    auto artifact_id = registry.register_artifact(record);

    REQUIRE(registry.unregister(artifact_id));
    REQUIRE(registry.count() == 0);
    REQUIRE(registry.find(artifact_id) == nullptr);
}

TEST_CASE("ArtifactRegistry: unregister nonexistent", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactId fake_id{"nonexistent-id"};
    REQUIRE_FALSE(registry.unregister(fake_id));
}

TEST_CASE("ArtifactRegistry: unregister active artifact clears active", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.display_name = "active.md";
    auto artifact_id = registry.register_artifact(record);

    registry.set_active_artifact(artifact_id);
    REQUIRE(registry.active_artifact() == artifact_id);

    registry.unregister(artifact_id);
    REQUIRE(registry.active_artifact().empty());
}

// ============================================================================
// Find
// ============================================================================

TEST_CASE("ArtifactRegistry: find by ID", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.display_name = "findme.md";
    record.language_id = "markdown";
    auto artifact_id = registry.register_artifact(record);

    const auto* found = registry.find(artifact_id);
    REQUIRE(found != nullptr);
    REQUIRE(found->display_name == "findme.md");
    REQUIRE(found->language_id == "markdown");
}

TEST_CASE("ArtifactRegistry: find by ID returns nullptr for missing", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactId missing{"does-not-exist"};
    REQUIRE(registry.find(missing) == nullptr);
}

TEST_CASE("ArtifactRegistry: find by path", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.display_name = "doc.md";
    record.file_path = "/workspace/doc.md";
    registry.register_artifact(record);

    const auto* found = registry.find_by_path("/workspace/doc.md");
    REQUIRE(found != nullptr);
    REQUIRE(found->display_name == "doc.md");
}

TEST_CASE("ArtifactRegistry: find by empty path returns nullptr", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    REQUIRE(registry.find_by_path("") == nullptr);
}

TEST_CASE("ArtifactRegistry: find by nonexistent path", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    REQUIRE(registry.find_by_path("/nonexistent/path.md") == nullptr);
}

// ============================================================================
// Update
// ============================================================================

TEST_CASE("ArtifactRegistry: update record", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.display_name = "original.md";
    auto artifact_id = registry.register_artifact(record);

    ArtifactRecord updated;
    updated.display_name = "updated.md";
    updated.language_id = "cpp";

    REQUIRE(registry.update(artifact_id, updated));

    const auto* found = registry.find(artifact_id);
    REQUIRE(found != nullptr);
    REQUIRE(found->display_name == "updated.md");
    REQUIRE(found->language_id == "cpp");
}

TEST_CASE("ArtifactRegistry: update nonexistent returns false", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactId fake{"fake-id"};
    ArtifactRecord updated;
    REQUIRE_FALSE(registry.update(fake, updated));
}

// ============================================================================
// State transitions
// ============================================================================

TEST_CASE("ArtifactRegistry: set state", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.display_name = "state-test.md";
    auto artifact_id = registry.register_artifact(record);

    REQUIRE(registry.set_state(artifact_id, ArtifactLifecycleState::kDirty));

    const auto* found = registry.find(artifact_id);
    REQUIRE(found != nullptr);
    REQUIRE(found->state == ArtifactLifecycleState::kDirty);
}

TEST_CASE("ArtifactRegistry: set state no-op for same state", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.state = ArtifactLifecycleState::kUnsaved;
    auto artifact_id = registry.register_artifact(record);

    // Setting same state should succeed without publishing
    REQUIRE(registry.set_state(artifact_id, ArtifactLifecycleState::kUnsaved));
}

TEST_CASE("ArtifactRegistry: set state on nonexistent", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactId fake{"fake"};
    REQUIRE_FALSE(registry.set_state(fake, ArtifactLifecycleState::kSaved));
}

// ============================================================================
// Promote to saved
// ============================================================================

TEST_CASE("ArtifactRegistry: promote to saved", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.display_name = "unsaved.md";
    record.state = ArtifactLifecycleState::kUnsaved;
    auto artifact_id = registry.register_artifact(record);

    REQUIRE(registry.promote_to_saved(artifact_id, "/workspace/saved.md"));

    const auto* found = registry.find(artifact_id);
    REQUIRE(found != nullptr);
    REQUIRE(found->state == ArtifactLifecycleState::kSaved);
    REQUIRE(found->file_path.has_value());
    REQUIRE(found->file_path.value() == "/workspace/saved.md");
}

TEST_CASE("ArtifactRegistry: promote nonexistent", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactId fake{"fake"};
    REQUIRE_FALSE(registry.promote_to_saved(fake, "/path/to/file.md"));
}

// ============================================================================
// Rename
// ============================================================================

TEST_CASE("ArtifactRegistry: rename", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.display_name = "old-name.md";
    auto artifact_id = registry.register_artifact(record);

    REQUIRE(registry.rename(artifact_id, "new-name.md"));

    const auto* found = registry.find(artifact_id);
    REQUIRE(found != nullptr);
    REQUIRE(found->display_name == "new-name.md");
}

TEST_CASE("ArtifactRegistry: rename nonexistent", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactId fake{"fake"};
    REQUIRE_FALSE(registry.rename(fake, "new-name"));
}

// ============================================================================
// Enumeration
// ============================================================================

TEST_CASE("ArtifactRegistry: all_artifacts", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord r1;
    r1.display_name = "a.md";
    ArtifactRecord r2;
    r2.display_name = "b.md";

    registry.register_artifact(r1);
    registry.register_artifact(r2);

    auto all = registry.all_artifacts();
    REQUIRE(all.size() == 2);
}

TEST_CASE("ArtifactRegistry: artifacts_by_kind", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord r1;
    r1.kind = ArtifactKind::kTextFile;
    ArtifactRecord r2;
    r2.kind = ArtifactKind::kNotebook;
    ArtifactRecord r3;
    r3.kind = ArtifactKind::kTextFile;

    registry.register_artifact(r1);
    registry.register_artifact(r2);
    registry.register_artifact(r3);

    REQUIRE(registry.artifacts_by_kind(ArtifactKind::kTextFile).size() == 2);
    REQUIRE(registry.artifacts_by_kind(ArtifactKind::kNotebook).size() == 1);
    REQUIRE(registry.artifacts_by_kind(ArtifactKind::kCanvas).empty());
}

TEST_CASE("ArtifactRegistry: artifacts_by_state", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord r1;
    r1.state = ArtifactLifecycleState::kUnsaved;
    ArtifactRecord r2;
    r2.state = ArtifactLifecycleState::kSaved;

    registry.register_artifact(r1);
    auto id2 = registry.register_artifact(r2);

    // Note: register_artifact doesn't enforce state, so r2 keeps kSaved
    REQUIRE(registry.artifacts_by_state(ArtifactLifecycleState::kUnsaved).size() == 1);
    REQUIRE(registry.artifacts_by_state(ArtifactLifecycleState::kSaved).size() == 1);
}

// ============================================================================
// Active artifact
// ============================================================================

TEST_CASE("ArtifactRegistry: set active artifact", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.display_name = "active.md";
    auto artifact_id = registry.register_artifact(record);

    registry.set_active_artifact(artifact_id);
    REQUIRE(registry.active_artifact() == artifact_id);
}

// ============================================================================
// Counting
// ============================================================================

TEST_CASE("ArtifactRegistry: count_by_kind", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord r1;
    r1.kind = ArtifactKind::kCanvas;
    ArtifactRecord r2;
    r2.kind = ArtifactKind::kCanvas;

    registry.register_artifact(r1);
    registry.register_artifact(r2);

    REQUIRE(registry.count_by_kind(ArtifactKind::kCanvas) == 2);
    REQUIRE(registry.count_by_kind(ArtifactKind::kTextFile) == 0);
}

TEST_CASE("ArtifactRegistry: count_by_state", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    auto artifact_id = registry.register_artifact(record);

    REQUIRE(registry.count_by_state(ArtifactLifecycleState::kUnsaved) == 1);

    registry.set_state(artifact_id, ArtifactLifecycleState::kDirty);
    REQUIRE(registry.count_by_state(ArtifactLifecycleState::kDirty) == 1);
    REQUIRE(registry.count_by_state(ArtifactLifecycleState::kUnsaved) == 0);
}

// ============================================================================
// ID generation
// ============================================================================

TEST_CASE("ArtifactRegistry: generate_id produces unique IDs", "[v20][artifact-registry]")
{
    auto id1 = ArtifactRegistry::generate_id();
    auto id2 = ArtifactRegistry::generate_id();

    REQUIRE_FALSE(id1.empty());
    REQUIRE_FALSE(id2.empty());
    REQUIRE(id1 != id2);
}

// ============================================================================
// Clear
// ============================================================================

TEST_CASE("ArtifactRegistry: clear", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord r1;
    r1.display_name = "a.md";
    auto artifact_id = registry.register_artifact(r1);
    registry.set_active_artifact(artifact_id);

    registry.clear();
    REQUIRE(registry.count() == 0);
    REQUIRE(registry.active_artifact().empty());
}

// ============================================================================
// ArtifactRecord convenience queries
// ============================================================================

TEST_CASE("ArtifactRecord: convenience queries", "[v20][artifact-registry]")
{
    ArtifactRecord record;
    record.kind = ArtifactKind::kTextFile;
    record.state = ArtifactLifecycleState::kUnsaved;

    REQUIRE(record.is_unsaved());
    REQUIRE_FALSE(record.is_dirty());
    REQUIRE_FALSE(record.is_saved());
    REQUIRE(record.is_text_file());
    REQUIRE_FALSE(record.is_notebook());
    REQUIRE_FALSE(record.is_canvas());
    REQUIRE_FALSE(record.has_path());

    record.file_path = "/some/path.md";
    REQUIRE(record.has_path());
}

// ============================================================================
// Event publishing
// ============================================================================

TEST_CASE("ArtifactRegistry: publishes ArtifactRegisteredEvent", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    bool received = false;
    auto sub = bus.subscribe<events::ArtifactRegisteredEvent>(
        [&received](const events::ArtifactRegisteredEvent& /*evt*/) { received = true; });

    ArtifactRecord record;
    registry.register_artifact(record);

    REQUIRE(received);
}

TEST_CASE("ArtifactRegistry: publishes ArtifactUnregisteredEvent", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    auto artifact_id = registry.register_artifact(record);

    bool received = false;
    auto sub = bus.subscribe<events::ArtifactUnregisteredEvent>(
        [&received](const events::ArtifactUnregisteredEvent& /*evt*/) { received = true; });

    registry.unregister(artifact_id);
    REQUIRE(received);
}

TEST_CASE("ArtifactRegistry: publishes ArtifactStateChangedEvent", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    auto artifact_id = registry.register_artifact(record);

    int old_state = -1;
    int new_state = -1;
    auto sub = bus.subscribe<events::ArtifactStateChangedEvent>(
        [&](const events::ArtifactStateChangedEvent& evt)
        {
            old_state = evt.old_state;
            new_state = evt.new_state;
        });

    registry.set_state(artifact_id, ArtifactLifecycleState::kDirty);
    REQUIRE(old_state == static_cast<int>(ArtifactLifecycleState::kUnsaved));
    REQUIRE(new_state == static_cast<int>(ArtifactLifecycleState::kDirty));
}

TEST_CASE("ArtifactRegistry: publishes ArtifactActivatedEvent", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    auto artifact_id = registry.register_artifact(record);

    std::string activated_id;
    auto sub = bus.subscribe<events::ArtifactActivatedEvent>(
        [&activated_id](const events::ArtifactActivatedEvent& evt)
        { activated_id = evt.artifact_id; });

    registry.set_active_artifact(artifact_id);
    REQUIRE(activated_id == artifact_id.value);
}

TEST_CASE("ArtifactRegistry: publishes ArtifactRenamedEvent", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    record.display_name = "old.md";
    auto artifact_id = registry.register_artifact(record);

    std::string old_name;
    std::string new_name;
    auto sub = bus.subscribe<events::ArtifactRenamedEvent>(
        [&](const events::ArtifactRenamedEvent& evt)
        {
            old_name = evt.old_name;
            new_name = evt.new_name;
        });

    registry.rename(artifact_id, "new.md");
    REQUIRE(old_name == "old.md");
    REQUIRE(new_name == "new.md");
}

TEST_CASE("ArtifactRegistry: publishes ArtifactPromotedEvent", "[v20][artifact-registry]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);

    ArtifactRecord record;
    auto artifact_id = registry.register_artifact(record);

    std::string promoted_path;
    auto sub = bus.subscribe<events::ArtifactPromotedEvent>(
        [&promoted_path](const events::ArtifactPromotedEvent& evt)
        { promoted_path = evt.file_path; });

    registry.promote_to_saved(artifact_id, "/saved/path.md");
    REQUIRE(promoted_path == "/saved/path.md");
}
