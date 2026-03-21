/// @file test_v20_artifact_mount.cpp
/// @brief V20 Phase 01 – ArtifactMountService unit tests.

#include "core/ArtifactMountAdapter.h"
#include "core/ArtifactRegistry.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

/// Test mount adapter that records calls and succeeds.
class FakeTextMountAdapter : public IArtifactMountAdapter
{
public:
    int mount_calls{0};
    int unmount_calls{0};
    bool should_succeed{true};

    auto mount(const ArtifactMountRequest& /*request*/, const ArtifactRecord& /*record*/)
        -> ArtifactMountResult override
    {
        ++mount_calls;
        return {should_succeed, should_succeed ? "" : "Fake mount failed"};
    }

    auto unmount(const ArtifactId& /*artifact_id*/) -> bool override
    {
        ++unmount_calls;
        return true;
    }
};

// ============================================================================
// Construction
// ============================================================================

TEST_CASE("ArtifactMountService: construction", "[v20][artifact-mount]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    ArtifactMountService mount_service(bus, registry);

    REQUIRE(mount_service.mounted_count() == 0);
    REQUIRE_FALSE(mount_service.has_adapter(ArtifactKind::kTextFile));
}

// ============================================================================
// Adapter registration
// ============================================================================

TEST_CASE("ArtifactMountService: register adapter", "[v20][artifact-mount]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    ArtifactMountService mount_service(bus, registry);
    FakeTextMountAdapter adapter;

    mount_service.register_adapter(ArtifactKind::kTextFile, &adapter);
    REQUIRE(mount_service.has_adapter(ArtifactKind::kTextFile));
    REQUIRE_FALSE(mount_service.has_adapter(ArtifactKind::kNotebook));
}

// ============================================================================
// Mount
// ============================================================================

TEST_CASE("ArtifactMountService: mount artifact", "[v20][artifact-mount]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    ArtifactMountService mount_service(bus, registry);
    FakeTextMountAdapter adapter;
    mount_service.register_adapter(ArtifactKind::kTextFile, &adapter);

    ArtifactRecord record;
    record.kind = ArtifactKind::kTextFile;
    record.display_name = "test.md";
    auto artifact_id = registry.register_artifact(record);

    auto result = mount_service.mount_artifact(artifact_id);
    REQUIRE(result.success);
    REQUIRE(adapter.mount_calls == 1);
    REQUIRE(mount_service.mounted_count() == 1);
    REQUIRE(mount_service.is_mounted(artifact_id));
}

TEST_CASE("ArtifactMountService: mount nonexistent artifact fails", "[v20][artifact-mount]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    ArtifactMountService mount_service(bus, registry);

    ArtifactId fake_id{"nonexistent"};
    auto result = mount_service.mount_artifact(fake_id);
    REQUIRE_FALSE(result.success);
    REQUIRE(mount_service.mounted_count() == 0);
}

TEST_CASE("ArtifactMountService: mount with no adapter fails", "[v20][artifact-mount]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    ArtifactMountService mount_service(bus, registry);

    ArtifactRecord record;
    record.kind = ArtifactKind::kNotebook;
    auto artifact_id = registry.register_artifact(record);

    auto result = mount_service.mount_artifact(artifact_id);
    REQUIRE_FALSE(result.success);
}

TEST_CASE("ArtifactMountService: mount publishes ArtifactMountedEvent", "[v20][artifact-mount]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    ArtifactMountService mount_service(bus, registry);
    FakeTextMountAdapter adapter;
    mount_service.register_adapter(ArtifactKind::kTextFile, &adapter);

    ArtifactRecord record;
    record.kind = ArtifactKind::kTextFile;
    auto artifact_id = registry.register_artifact(record);

    std::string mounted_id;
    auto sub = bus.subscribe<events::ArtifactMountedEvent>(
        [&mounted_id](const events::ArtifactMountedEvent& evt)
        { mounted_id = evt.artifact_id; });

    mount_service.mount_artifact(artifact_id);
    REQUIRE(mounted_id == artifact_id.value);
}

TEST_CASE("ArtifactMountService: failed mount does not publish event", "[v20][artifact-mount]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    ArtifactMountService mount_service(bus, registry);
    FakeTextMountAdapter adapter;
    adapter.should_succeed = false;
    mount_service.register_adapter(ArtifactKind::kTextFile, &adapter);

    ArtifactRecord record;
    record.kind = ArtifactKind::kTextFile;
    auto artifact_id = registry.register_artifact(record);

    bool event_fired = false;
    auto sub = bus.subscribe<events::ArtifactMountedEvent>(
        [&event_fired](const events::ArtifactMountedEvent& /*evt*/) { event_fired = true; });

    auto result = mount_service.mount_artifact(artifact_id);
    REQUIRE_FALSE(result.success);
    REQUIRE_FALSE(event_fired);
    REQUIRE(mount_service.mounted_count() == 0);
}

// ============================================================================
// Unmount
// ============================================================================

TEST_CASE("ArtifactMountService: unmount artifact", "[v20][artifact-mount]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    ArtifactMountService mount_service(bus, registry);
    FakeTextMountAdapter adapter;
    mount_service.register_adapter(ArtifactKind::kTextFile, &adapter);

    ArtifactRecord record;
    record.kind = ArtifactKind::kTextFile;
    auto artifact_id = registry.register_artifact(record);

    mount_service.mount_artifact(artifact_id);
    REQUIRE(mount_service.is_mounted(artifact_id));

    REQUIRE(mount_service.unmount_artifact(artifact_id));
    REQUIRE_FALSE(mount_service.is_mounted(artifact_id));
    REQUIRE(mount_service.mounted_count() == 0);
    REQUIRE(adapter.unmount_calls == 1);
}

TEST_CASE("ArtifactMountService: unmount non-mounted fails", "[v20][artifact-mount]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    ArtifactMountService mount_service(bus, registry);

    ArtifactId fake_id{"not-mounted"};
    REQUIRE_FALSE(mount_service.unmount_artifact(fake_id));
}

TEST_CASE("ArtifactMountService: unmount publishes ArtifactUnmountedEvent",
          "[v20][artifact-mount]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    ArtifactMountService mount_service(bus, registry);
    FakeTextMountAdapter adapter;
    mount_service.register_adapter(ArtifactKind::kTextFile, &adapter);

    ArtifactRecord record;
    record.kind = ArtifactKind::kTextFile;
    auto artifact_id = registry.register_artifact(record);
    mount_service.mount_artifact(artifact_id);

    std::string unmounted_id;
    auto sub = bus.subscribe<events::ArtifactUnmountedEvent>(
        [&unmounted_id](const events::ArtifactUnmountedEvent& evt)
        { unmounted_id = evt.artifact_id; });

    mount_service.unmount_artifact(artifact_id);
    REQUIRE(unmounted_id == artifact_id.value);
}

// ============================================================================
// Is mounted
// ============================================================================

TEST_CASE("ArtifactMountService: is_mounted for unmounted returns false", "[v20][artifact-mount]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    ArtifactMountService mount_service(bus, registry);

    ArtifactId fake_id{"not-registered"};
    REQUIRE_FALSE(mount_service.is_mounted(fake_id));
}
