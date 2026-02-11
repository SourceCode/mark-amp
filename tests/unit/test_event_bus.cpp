#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

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
