// test_event_bus.cpp — 10 tests for EventBus
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// Test event types
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
    std::string msg;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "OtherEvent";
    }
};

TEST_CASE("EventBus publish delivers to subscriber", "[event_bus]")
{
    EventBus bus;
    int received = 0;
    auto sub = bus.subscribe<TestEvent>([&received](const TestEvent& e) { received = e.value; });
    TestEvent evt;
    evt.value = 42;
    bus.publish(evt);
    CHECK(received == 42);
}

TEST_CASE("EventBus multiple subscribers all receive", "[event_bus]")
{
    EventBus bus;
    int count = 0;
    auto s1 = bus.subscribe<TestEvent>([&count](const TestEvent&) { ++count; });
    auto s2 = bus.subscribe<TestEvent>([&count](const TestEvent&) { ++count; });
    auto s3 = bus.subscribe<TestEvent>([&count](const TestEvent&) { ++count; });
    bus.publish(TestEvent{});
    CHECK(count == 3);
}

TEST_CASE("EventBus subscription RAII unsubscribes on destroy", "[event_bus]")
{
    EventBus bus;
    int count = 0;
    {
        auto sub = bus.subscribe<TestEvent>([&count](const TestEvent&) { ++count; });
        bus.publish(TestEvent{});
        CHECK(count == 1);
    } // sub destroyed here
    bus.publish(TestEvent{});
    CHECK(count == 1); // no increment after unsubscribe
}

TEST_CASE("EventBus type-safe: TestEvent does not trigger OtherEvent handler", "[event_bus]")
{
    EventBus bus;
    int other_count = 0;
    auto sub = bus.subscribe<OtherEvent>([&other_count](const OtherEvent&) { ++other_count; });
    bus.publish(TestEvent{});
    CHECK(other_count == 0);
}

TEST_CASE("EventBus stats track publish count", "[event_bus]")
{
    EventBus bus;
    auto sub = bus.subscribe<TestEvent>([](const TestEvent&) {});
    bus.publish(TestEvent{});
    bus.publish(TestEvent{});
    auto stats = bus.stats();
    CHECK(stats.publish_count == 2);
}

TEST_CASE("EventBus queue and process_queued", "[event_bus]")
{
    EventBus bus;
    int received = 0;
    auto sub = bus.subscribe<TestEvent>([&received](const TestEvent& e) { received = e.value; });
    TestEvent evt;
    evt.value = 99;
    bus.queue(evt);
    CHECK(received == 0); // not yet delivered
    bus.process_queued();
    CHECK(received == 99);
}

TEST_CASE("EventBus has_pending reflects queued state", "[event_bus]")
{
    EventBus bus;
    auto sub = bus.subscribe<TestEvent>([](const TestEvent&) {});
    CHECK_FALSE(bus.has_pending());
    bus.queue(TestEvent{});
    CHECK(bus.has_pending());
    bus.process_queued();
}

TEST_CASE("EventBus publish_fast delivers immediately", "[event_bus]")
{
    EventBus bus;
    int received = 0;
    auto sub = bus.subscribe<TestEvent>([&received](const TestEvent& e) { received = e.value; });
    TestEvent evt;
    evt.value = 77;
    bus.publish_fast(evt);
    CHECK(received == 77);
}

TEST_CASE("EventBus Subscription cancel works", "[event_bus]")
{
    EventBus bus;
    int count = 0;
    auto sub = bus.subscribe<TestEvent>([&count](const TestEvent&) { ++count; });
    bus.publish(TestEvent{});
    CHECK(count == 1);
    sub.cancel();
    bus.publish(TestEvent{});
    CHECK(count == 1);
}

TEST_CASE("EventBus stats active_subscriptions tracks correctly", "[event_bus]")
{
    EventBus bus;
    auto stats0 = bus.stats();
    CHECK(stats0.active_subscriptions == 0);
    auto sub = bus.subscribe<TestEvent>([](const TestEvent&) {});
    auto stats1 = bus.stats();
    CHECK(stats1.active_subscriptions == 1);
}
