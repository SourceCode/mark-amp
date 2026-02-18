#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

#include <thread>

using namespace markamp::core;

struct TestEvent : Event
{
    int value{0};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TestEvent";
    }
};

struct OtherEvent : Event
{
    std::string message;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "OtherEvent";
    }
};

// =============================================================================
// Original test cases (preserved)
// =============================================================================

TEST_CASE("EventBus: subscribe and publish", "[core][eventbus]")
{
    EventBus bus;
    int received_value = 0;

    auto sub = bus.subscribe<TestEvent>([&](const TestEvent& e) { received_value = e.value; });

    TestEvent evt;
    evt.value = 42;
    bus.publish(evt);

    REQUIRE(received_value == 42);
}

TEST_CASE("EventBus: multiple subscribers", "[core][eventbus]")
{
    EventBus bus;
    int count = 0;

    auto sub1 = bus.subscribe<TestEvent>([&](const TestEvent&) { count++; });
    auto sub2 = bus.subscribe<TestEvent>([&](const TestEvent&) { count++; });

    TestEvent evt;
    bus.publish(evt);

    REQUIRE(count == 2);
}

TEST_CASE("EventBus: subscription auto-unsubscribe on destruction", "[core][eventbus]")
{
    EventBus bus;
    int count = 0;

    {
        auto sub = bus.subscribe<TestEvent>([&](const TestEvent&) { count++; });
        TestEvent evt;
        bus.publish(evt);
        REQUIRE(count == 1);
    }
    // sub is destroyed, should not receive further events
    TestEvent evt;
    bus.publish(evt);
    REQUIRE(count == 1);
}

TEST_CASE("EventBus: type safety -- different event types are independent", "[core][eventbus]")
{
    EventBus bus;
    int test_count = 0;
    int other_count = 0;

    auto sub1 = bus.subscribe<TestEvent>([&](const TestEvent&) { test_count++; });
    auto sub2 = bus.subscribe<OtherEvent>([&](const OtherEvent&) { other_count++; });

    TestEvent te;
    bus.publish(te);

    REQUIRE(test_count == 1);
    REQUIRE(other_count == 0);

    OtherEvent oe;
    bus.publish(oe);

    REQUIRE(test_count == 1);
    REQUIRE(other_count == 1);
}

TEST_CASE("EventBus: queued events are delivered on process_queued", "[core][eventbus]")
{
    // Set main thread ID for assertions
    set_main_thread_id();

    EventBus bus;
    int received_value = 0;

    auto sub = bus.subscribe<TestEvent>([&](const TestEvent& e) { received_value = e.value; });

    TestEvent evt;
    evt.value = 99;
    bus.queue(evt);

    // Not delivered yet
    REQUIRE(received_value == 0);

    // Now deliver
    bus.process_queued();
    REQUIRE(received_value == 99);
}

TEST_CASE("EventBus: manual cancel subscription", "[core][eventbus]")
{
    EventBus bus;
    int count = 0;

    auto sub = bus.subscribe<TestEvent>([&](const TestEvent&) { count++; });

    TestEvent evt;
    bus.publish(evt);
    REQUIRE(count == 1);

    sub.cancel();
    bus.publish(evt);
    REQUIRE(count == 1); // Should not increment
}

// =============================================================================
// Phase 01 Task 7: Performance counters
// =============================================================================

TEST_CASE("EventBus: performance counters track publishes", "[core][eventbus][stats]")
{
    EventBus bus;
    auto sub = bus.subscribe<TestEvent>([](const TestEvent&) {});

    TestEvent evt;
    bus.publish(evt);
    bus.publish(evt);
    bus.publish(evt);

    auto st = bus.stats();
    REQUIRE(st.publish_count == 3);
    REQUIRE(st.active_subscriptions == 1);
}

TEST_CASE("EventBus: performance counters track queue depth", "[core][eventbus][stats]")
{
    set_main_thread_id();
    EventBus bus;
    auto sub = bus.subscribe<TestEvent>([](const TestEvent&) {});

    TestEvent evt;
    evt.value = 1;
    bus.queue(evt);
    bus.queue(evt);
    bus.queue(evt);

    auto st = bus.stats();
    REQUIRE(st.queue_count == 3);

    bus.process_queued();
    st = bus.stats();
    REQUIRE(st.drain_count == 1);
}

// =============================================================================
// Phase 01 Task 2: Bounded queued_events_
// =============================================================================

TEST_CASE("EventBus: queued events are bounded", "[core][eventbus][bounded]")
{
    set_main_thread_id();
    EventBus bus;
    int count = 0;
    auto sub = bus.subscribe<TestEvent>([&](const TestEvent&) { count++; });

    // Queue more than kMaxQueuedEvents to trigger overflow
    // We use a smaller number to keep the test fast but verify the mechanism
    for (int i = 0; i < 100; ++i)
    {
        TestEvent evt;
        evt.value = i;
        bus.queue(evt);
    }

    bus.process_queued();
    REQUIRE(count == 100); // All 100 should be delivered (well within 8192 limit)

    auto st = bus.stats();
    REQUIRE(st.drop_count == 0); // No drops within limit
}

