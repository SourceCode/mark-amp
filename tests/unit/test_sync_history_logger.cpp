// test_sync_history_logger.cpp — 10 tests for SyncHistoryLogger
#include "core/EventBus.h"
#include "core/SyncHistoryLogger.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SyncHistoryLogger starts empty", "[sync][history_logger]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);
    CHECK(logger.is_empty());
    CHECK(logger.entry_count() == 0);
}

TEST_CASE("SyncHistoryLogger log_entry increases count", "[sync][history_logger]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);
    SyncHistoryEntry entry;
    entry.operation = "sync";
    entry.files_uploaded = 3;
    logger.log_entry(entry);
    CHECK(logger.entry_count() == 1);
    CHECK_FALSE(logger.is_empty());
}

TEST_CASE("SyncHistoryLogger log_sync records result", "[sync][history_logger]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);
    CloudSyncResult result;
    result.files_uploaded = 5;
    result.files_downloaded = 2;
    result.status = CloudSyncStatus::Completed;
    logger.log_sync(result, "push", "MacBook");
    CHECK(logger.entry_count() == 1);
}

TEST_CASE("SyncHistoryLogger latest returns most recent", "[sync][history_logger]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);
    SyncHistoryEntry e1;
    e1.operation = "push";
    SyncHistoryEntry e2;
    e2.operation = "pull";
    logger.log_entry(e1);
    logger.log_entry(e2);
    auto latest = logger.latest();
    REQUIRE(latest.has_value());
}

TEST_CASE("SyncHistoryLogger search_by_operation", "[sync][history_logger]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);
    SyncHistoryEntry push_entry;
    push_entry.operation = "push";
    SyncHistoryEntry pull_entry;
    pull_entry.operation = "pull";
    logger.log_entry(push_entry);
    logger.log_entry(pull_entry);
    auto pushes = logger.search_by_operation("push");
    CHECK(pushes.size() == 1);
}

TEST_CASE("SyncHistoryLogger total_files_synced", "[sync][history_logger]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);
    SyncHistoryEntry entry;
    entry.files_uploaded = 10;
    entry.files_downloaded = 5;
    logger.log_entry(entry);
    CHECK(logger.total_files_synced() >= 15);
}

TEST_CASE("SyncHistoryLogger clear removes all", "[sync][history_logger]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);
    SyncHistoryEntry entry;
    entry.operation = "sync";
    logger.log_entry(entry);
    logger.clear();
    CHECK(logger.is_empty());
}

TEST_CASE("SyncHistoryLogger max_entries default", "[sync][history_logger]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);
    CHECK(logger.max_entries() == 200);
}

TEST_CASE("SyncHistoryLogger set_max_entries", "[sync][history_logger]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);
    logger.set_max_entries(50);
    CHECK(logger.max_entries() == 50);
}

TEST_CASE("SyncHistoryLogger export_json produces output", "[sync][history_logger]")
{
    EventBus bus;
    SyncHistoryLogger logger(bus);
    SyncHistoryEntry entry;
    entry.operation = "sync";
    logger.log_entry(entry);
    auto json = logger.export_json();
    CHECK_FALSE(json.empty());
}
