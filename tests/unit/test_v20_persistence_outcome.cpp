/// @file test_v20_persistence_outcome.cpp
/// @brief V20 Phase 05 – PersistenceOutcomeService unit tests.

#include "core/PersistenceOutcome.h"
#include "core/ArtifactCreationService.h"
#include "core/ArtifactRegistry.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct PersistFixture
{
    EventBus bus;
    ArtifactRegistry registry{bus};
    Config config;
    ArtifactCreationService creation{bus, registry, config};
    PersistenceOutcomeService persistence{bus, registry};

    auto add_artifact(const std::string& name) -> ArtifactId
    {
        ArtifactCreationRequest req;
        req.kind = ArtifactKind::kTextFile;
        req.display_name = name;
        req.language_id = "markdown";
        req.source = "test";
        return creation.create(req).id;
    }
};

TEST_CASE("PersistenceOutcome: construction", "[v20][persistence]")
{
    PersistFixture fix;
    REQUIRE(fix.persistence.save_count() == 0);
    REQUIRE(fix.persistence.failure_count() == 0);
}

TEST_CASE("PersistenceOutcome: execute_save succeeds", "[v20][persistence]")
{
    PersistFixture fix;
    auto id = fix.add_artifact("test.md");

    auto outcome = fix.persistence.execute_save(id, "/workspace/test.md");
    REQUIRE(outcome.ok());
    REQUIRE(outcome.was_atomic);
    REQUIRE(fix.persistence.save_count() == 1);

    const auto* record = fix.registry.find(id);
    REQUIRE(record->is_saved());
}

TEST_CASE("PersistenceOutcome: save nonexistent fails", "[v20][persistence]")
{
    PersistFixture fix;
    auto outcome = fix.persistence.execute_save(ArtifactId{"fake"}, "/path");
    REQUIRE_FALSE(outcome.ok());
    REQUIRE(outcome.error_code == "NOT_FOUND");
    REQUIRE(fix.persistence.failure_count() == 1);
}

TEST_CASE("PersistenceOutcome: execute_save_with custom executor", "[v20][persistence]")
{
    PersistFixture fix;
    auto id = fix.add_artifact("doc.md");

    // Failing executor
    SaveExecutor failing = [](const ArtifactId& aid, const std::string&) {
        SaveOutcome o;
        o.artifact_id = aid;
        o.error_message = "Disk full";
        return o;
    };

    auto outcome = fix.persistence.execute_save_with(id, "/workspace/doc.md", failing);
    REQUIRE_FALSE(outcome.ok());
    REQUIRE(outcome.error_message == "Disk full");
    REQUIRE(fix.persistence.failure_count() == 1);

    // Artifact should remain dirty, not clean
    const auto* record = fix.registry.find(id);
    REQUIRE(record->is_dirty());
}

TEST_CASE("PersistenceOutcome: retry_save", "[v20][persistence]")
{
    PersistFixture fix;
    auto id = fix.add_artifact("retry.md");

    fix.persistence.record_failure(id, "Temporary error");

    auto retry = fix.persistence.retry_save(id);
    REQUIRE(retry.ok());
}

TEST_CASE("PersistenceOutcome: last_outcome tracking", "[v20][persistence]")
{
    PersistFixture fix;
    auto id = fix.add_artifact("track.md");

    REQUIRE_FALSE(fix.persistence.last_outcome(id).has_value());

    fix.persistence.execute_save(id, "/workspace/track.md");
    auto last = fix.persistence.last_outcome(id);
    REQUIRE(last.has_value());
    REQUIRE(last->ok());
}

TEST_CASE("PersistenceOutcome: record_failure", "[v20][persistence]")
{
    PersistFixture fix;
    auto id = fix.add_artifact("fail.md");

    fix.persistence.record_failure(id, "Permission denied");
    REQUIRE(fix.persistence.failure_count() == 1);

    auto last = fix.persistence.last_outcome(id);
    REQUIRE(last.has_value());
    REQUIRE_FALSE(last->ok());
}

TEST_CASE("SaveOutcome: is_retriable", "[v20][persistence]")
{
    SaveOutcome perm;
    perm.error_code = "PERMISSION_DENIED";
    REQUIRE_FALSE(perm.is_retriable());

    SaveOutcome disk;
    disk.error_code = "DISK_FULL";
    REQUIRE(disk.is_retriable());
}
