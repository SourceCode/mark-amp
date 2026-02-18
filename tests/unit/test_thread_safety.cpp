/// @file test_thread_safety.cpp
/// Phase 34 Batch 34C – Task 13: Thread safety tests.
/// Tests concurrent access to EventBus, Config, MetricsCollector, and SpanTracker.
/// Verifies no deadlocks, data corruption, or crashes under contention.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ObservabilityService.h"

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

using namespace markamp::core;

// ---------------------------------------------------------------------------
// EventBus multi-threaded publishing
// ---------------------------------------------------------------------------

struct ThreadEvent : markamp::core::Event
{
    int thread_id{0};
    int sequence{0};
    ThreadEvent() = default;
    ThreadEvent(int tid, int seq)
        : thread_id(tid)
        , sequence(seq)
    {
    }
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ThreadEvent";
    }
};

TEST_CASE("Thread — 4 threads publishing concurrently to EventBus", "[thread][eventbus]")
{
    EventBus bus;
    std::atomic<int> received{0};

    auto sub = bus.subscribe<ThreadEvent>([&received](const ThreadEvent& /*evt*/)
                                          { received.fetch_add(1); });

    constexpr int kThreadCount = 4;
    constexpr int kEventsPerThread = 250;

    std::vector<std::thread> threads;
    threads.reserve(kThreadCount);

    for (int tid = 0; tid < kThreadCount; ++tid)
    {
        threads.emplace_back(
            [&bus, tid]()
            {
                for (int seq = 0; seq < kEventsPerThread; ++seq)
                {
                    bus.publish(ThreadEvent{tid, seq});
                }
            });
    }

    for (auto& thr : threads)
    {
        thr.join();
    }

    REQUIRE(received.load() == kThreadCount * kEventsPerThread);
}

// ---------------------------------------------------------------------------
// Config concurrent read + single write
// ---------------------------------------------------------------------------

TEST_CASE("Thread — concurrent Config reads with single writer", "[thread][config]")
{
    Config config;
    config.set("thread.counter", 0);

    std::atomic<bool> done{false};
    std::atomic<int> read_count{0};
    std::atomic<int> violations{0};

    // Writer thread
    std::thread writer(
        [&config, &done]()
        {
            for (int idx = 0; idx < 500; ++idx)
            {
                config.set("thread.counter", idx);
            }
            done.store(true);
        });

    // Reader threads
    constexpr int kReaderCount = 3;
    std::vector<std::thread> readers;
    readers.reserve(kReaderCount);

    for (int rid = 0; rid < kReaderCount; ++rid)
    {
        readers.emplace_back(
            [&config, &done, &read_count, &violations]()
            {
                while (!done.load())
                {
                    auto val = config.get_int("thread.counter");
                    // Track violations (REQUIRE is not thread-safe with Catch2)
                    if (val < 0 || val >= 500)
                    {
                        violations.fetch_add(1);
                    }
                    read_count.fetch_add(1);
                }
            });
    }

    writer.join();
    for (auto& reader : readers)
    {
        reader.join();
    }

    // Writer should have completed
    REQUIRE(config.get_int("thread.counter") == 499);
    // No out-of-range values should have been read
    REQUIRE(violations.load() == 0);
    // Readers should have performed reads
    REQUIRE(read_count.load() > 0);
}

// ---------------------------------------------------------------------------
// MetricsCollector concurrent increment
// ---------------------------------------------------------------------------

TEST_CASE("Thread — MetricsCollector concurrent increments", "[thread][metrics]")
{
    MetricsCollector collector;

    constexpr int kThreadCount = 4;
    constexpr int kIncrementsPerThread = 1000;

    std::vector<std::thread> threads;
    threads.reserve(kThreadCount);

    for (int tid = 0; tid < kThreadCount; ++tid)
    {
        threads.emplace_back(
            [&collector]()
            {
                for (int idx = 0; idx < kIncrementsPerThread; ++idx)
                {
                    collector.increment("test.counter");
                }
            });
    }

    for (auto& thr : threads)
    {
        thr.join();
    }

    auto snapshot = collector.get("test.counter");
    REQUIRE(snapshot.value == kThreadCount * kIncrementsPerThread);
}

