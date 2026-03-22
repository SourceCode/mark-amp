/// @file test_v25_p03_persistence.cpp
/// @brief V25 Phase 03: Persistence coordinator tests.
#include <catch2/catch_test_macros.hpp>
#include "core/PersistenceCoordinator.h"
#include "core/ArtifactCreationService.h"
#include "core/Config.h"

using namespace markamp::core;

TEST_CASE("V25 P03: Save dispatches by artifact kind", "[v25][p03]")
{
    EventBus bus;
    Config cfg;
    ArtifactRegistry registry(bus);
    ArtifactCreationService service(bus, registry, cfg);
    PersistenceCoordinator coord(bus, registry);

    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kTextFile;
    req.display_name = "test.md";
    auto created = service.create(req);
    REQUIRE(created.success);

    auto result = coord.save(created.id);
    REQUIRE(result.ok());
    REQUIRE(coord.save_count() == 1);
}

TEST_CASE("V25 P03: Save fails for unknown artifact", "[v25][p03]")
{
    EventBus bus;
    ArtifactRegistry registry(bus);
    PersistenceCoordinator coord(bus, registry);

    ArtifactId unknown;
    unknown.value = "nonexistent";
    auto result = coord.save(unknown);
    REQUIRE_FALSE(result.ok());
}

TEST_CASE("V25 P03: Recovery metadata creation", "[v25][p03]")
{
    EventBus bus;
    Config cfg;
    ArtifactRegistry registry(bus);
    ArtifactCreationService service(bus, registry, cfg);
    PersistenceCoordinator coord(bus, registry);

    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kTextFile;
    auto created = service.create(req);

    auto meta = coord.create_recovery_metadata(created.id);
    REQUIRE(meta.is_valid);
    REQUIRE(meta.has_draft());
    REQUIRE(coord.recovery_draft_count() == 1);
}

TEST_CASE("V25 P03: Restore entry validation", "[v25][p03]")
{
    EventBus bus;
    Config cfg;
    ArtifactRegistry registry(bus);
    ArtifactCreationService service(bus, registry, cfg);
    PersistenceCoordinator coord(bus, registry);

    ArtifactCreationRequest req;
    req.kind = ArtifactKind::kTextFile;
    auto created = service.create(req);

    auto validation = coord.validate_restore_entry(created.id);
    REQUIRE(validation.is_valid);
    REQUIRE(validation.artifact_exists);
}
