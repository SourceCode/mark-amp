// Phase 34 Batch 34B — Task 5: EventBus stress tests
// High-frequency publishing, many subscribers, cross-thread, RAII cleanup.

#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <string>
#include <thread>
#include <vector>

using namespace markamp::core;
using namespace markamp::core::events;

namespace
{
auto make_config_evt(const std::string& key_val,
                     const std::string& old_val,
                     const std::string& new_val) -> ConfigChangedEvent
{
    ConfigChangedEvent evt;
    evt.key = key_val;
    evt.old_value = old_val;
    evt.new_value = new_val;
    return evt;
}
} // namespace

// ---------------------------------------------------------------------------
// High-frequency publishing
// ---------------------------------------------------------------------------

TEST_CASE("EventBus — 10,000 events all delivered", "[eventbus][stress]")
{
    EventBus bus;
    int received = 0;

    auto sub =
        bus.subscribe<ConfigChangedEvent>([&](const ConfigChangedEvent& /*evt*/) { ++received; });

    constexpr int kCount = 10'000;
    for (int idx = 0; idx < kCount; ++idx)
    {
        bus.publish(make_config_evt("k", "a", "b"));
    }

    REQUIRE(received == kCount);
}

// ---------------------------------------------------------------------------
// Many subscribers
// ---------------------------------------------------------------------------

TEST_CASE("EventBus — 100 subscribers all receive", "[eventbus][stress]")
{
    EventBus bus;
    constexpr int kSubCount = 100;
    std::vector<Subscription> subs;
    std::vector<int> counts(kSubCount, 0);

    for (int idx = 0; idx < kSubCount; ++idx)
    {
        subs.push_back(
            bus.subscribe<ConfigChangedEvent>([&counts, idx](const ConfigChangedEvent& /*evt*/)
                                              { ++counts[static_cast<size_t>(idx)]; }));
    }

    bus.publish(make_config_evt("k", "a", "b"));

    for (int idx = 0; idx < kSubCount; ++idx)
    {
        REQUIRE(counts[static_cast<size_t>(idx)] == 1);
    }
}

// ---------------------------------------------------------------------------
// RAII subscription cleanup
// ---------------------------------------------------------------------------

TEST_CASE("EventBus — RAII cleanup under load", "[eventbus][stress]")
{
    EventBus bus;
    int received = 0;

    // Subscribe and unsubscribe in a tight loop
    for (int idx = 0; idx < 1000; ++idx)
    {
        auto sub = bus.subscribe<ConfigChangedEvent>([&](const ConfigChangedEvent& /*evt*/)
                                                     { ++received; });
        bus.publish(make_config_evt("k", "a", "b"));
        // sub goes out of scope, unsubscribing
    }

    REQUIRE(received == 1000);

    // After all subs gone, publishing should not deliver
    received = 0;
    bus.publish(make_config_evt("k", "a", "b"));
    REQUIRE(received == 0);
}

// ---------------------------------------------------------------------------
// Cross-thread publishing
// ---------------------------------------------------------------------------

TEST_CASE("EventBus — 4 threads publishing concurrently", "[eventbus][stress][thread]")
{
    EventBus bus;
    std::atomic<int> received{0};

    auto sub =
        bus.subscribe<ConfigChangedEvent>([&](const ConfigChangedEvent& /*evt*/)
                                          { received.fetch_add(1, std::memory_order_relaxed); });

    constexpr int kPerThread = 1000;
    constexpr int kThreads = 4;

    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    for (int thr_idx = 0; thr_idx < kThreads; ++thr_idx)
    {
        threads.emplace_back(
            [&bus]()
            {
                for (int msg_idx = 0; msg_idx < kPerThread; ++msg_idx)
                {
                    bus.publish(make_config_evt("k", "a", "b"));
                }
            });
    }

    for (auto& thr : threads)
    {
        thr.join();
    }

    REQUIRE(received.load() == kThreads * kPerThread);
}

// ---------------------------------------------------------------------------
// Subscribe and publish different event types
// ---------------------------------------------------------------------------

TEST_CASE("EventBus — different event types don't interfere", "[eventbus][stress]")
{
    EventBus bus;
    int config_count = 0;
    int file_count = 0;

    auto sub1 = bus.subscribe<ConfigChangedEvent>([&](const ConfigChangedEvent& /*evt*/)
                                                  { ++config_count; });

    auto sub2 =
        bus.subscribe<FileOpenedEvent>([&](const FileOpenedEvent& /*evt*/) { ++file_count; });

    for (int idx = 0; idx < 100; ++idx)
    {
        bus.publish(make_config_evt("k", "a", "b"));
    }

    REQUIRE(config_count == 100);
    REQUIRE(file_count == 0);
}

// ---------------------------------------------------------------------------
// Memory: subscribe/unsubscribe cycles
// ---------------------------------------------------------------------------

TEST_CASE("EventBus — subscribe/unsubscribe cycle stability", "[eventbus][stress]")
{
    EventBus bus;
    int count = 0;

    // 500 cycles of subscribe → publish → unsubscribe
    for (int idx = 0; idx < 500; ++idx)
    {
        auto sub =
            bus.subscribe<ConfigChangedEvent>([&](const ConfigChangedEvent& /*evt*/) { ++count; });
        bus.publish(make_config_evt("k", "a", "b"));
    }

    REQUIRE(count == 500);
}

// ---------------------------------------------------------------------------
// Publish with no subscribers — no crash
// ---------------------------------------------------------------------------

TEST_CASE("EventBus — publish with no subscribers", "[eventbus][stress]")
{
    EventBus bus;
    // Should not crash or throw
    for (int idx = 0; idx < 1000; ++idx)
    {
        bus.publish(make_config_evt("k", "a", "b"));
    }
    REQUIRE(true); // Reached without crash
}
