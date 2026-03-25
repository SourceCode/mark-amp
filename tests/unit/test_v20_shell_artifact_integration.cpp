/// @file test_v20_shell_artifact_integration.cpp
/// @brief V20 Phase 01 – WorkbenchShellController + Artifact integration tests.
///
/// Tests that the creation-to-registry-to-shell flow works end-to-end.

#include "core/ArtifactCreationService.h"
#include "core/ArtifactMountAdapter.h"
#include "core/ArtifactRegistry.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

/// Stub mount adapter for shell integration tests.
class StubMountAdapter : public IArtifactMountAdapter
{
public:
    int mount_calls{0};

    auto mount(const ArtifactMountRequest& /*request*/, const ArtifactRecord& /*record*/)
        -> ArtifactMountResult override
    {
        ++mount_calls;
        return {true, ""};
    }

    auto unmount(const ArtifactId& /*artifact_id*/) -> bool override { return true; }
};

// ============================================================================
// End-to-end: creation → registry → mount
// ============================================================================

TEST_CASE("Shell integration: create text file and mount", "[v20][shell-integration]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService creation(bus, registry, config);
    ArtifactMountService mount_service(bus, registry);
    StubMountAdapter text_adapter;
    mount_service.register_adapter(ArtifactKind::kTextFile, &text_adapter);

    // Create artifact
    auto result = creation.create_text_file("integration.md", "markdown", "test");
    REQUIRE(result.ok());
    REQUIRE(registry.count() == 1);

    // Mount it
    auto mount_result = mount_service.mount_artifact(result.id);
    REQUIRE(mount_result.success);
    REQUIRE(text_adapter.mount_calls == 1);
    REQUIRE(mount_service.is_mounted(result.id));
}

TEST_CASE("Shell integration: create notebook and mount", "[v20][shell-integration]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService creation(bus, registry, config);
    ArtifactMountService mount_service(bus, registry);
    StubMountAdapter nb_adapter;
    mount_service.register_adapter(ArtifactKind::kTextFile, &nb_adapter);

    auto result = creation.create_text_file("test-notebook", "test");
    REQUIRE(result.ok());

    auto mount_result = mount_service.mount_artifact(result.id);
    REQUIRE(mount_result.success);
    REQUIRE(nb_adapter.mount_calls == 1);
}

TEST_CASE("Shell integration: create canvas and mount", "[v20][shell-integration]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService creation(bus, registry, config);
    ArtifactMountService mount_service(bus, registry);
    StubMountAdapter canvas_adapter;
    mount_service.register_adapter(ArtifactKind::kTextFile, &canvas_adapter);

    auto result = creation.create_text_file("test-board", "test");
    REQUIRE(result.ok());

    auto mount_result = mount_service.mount_artifact(result.id);
    REQUIRE(mount_result.success);
    REQUIRE(canvas_adapter.mount_calls == 1);
}

// ============================================================================
// Active artifact flow
// ============================================================================

TEST_CASE("Shell integration: set active artifact after creation", "[v20][shell-integration]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService creation(bus, registry, config);

    auto r1 = creation.create_text_file("first.md");
    auto r2 = creation.create_text_file("second.md");

    registry.set_active_artifact(r1.id);
    REQUIRE(registry.active_artifact() == r1.id);

    registry.set_active_artifact(r2.id);
    REQUIRE(registry.active_artifact() == r2.id);
}

TEST_CASE("Shell integration: activate publishes with previous ID", "[v20][shell-integration]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService creation(bus, registry, config);

    auto r1 = creation.create_text_file("a.md");
    auto r2 = creation.create_text_file("b.md");

    registry.set_active_artifact(r1.id);

    std::string prev_id;
    auto sub = bus.subscribe<events::ArtifactActivatedEvent>(
        [&prev_id](const events::ArtifactActivatedEvent& evt)
        { prev_id = evt.previous_artifact_id; });

    registry.set_active_artifact(r2.id);
    REQUIRE(prev_id == r1.id.value);
}

// ============================================================================
// Lifecycle state transitions
// ============================================================================

TEST_CASE("Shell integration: full lifecycle (unsaved → dirty → saving → saved)",
          "[v20][shell-integration]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService creation(bus, registry, config);

    auto result = creation.create_text_file("lifecycle.md");
    REQUIRE(registry.find(result.id)->is_unsaved());

    // Simulate user edits
    REQUIRE(registry.set_state(result.id, ArtifactLifecycleState::kDirty));
    REQUIRE(registry.find(result.id)->is_dirty());

    // Simulate save in progress
    REQUIRE(registry.set_state(result.id, ArtifactLifecycleState::kSaving));
    REQUIRE(registry.find(result.id)->state == ArtifactLifecycleState::kSaving);

    // Simulate save completion with path promotion
    REQUIRE(registry.promote_to_saved(result.id, "/workspace/lifecycle.md"));
    REQUIRE(registry.find(result.id)->is_saved());
    REQUIRE(registry.find(result.id)->has_path());
}

TEST_CASE("Shell integration: save error recovery", "[v20][shell-integration]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService creation(bus, registry, config);

    auto result = creation.create_text_file("error.md");

    // Edit → save attempt → error → retry → success
    registry.set_state(result.id, ArtifactLifecycleState::kDirty);
    registry.set_state(result.id, ArtifactLifecycleState::kSaving);
    registry.set_state(result.id, ArtifactLifecycleState::kSaveError);
    REQUIRE(registry.find(result.id)->state == ArtifactLifecycleState::kSaveError);

    // Retry and succeed
    registry.set_state(result.id, ArtifactLifecycleState::kSaving);
    registry.promote_to_saved(result.id, "/workspace/error.md");
    REQUIRE(registry.find(result.id)->is_saved());
}

// ============================================================================
// Multiple artifact types in one session
// ============================================================================

TEST_CASE("Shell integration: mixed artifact types", "[v20][shell-integration]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService creation(bus, registry, config);

    creation.create_text_file("readme.md");
    creation.create_text_file("analysis");
    creation.create_text_file("brainstorm");
    creation.create_text_file("notes.md");

    REQUIRE(registry.count() == 4);
    REQUIRE(registry.count_by_kind(ArtifactKind::kTextFile) == 4);
}

// ============================================================================
// Unregister and re-create
// ============================================================================

TEST_CASE("Shell integration: unregister and re-create", "[v20][shell-integration]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    Config config;
    ArtifactCreationService creation(bus, registry, config);

    auto r1 = creation.create_text_file("temp.md");
    REQUIRE(registry.count() == 1);

    registry.unregister(r1.id);
    REQUIRE(registry.count() == 0);

    auto r2 = creation.create_text_file("temp2.md");
    REQUIRE(registry.count() == 1);
    REQUIRE(r1.id != r2.id); // New ID
}