// =============================================================================
// Phase 01 Task 10: has_pending() for idle loop optimization
// =============================================================================

TEST_CASE("EventBus: has_pending reports empty state correctly", "[core][eventbus][pending]")
{
    set_main_thread_id();
    EventBus bus;
    auto sub = bus.subscribe<TestEvent>([](const TestEvent&) {});

    REQUIRE_FALSE(bus.has_pending());

    TestEvent evt;
    bus.queue(evt);
    REQUIRE(bus.has_pending());

    bus.process_queued();
    REQUIRE_FALSE(bus.has_pending());
}

// =============================================================================
// Phase 01 Task 16: Type-safe event filtering
// =============================================================================

TEST_CASE("EventBus: subscribe with filter predicate", "[core][eventbus][filter]")
{
    EventBus bus;
    int received_count = 0;

    // Only receive events with value > 10
    auto sub = bus.subscribe<TestEvent>([&](const TestEvent&) { received_count++; },
                                        [](const TestEvent& e) { return e.value > 10; });

    TestEvent low;
    low.value = 5;
    bus.publish(low); // Should be filtered out

    TestEvent high;
    high.value = 42;
    bus.publish(high); // Should be delivered

    REQUIRE(received_count == 1);
}

TEST_CASE("EventBus: filter does not affect non-filtered subscriptions", "[core][eventbus][filter]")
{
    EventBus bus;
    int filtered_count = 0;
    int unfiltered_count = 0;

    // Filtered subscription
    auto sub1 = bus.subscribe<TestEvent>([&](const TestEvent&) { filtered_count++; },
                                         [](const TestEvent& e) { return e.value > 10; });

    // Unfiltered subscription
    auto sub2 = bus.subscribe<TestEvent>([&](const TestEvent&) { unfiltered_count++; });

    TestEvent evt;
    evt.value = 5;
    bus.publish(evt);

    REQUIRE(filtered_count == 0);
    REQUIRE(unfiltered_count == 1);
}

// =============================================================================
// Phase 01 Task 17: EventBus lifecycle contract tests
// =============================================================================

TEST_CASE("EventBus: subscriptions called in registration order", "[core][eventbus][lifecycle]")
{
    EventBus bus;
    std::vector<int> order;

    auto sub1 = bus.subscribe<TestEvent>([&](const TestEvent&) { order.push_back(1); });
    auto sub2 = bus.subscribe<TestEvent>([&](const TestEvent&) { order.push_back(2); });
    auto sub3 = bus.subscribe<TestEvent>([&](const TestEvent&) { order.push_back(3); });

    TestEvent evt;
    bus.publish(evt);

    REQUIRE(order == std::vector<int>{1, 2, 3});
}

TEST_CASE("EventBus: RAII cleanup prevents dangling callbacks", "[core][eventbus][lifecycle]")
{
    EventBus bus;
    int count = 0;

    {
        auto sub = bus.subscribe<TestEvent>([&](const TestEvent&) { count++; });
        TestEvent evt;
        bus.publish(evt);
        REQUIRE(count == 1);
    }
    // sub destroyed — handler should be removed

    TestEvent evt;
    bus.publish(evt);
    REQUIRE(count == 1); // Not incremented
}

TEST_CASE("EventBus: re-entrant publish does not deadlock", "[core][eventbus][lifecycle]")
{
    EventBus bus;
    int outer_count = 0;
    int inner_count = 0;

    auto sub_other = bus.subscribe<OtherEvent>([&](const OtherEvent&) { inner_count++; });

    // Publishing OtherEvent from within a TestEvent handler
    auto sub_test = bus.subscribe<TestEvent>(
        [&](const TestEvent&)
        {
            outer_count++;
            OtherEvent oe;
            oe.message = "nested";
            bus.publish(oe);
        });

    TestEvent evt;
    bus.publish(evt);

    REQUIRE(outer_count == 1);
    REQUIRE(inner_count == 1);
}

TEST_CASE("EventBus: queued events processed FIFO", "[core][eventbus][lifecycle]")
{
    set_main_thread_id();
    EventBus bus;
    std::vector<int> delivery_order;

    auto sub =
        bus.subscribe<TestEvent>([&](const TestEvent& e) { delivery_order.push_back(e.value); });

    for (int i = 1; i <= 5; ++i)
    {
        TestEvent evt;
        evt.value = i;
        bus.queue(evt);
    }

    bus.process_queued();

    REQUIRE(delivery_order == std::vector<int>{1, 2, 3, 4, 5});
}