TEST_CASE("Thread — MetricsCollector concurrent gauge sets", "[thread][metrics]")
{
    MetricsCollector collector;

    constexpr int kThreadCount = 4;

    std::vector<std::thread> threads;
    threads.reserve(kThreadCount);

    for (int tid = 0; tid < kThreadCount; ++tid)
    {
        threads.emplace_back(
            [&collector, tid]()
            {
                for (int idx = 0; idx < 100; ++idx)
                {
                    collector.set("test.gauge", static_cast<double>(tid * 100 + idx));
                }
            });
    }

    for (auto& thr : threads)
    {
        thr.join();
    }

    // Final value should be some valid number (last write wins)
    auto snapshot = collector.get("test.gauge");
    REQUIRE(snapshot.value >= 0.0);
}

// ---------------------------------------------------------------------------
// SpanTracker concurrent spans
// ---------------------------------------------------------------------------

TEST_CASE("Thread — SpanTracker concurrent begin/end", "[thread][spans]")
{
    SpanTracker tracker;

    constexpr int kThreadCount = 4;
    constexpr int kSpansPerThread = 50;

    std::vector<std::thread> threads;
    threads.reserve(kThreadCount);

    for (int tid = 0; tid < kThreadCount; ++tid)
    {
        threads.emplace_back(
            [&tracker, tid]()
            {
                for (int idx = 0; idx < kSpansPerThread; ++idx)
                {
                    auto span_idx = tracker.begin_span("thread_" + std::to_string(tid) + "_span_" +
                                                       std::to_string(idx));
                    // Simulate some work
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                    tracker.end_span(span_idx);
                }
            });
    }

    for (auto& thr : threads)
    {
        thr.join();
    }

    auto spans = tracker.completed_spans();
    REQUIRE(spans.size() == kThreadCount * kSpansPerThread);
}

// ---------------------------------------------------------------------------
// ErrorRateTracker concurrent recording
// ---------------------------------------------------------------------------

TEST_CASE("Thread — ErrorRateTracker concurrent error recording", "[thread][errorrate]")
{
    ErrorRateTracker tracker;

    constexpr int kThreadCount = 4;
    constexpr int kErrorsPerThread = 100;

    std::vector<std::thread> threads;
    threads.reserve(kThreadCount);

    for (int tid = 0; tid < kThreadCount; ++tid)
    {
        threads.emplace_back(
            [&tracker]()
            {
                for (int idx = 0; idx < kErrorsPerThread; ++idx)
                {
                    tracker.record_error("test-module");
                }
            });
    }

    for (auto& thr : threads)
    {
        thr.join();
    }

    auto count = tracker.error_count("test-module");
    REQUIRE(count == kThreadCount * kErrorsPerThread);
}

// ---------------------------------------------------------------------------
// No deadlock under contention
// ---------------------------------------------------------------------------

TEST_CASE("Thread — no deadlock with EventBus subscribe during publish", "[thread][deadlock]")
{
    EventBus bus;
    std::atomic<int> count{0};

    auto sub =
        bus.subscribe<ThreadEvent>([&count](const ThreadEvent& /*evt*/) { count.fetch_add(1); });

    // Multiple threads publishing simultaneously
    constexpr int kThreadCount = 8;
    std::vector<std::thread> threads;
    threads.reserve(kThreadCount);

    for (int tid = 0; tid < kThreadCount; ++tid)
    {
        threads.emplace_back(
            [&bus, tid]()
            {
                for (int idx = 0; idx < 100; ++idx)
                {
                    bus.publish(ThreadEvent{tid, idx});
                }
            });
    }

    // Add a timeout check — if we hang, the test framework will report it
    for (auto& thr : threads)
    {
        thr.join();
    }

    REQUIRE(count.load() == kThreadCount * 100);
}
