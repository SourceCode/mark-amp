// test_sync_scheduler.cpp — 10 tests for SyncScheduler
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/SyncScheduler.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SyncScheduler default interval is 30min", "[sync][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);
    CHECK(scheduler.interval() == SyncScheduleInterval::k30Min);
}

TEST_CASE("SyncScheduler set interval", "[sync][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);
    scheduler.set_interval(SyncScheduleInterval::k5Min);
    CHECK(scheduler.interval() == SyncScheduleInterval::k5Min);
}

TEST_CASE("SyncScheduler pause and resume", "[sync][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);
    CHECK_FALSE(scheduler.is_paused());
    scheduler.pause();
    CHECK(scheduler.is_paused());
    scheduler.resume();
    CHECK_FALSE(scheduler.is_paused());
}

TEST_CASE("SyncScheduler is online by default", "[sync][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);
    CHECK(scheduler.is_online());
}

TEST_CASE("SyncScheduler set_online toggles offline", "[sync][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);
    scheduler.set_online(false);
    CHECK_FALSE(scheduler.is_online());
}

TEST_CASE("SyncScheduler offline queue management", "[sync][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);
    scheduler.queue_offline_change("file1.md");
    scheduler.queue_offline_change("file2.md");
    CHECK(scheduler.pending_count() == 2);
    CHECK(scheduler.pending_files().size() == 2);
}

TEST_CASE("SyncScheduler clear_offline_queue", "[sync][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);
    scheduler.queue_offline_change("file.md");
    scheduler.clear_offline_queue();
    CHECK(scheduler.pending_count() == 0);
}

TEST_CASE("SyncScheduler sync_on_close_enabled default", "[sync][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);
    CHECK(scheduler.sync_on_close_enabled());
    scheduler.set_sync_on_close_enabled(false);
    CHECK_FALSE(scheduler.sync_on_close_enabled());
}

TEST_CASE("SyncScheduler save_debounce_ms default", "[sync][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);
    CHECK(scheduler.save_debounce_ms() == 30000);
    scheduler.set_save_debounce_ms(5000);
    CHECK(scheduler.save_debounce_ms() == 5000);
}

TEST_CASE("SyncScheduler last_sync_time tracking", "[sync][scheduler]")
{
    EventBus bus;
    Config config;
    SyncScheduler scheduler(bus, config);
    CHECK(scheduler.last_sync_time() == 0);
    scheduler.set_last_sync_time(1234567890);
    CHECK(scheduler.last_sync_time() == 1234567890);
}
