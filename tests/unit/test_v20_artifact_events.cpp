/// @file test_v20_artifact_events.cpp
/// @brief V20 Phase 01 – Artifact event type tests.

#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ============================================================================
// Event instantiation and fields
// ============================================================================

TEST_CASE("ArtifactCreatedEvent: construction and fields", "[v20][artifact-events]")
{
    ArtifactCreatedEvent evt;
    evt.artifact_id = "test-id";
    evt.artifact_kind = 0;
    evt.display_name = "test.md";
    evt.source = "menu";

    REQUIRE(evt.artifact_id == "test-id");
    REQUIRE(evt.artifact_kind == 0);
    REQUIRE(evt.display_name == "test.md");
    REQUIRE(evt.source == "menu");
}

TEST_CASE("ArtifactRegisteredEvent: fields", "[v20][artifact-events]")
{
    ArtifactRegisteredEvent evt;
    evt.artifact_id = "reg-id";
    evt.artifact_kind = 1;

    REQUIRE(evt.artifact_id == "reg-id");
    REQUIRE(evt.artifact_kind == 1);
}

TEST_CASE("ArtifactUnregisteredEvent: fields", "[v20][artifact-events]")
{
    ArtifactUnregisteredEvent evt;
    evt.artifact_id = "unreg-id";
    REQUIRE(evt.artifact_id == "unreg-id");
}

TEST_CASE("ArtifactStateChangedEvent: fields", "[v20][artifact-events]")
{
    ArtifactStateChangedEvent evt;
    evt.artifact_id = "state-id";
    evt.old_state = 0;
    evt.new_state = 2;

    REQUIRE(evt.artifact_id == "state-id");
    REQUIRE(evt.old_state == 0);
    REQUIRE(evt.new_state == 2);
}

TEST_CASE("ArtifactActivatedEvent: fields", "[v20][artifact-events]")
{
    ArtifactActivatedEvent evt;
    evt.artifact_id = "active-id";
    evt.previous_artifact_id = "prev-id";

    REQUIRE(evt.artifact_id == "active-id");
    REQUIRE(evt.previous_artifact_id == "prev-id");
}

TEST_CASE("ArtifactMountedEvent: fields", "[v20][artifact-events]")
{
    ArtifactMountedEvent evt;
    evt.artifact_id = "mount-id";
    evt.surface_kind = 2;

    REQUIRE(evt.artifact_id == "mount-id");
    REQUIRE(evt.surface_kind == 2);
}

TEST_CASE("ArtifactUnmountedEvent: fields", "[v20][artifact-events]")
{
    ArtifactUnmountedEvent evt;
    evt.artifact_id = "unmount-id";
    REQUIRE(evt.artifact_id == "unmount-id");
}

TEST_CASE("ArtifactRenamedEvent: fields", "[v20][artifact-events]")
{
    ArtifactRenamedEvent evt;
    evt.artifact_id = "rename-id";
    evt.old_name = "old.md";
    evt.new_name = "new.md";

    REQUIRE(evt.artifact_id == "rename-id");
    REQUIRE(evt.old_name == "old.md");
    REQUIRE(evt.new_name == "new.md");
}

TEST_CASE("ArtifactPromotedEvent: fields", "[v20][artifact-events]")
{
    ArtifactPromotedEvent evt;
    evt.artifact_id = "promo-id";
    evt.file_path = "/workspace/saved.md";

    REQUIRE(evt.artifact_id == "promo-id");
    REQUIRE(evt.file_path == "/workspace/saved.md");
}

TEST_CASE("ArtifactSaveRequestEvent: fields", "[v20][artifact-events]")
{
    ArtifactSaveRequestEvent evt;
    evt.artifact_id = "save-id";
    REQUIRE(evt.artifact_id == "save-id");
}

// ============================================================================
// EventBus publish/subscribe
// ============================================================================

TEST_CASE("Artifact events: publish through EventBus", "[v20][artifact-events]")
{
    EventBus bus;

    bool created_received = false;
    bool registered_received = false;
    bool state_changed_received = false;

    auto sub1 = bus.subscribe<ArtifactCreatedEvent>(
        [&](const ArtifactCreatedEvent& /*evt*/) { created_received = true; });
    auto sub2 = bus.subscribe<ArtifactRegisteredEvent>(
        [&](const ArtifactRegisteredEvent& /*evt*/) { registered_received = true; });
    auto sub3 = bus.subscribe<ArtifactStateChangedEvent>(
        [&](const ArtifactStateChangedEvent& /*evt*/) { state_changed_received = true; });

    ArtifactCreatedEvent e1;
    bus.publish(e1);
    REQUIRE(created_received);

    ArtifactRegisteredEvent e2;
    bus.publish(e2);
    REQUIRE(registered_received);

    ArtifactStateChangedEvent e3;
    bus.publish(e3);
    REQUIRE(state_changed_received);
}

TEST_CASE("Artifact events: multiple subscribers receive same event", "[v20][artifact-events]")
{
    EventBus bus;

    int receive_count = 0;
    auto sub1 = bus.subscribe<ArtifactCreatedEvent>(
        [&](const ArtifactCreatedEvent& /*evt*/) { ++receive_count; });
    auto sub2 = bus.subscribe<ArtifactCreatedEvent>(
        [&](const ArtifactCreatedEvent& /*evt*/) { ++receive_count; });

    ArtifactCreatedEvent evt;
    bus.publish(evt);
    REQUIRE(receive_count == 2);
}

TEST_CASE("Artifact events: unsubscribe stops delivery", "[v20][artifact-events]")
{
    EventBus bus;

    int receive_count = 0;
    {
        auto sub = bus.subscribe<ArtifactCreatedEvent>(
            [&](const ArtifactCreatedEvent& /*evt*/) { ++receive_count; });

        ArtifactCreatedEvent evt;
        bus.publish(evt);
        REQUIRE(receive_count == 1);
    } // sub goes out of scope, unsubscribing

    ArtifactCreatedEvent evt2;
    bus.publish(evt2);
    REQUIRE(receive_count == 1); // No additional delivery
}
