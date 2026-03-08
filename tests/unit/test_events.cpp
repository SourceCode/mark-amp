/// test_events.cpp — Unit tests for Event types
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct TestEvent1 : Event
{
    int value{0};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TestEvent1";
    }
};

struct TestEvent2 : Event
{
    std::string msg;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TestEvent2";
    }
};

TEST_CASE("Event: type_name returns correct string", "[events]")
{
    TestEvent1 e;
    REQUIRE(e.type_name() == "TestEvent1");
}

TEST_CASE("Event: derived events are independent types", "[events]")
{
    TestEvent1 e1;
    TestEvent2 e2;
    REQUIRE(e1.type_name() != e2.type_name());
}

TEST_CASE("Event: can copy events", "[events]")
{
    TestEvent1 orig;
    orig.value = 42;
    TestEvent1 copy = orig;
    REQUIRE(copy.value == 42);
}

TEST_CASE("Event: polymorphic destruction works", "[events]")
{
    std::unique_ptr<Event> ptr = std::make_unique<TestEvent1>();
    REQUIRE(ptr->type_name() == "TestEvent1");
    // No crash on destruction
}

TEST_CASE("Subscription: default constructed is inactive", "[events]")
{
    Subscription sub;
    // Should not crash when destroyed
}

TEST_CASE("Subscription: move semantics transfer ownership", "[events]")
{
    EventBus bus;
    int count = 0;
    auto sub = bus.subscribe<TestEvent1>([&](const TestEvent1&) { count++; });
    Subscription moved = std::move(sub);
    TestEvent1 evt;
    bus.publish(evt);
    REQUIRE(count == 1);
}
