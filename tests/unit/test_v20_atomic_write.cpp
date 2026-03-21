/// @file test_v20_atomic_write.cpp
/// @brief V20 Phase 05 – AtomicWriteService unit tests.

#include "core/AtomicWriteService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AtomicWrite: construction", "[v20][atomic-write]")
{
    AtomicWriteService service;
    REQUIRE(service.write_count() == 0);
    REQUIRE(service.backup_count() == 0);
    REQUIRE(service.journal_entry_count() == 0);
}

TEST_CASE("AtomicWrite: write_atomic success", "[v20][atomic-write]")
{
    AtomicWriteService service;

    auto result = service.write_atomic("/workspace/test.md", "# Hello");
    REQUIRE(result.ok());
    REQUIRE(result.final_path == "/workspace/test.md");
    REQUIRE(result.bytes_written == 7);
    REQUIRE(result.used_temp_file);
    REQUIRE(service.write_count() == 1);
}

TEST_CASE("AtomicWrite: write_atomic empty path fails", "[v20][atomic-write]")
{
    AtomicWriteService service;
    auto result = service.write_atomic("", "content");
    REQUIRE_FALSE(result.ok());
}

TEST_CASE("AtomicWrite: create_backup", "[v20][atomic-write]")
{
    AtomicWriteService service;
    auto result = service.create_backup("/workspace/notes.md");
    REQUIRE(result.ok());
    REQUIRE(result.backup_path == "/workspace/notes.md.bak");
    REQUIRE(service.backup_count() == 1);
}

TEST_CASE("AtomicWrite: create_backup empty path fails", "[v20][atomic-write]")
{
    AtomicWriteService service;
    auto result = service.create_backup("");
    REQUIRE_FALSE(result.ok());
}

TEST_CASE("AtomicWrite: recovery journal", "[v20][atomic-write]")
{
    AtomicWriteService service;

    service.record_journal_entry("art-1", "/workspace/doc.md", "/tmp/journal/art-1.journal");
    service.record_journal_entry("art-2", "/workspace/note.md", "/tmp/journal/art-2.journal");

    REQUIRE(service.journal_entry_count() == 2);

    auto& entries = service.pending_recoveries();
    REQUIRE(entries.size() == 2);
    REQUIRE(entries[0].artifact_id == "art-1");
    REQUIRE_FALSE(entries[0].is_complete);
}

TEST_CASE("AtomicWrite: resolve_recovery", "[v20][atomic-write]")
{
    AtomicWriteService service;
    service.record_journal_entry("art-1", "/doc.md", "/journal/art-1.journal");

    service.resolve_recovery("art-1");
    REQUIRE(service.pending_recoveries()[0].is_complete);
}

TEST_CASE("AtomicWrite: cleanup_journals", "[v20][atomic-write]")
{
    AtomicWriteService service;
    service.record_journal_entry("art-1", "/a.md", "/j/1.journal");
    service.record_journal_entry("art-2", "/b.md", "/j/2.journal");

    service.resolve_recovery("art-1");
    service.cleanup_journals();

    REQUIRE(service.journal_entry_count() == 1);
    REQUIRE(service.pending_recoveries()[0].artifact_id == "art-2");
}

TEST_CASE("AtomicWrite: max backup rotations", "[v20][atomic-write]")
{
    REQUIRE(AtomicWriteService::kMaxBackupRotations == 3);
}
