/// @file test_v20_session_restore.cpp
/// @brief V20 Phase 05 – SessionRestoreService unit tests.

#include "core/SessionRestoreService.h"
#include "core/ArtifactCreationService.h"
#include "core/ArtifactRegistry.h"
#include "core/Config.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct SessionFixture
{
    EventBus bus;
    ArtifactRegistry registry{bus};
    Config config;
    ArtifactCreationService creation{bus, registry, config};
    SessionRestoreService session{bus, registry, config};

    auto add_artifact(const std::string& name, ArtifactKind kind = ArtifactKind::kTextFile,
                       bool saved = false) -> ArtifactId
    {
        ArtifactCreationRequest req;
        req.kind = kind;
        req.display_name = name;
        req.language_id = "markdown";
        req.source = "test";
        auto result = creation.create(req);
        if (saved)
        {
            registry.promote_to_saved(result.id, "/workspace/" + name);
        }
        return result.id;
    }
};

TEST_CASE("SessionRestore: construction", "[v20][session-restore]")
{
    SessionFixture fix;
    REQUIRE(fix.session.restore_count() == 0);
    REQUIRE(fix.session.autosave_trigger_count() == 0);
}

TEST_CASE("SessionRestore: capture session", "[v20][session-restore]")
{
    SessionFixture fix;
    fix.add_artifact("file.md", ArtifactKind::kTextFile, true);
    fix.add_artifact("workspace.md", ArtifactKind::kTextFile);
    auto canvas = fix.add_artifact("session.json", ArtifactKind::kTextFile, true);
    fix.registry.set_active_artifact(canvas);

    auto records = fix.session.capture_session();
    REQUIRE(records.size() == 3);

    bool found_active = false;
    for (const auto& r : records)
    {
        if (r.was_active)
        {
            REQUIRE(r.kind == ArtifactKind::kTextFile);
            found_active = true;
        }
    }
    REQUIRE(found_active);
}

TEST_CASE("SessionRestore: apply session", "[v20][session-restore]")
{
    SessionFixture fix;

    std::vector<ArtifactSessionRecord> records;
    {
        ArtifactSessionRecord r;
        r.kind = ArtifactKind::kTextFile;
        r.display_name = "restored.md";
        r.file_path = "/workspace/restored.md";
        r.language_id = "markdown";
        r.was_active = true;
        records.push_back(r);
    }
    {
        ArtifactSessionRecord r;
        r.kind = ArtifactKind::kTextFile;
        r.display_name = "workspace.md";
        r.language_id = "notebook";
        records.push_back(r);
    }

    int restored = fix.session.apply_session(records);
    REQUIRE(restored == 2);
    REQUIRE(fix.registry.count() == 2);
    REQUIRE(fix.session.restore_count() == 1);

    // Active should be set to the was_active record
    REQUIRE_FALSE(fix.registry.active_artifact().empty());
}

TEST_CASE("SessionRestore: capture then restore round-trip", "[v20][session-restore]")
{
    SessionFixture fix;
    fix.add_artifact("a.md", ArtifactKind::kTextFile, true);
    fix.add_artifact("s.json", ArtifactKind::kTextFile, true);

    auto captured = fix.session.capture_session();
    REQUIRE(captured.size() == 2);

    // Clear registry, then restore
    EventBus bus2;
    ArtifactRegistry reg2{bus2};
    Config config2;
    SessionRestoreService session2{bus2, reg2, config2};

    int count = session2.apply_session(captured);
    REQUIRE(count == 2);
    REQUIRE(reg2.count() == 2);
}

TEST_CASE("SessionRestore: autosave policy", "[v20][session-restore]")
{
    SessionFixture fix;

    auto& policy = fix.session.autosave_policy();
    REQUIRE(policy.is_enabled());
    REQUIRE(policy.interval_seconds == 30);

    AutosavePolicy custom;
    custom.enabled = false;
    custom.interval_seconds = 60;
    fix.session.set_autosave_policy(custom);

    REQUIRE_FALSE(fix.session.autosave_policy().is_enabled());
}

TEST_CASE("SessionRestore: should_autosave", "[v20][session-restore]")
{
    SessionFixture fix;

    // No dirty artifacts
    REQUIRE_FALSE(fix.session.should_autosave());

    // Add and dirty an artifact
    auto id = fix.add_artifact("dirty.md", ArtifactKind::kTextFile, true);
    fix.registry.set_state(id, ArtifactLifecycleState::kDirty);

    REQUIRE(fix.session.should_autosave());
}

TEST_CASE("SessionRestore: trigger_autosave", "[v20][session-restore]")
{
    SessionFixture fix;

    auto id = fix.add_artifact("auto.md", ArtifactKind::kTextFile, true);
    fix.registry.set_state(id, ArtifactLifecycleState::kDirty);

    int saved = fix.session.trigger_autosave();
    REQUIRE(saved == 1);
    REQUIRE(fix.session.autosave_trigger_count() == 1);

    // Should be saved now
    REQUIRE(fix.registry.find(id)->is_saved());
}

TEST_CASE("SessionRestore: autosave disabled", "[v20][session-restore]")
{
    SessionFixture fix;

    AutosavePolicy disabled;
    disabled.enabled = false;
    fix.session.set_autosave_policy(disabled);

    auto id = fix.add_artifact("no-auto.md", ArtifactKind::kTextFile, true);
    fix.registry.set_state(id, ArtifactLifecycleState::kDirty);

    int saved = fix.session.trigger_autosave();
    REQUIRE(saved == 0);
}
