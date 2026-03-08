// test_sync_conflict_manager.cpp — 10 tests for SyncConflictManager
#include "core/EventBus.h"
#include "core/SyncConflictManager.h"
#include "core/SyncEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SyncConflictManager starts with no conflicts", "[sync][conflict]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager manager(bus, engine);
    CHECK_FALSE(manager.has_conflicts());
    CHECK(manager.pending_count() == 0);
}

TEST_CASE("SyncConflictManager set_pending_conflicts", "[sync][conflict]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager manager(bus, engine);
    SyncConflict conflict;
    conflict.relative_path = "notes.md";
    manager.set_pending_conflicts({conflict});
    CHECK(manager.has_conflicts());
    CHECK(manager.pending_count() == 1);
}

TEST_CASE("SyncConflictManager default_strategy is KeepNewer", "[sync][conflict]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager manager(bus, engine);
    CHECK(manager.default_strategy() == SyncConflictResolution::KeepNewer);
}

TEST_CASE("SyncConflictManager set_default_strategy", "[sync][conflict]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager manager(bus, engine);
    manager.set_default_strategy(SyncConflictResolution::KeepLocal);
    CHECK(manager.default_strategy() == SyncConflictResolution::KeepLocal);
}

TEST_CASE("SyncConflictManager find_conflict", "[sync][conflict]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager manager(bus, engine);
    SyncConflict conflict;
    conflict.relative_path = "readme.md";
    manager.set_pending_conflicts({conflict});
    auto found = manager.find_conflict("readme.md");
    REQUIRE(found.has_value());
}

TEST_CASE("SyncConflictManager find_conflict missing returns nullopt", "[sync][conflict]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager manager(bus, engine);
    auto found = manager.find_conflict("ghost.md");
    CHECK_FALSE(found.has_value());
}

TEST_CASE("SyncConflictManager resolve_all returns count", "[sync][conflict]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager manager(bus, engine);
    SyncConflict c1;
    c1.relative_path = "a.md";
    SyncConflict c2;
    c2.relative_path = "b.md";
    manager.set_pending_conflicts({c1, c2});
    auto resolved = manager.resolve_all(SyncConflictResolution::KeepLocal);
    CHECK(resolved == 2);
    CHECK_FALSE(manager.has_conflicts());
}

TEST_CASE("SyncConflictManager resolution_history tracks resolutions", "[sync][conflict]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager manager(bus, engine);
    SyncConflict conflict;
    conflict.relative_path = "doc.md";
    manager.set_pending_conflicts({conflict});
    manager.resolve_conflict("doc.md", SyncConflictResolution::KeepRemote);
    CHECK(manager.resolution_count() >= 1);
}

TEST_CASE("SyncConflictManager clear_pending", "[sync][conflict]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager manager(bus, engine);
    SyncConflict conflict;
    conflict.relative_path = "file.md";
    manager.set_pending_conflicts({conflict});
    manager.clear_pending();
    CHECK_FALSE(manager.has_conflicts());
}

TEST_CASE("SyncConflictManager clear_history", "[sync][conflict]")
{
    EventBus bus;
    SyncEngine engine(bus);
    SyncConflictManager manager(bus, engine);
    SyncConflict conflict;
    conflict.relative_path = "file.md";
    manager.set_pending_conflicts({conflict});
    manager.resolve_conflict("file.md", SyncConflictResolution::KeepLocal);
    manager.clear_history();
    CHECK(manager.resolution_count() == 0);
}
