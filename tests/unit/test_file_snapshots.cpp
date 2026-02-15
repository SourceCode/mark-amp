/// @file test_file_snapshots.cpp
/// @brief V4 Phase 39 – FileSnapshotService tests.

#include "core/EventBus.h"
#include "core/FileSnapshotService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct SnapshotFixture
{
    EventBus event_bus;
    FileSnapshotService service{event_bus};
};

TEST_CASE("FileSnapshotService: create snapshot", "[file_snapshots]")
{
    SnapshotFixture fixture;
    auto sid = fixture.service.create_snapshot("/test.md", "Hello World");

    REQUIRE(!sid.empty());
    REQUIRE(fixture.service.snapshot_count("/test.md") == 1);
}

TEST_CASE("FileSnapshotService: deduplication skips identical content", "[file_snapshots]")
{
    SnapshotFixture fixture;
    auto sid1 = fixture.service.create_snapshot("/test.md", "Hello World");
    auto sid2 = fixture.service.create_snapshot("/test.md", "Hello World");

    // Same content should return same snapshot id.
    REQUIRE(sid1 == sid2);
    REQUIRE(fixture.service.snapshot_count("/test.md") == 1);
}

TEST_CASE("FileSnapshotService: different content creates new snapshot", "[file_snapshots]")
{
    SnapshotFixture fixture;
    fixture.service.create_snapshot("/test.md", "Version 1");
    fixture.service.create_snapshot("/test.md", "Version 2");

    REQUIRE(fixture.service.snapshot_count("/test.md") == 2);
}

TEST_CASE("FileSnapshotService: get snapshot content", "[file_snapshots]")
{
    SnapshotFixture fixture;
    auto sid = fixture.service.create_snapshot("/test.md", "Content here");
    auto content = fixture.service.get_snapshot(sid);

    REQUIRE(content.has_value());
    REQUIRE(*content == "Content here");
}

TEST_CASE("FileSnapshotService: restore snapshot", "[file_snapshots]")
{
    SnapshotFixture fixture;
    auto sid = fixture.service.create_snapshot("/test.md", "Original");
    auto restored = fixture.service.restore_snapshot(sid);

    REQUIRE(restored.has_value());
    REQUIRE(*restored == "Original");
}

TEST_CASE("FileSnapshotService: list snapshots in reverse order", "[file_snapshots]")
{
    SnapshotFixture fixture;
    auto sid1 = fixture.service.create_snapshot("/test.md", "V1");
    auto sid2 = fixture.service.create_snapshot("/test.md", "V2");
    auto sid3 = fixture.service.create_snapshot("/test.md", "V3");

    auto snapshots = fixture.service.list_snapshots("/test.md");
    REQUIRE(snapshots.size() == 3);
    // Newest first.
    REQUIRE(snapshots[0].snapshot_id == sid3);
    REQUIRE(snapshots[2].snapshot_id == sid1);
}

TEST_CASE("FileSnapshotService: diff snapshots", "[file_snapshots]")
{
    SnapshotFixture fixture;
    auto sid1 = fixture.service.create_snapshot("/test.md", "Line 1\nLine 2\nLine 3");
    auto sid2 =
        fixture.service.create_snapshot("/test.md", "Line 1\nLine 2 modified\nLine 3\nLine 4");

    auto diff = fixture.service.diff_snapshots(sid1, sid2);
    REQUIRE(diff.changed_lines == 1); // "Line 2" -> "Line 2 modified"
    REQUIRE(diff.added_lines == 1);   // "Line 4" added
    REQUIRE(!diff.unified_diff.empty());
}

TEST_CASE("FileSnapshotService: prune old snapshots", "[file_snapshots]")
{
    SnapshotFixture fixture;
    fixture.service.create_snapshot("/test.md", "V1");
    fixture.service.create_snapshot("/test.md", "V2");
    fixture.service.create_snapshot("/test.md", "V3");
    fixture.service.create_snapshot("/test.md", "V4");
    fixture.service.create_snapshot("/test.md", "V5");

    auto pruned = fixture.service.prune_old("/test.md", 3);
    REQUIRE(pruned == 2);
    REQUIRE(fixture.service.snapshot_count("/test.md") == 3);
}

TEST_CASE("FileSnapshotService: total storage bytes", "[file_snapshots]")
{
    SnapshotFixture fixture;
    fixture.service.create_snapshot("/a.md", "Short");
    fixture.service.create_snapshot("/b.md", "Longer content here");

    auto bytes = fixture.service.total_storage_bytes();
    REQUIRE(bytes == 5 + 19); // "Short" + "Longer content here"
}

TEST_CASE("FileSnapshotService: nonexistent snapshot returns nullopt", "[file_snapshots]")
{
    SnapshotFixture fixture;
    auto content = fixture.service.get_snapshot("nonexistent-id");

    REQUIRE(!content.has_value());
}
