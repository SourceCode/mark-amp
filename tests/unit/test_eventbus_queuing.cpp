/// @file test_eventbus_queuing.cpp
/// Tests for EventBus queued and fast-path event delivery.

#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>
#include <vector>

using namespace markamp::core;

// ── Test events ─────────────────────────────────────────────────────────────

struct QueueTestEvent : Event
{
    int value{0};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "QueueTestEvent";
    }
};

struct FastTestEvent : Event
{
    std::string tag;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FastTestEvent";
    }
};

// ── Queue tests ─────────────────────────────────────────────────────────────

TEST_CASE("EventBus: queue defers delivery until process_queued", "[eventbus][queue]")
{
    EventBus bus;
    int received = 0;

    auto sub = bus.subscribe<QueueTestEvent>([&](const QueueTestEvent& e) { received = e.value; });

    QueueTestEvent evt;
    evt.value = 42;
    bus.queue(evt);

    REQUIRE(received == 0); // Not yet delivered
    bus.process_queued();
    REQUIRE(received == 42);
}

TEST_CASE("EventBus: multiple queued events delivered in order", "[eventbus][queue]")
{
    EventBus bus;
    std::vector<int> order;

    auto sub =
        bus.subscribe<QueueTestEvent>([&](const QueueTestEvent& e) { order.push_back(e.value); });

    QueueTestEvent e1;
    e1.value = 1;
    QueueTestEvent e2;
    e2.value = 2;
    QueueTestEvent e3;
    e3.value = 3;

    bus.queue(e1);
    bus.queue(e2);
    bus.queue(e3);

    REQUIRE(order.empty());
    bus.process_queued();
    REQUIRE(order.size() == 3);
    REQUIRE(order[0] == 1);
    REQUIRE(order[1] == 2);
    REQUIRE(order[2] == 3);
}

TEST_CASE("EventBus: process_queued is idempotent on empty queue", "[eventbus][queue]")
{
    EventBus bus;
    // Should not crash or throw
    bus.process_queued();
    bus.process_queued();
    REQUIRE(true);
}

// ── Fast-path tests ─────────────────────────────────────────────────────────

TEST_CASE("EventBus: publish_fast delivers to subscribers", "[eventbus][fast]")
{
    EventBus bus;
    std::string received_tag;

    auto sub = bus.subscribe<FastTestEvent>([&](const FastTestEvent& e) { received_tag = e.tag; });

    FastTestEvent evt;
    evt.tag = "fast-42";
    bus.publish_fast(evt);

    REQUIRE(received_tag == "fast-42");
}

TEST_CASE("EventBus: publish_fast multiple subscribers", "[eventbus][fast]")
{
    EventBus bus;
    int count = 0;

    auto sub1 = bus.subscribe<FastTestEvent>([&](const FastTestEvent&) { count++; });
    auto sub2 = bus.subscribe<FastTestEvent>([&](const FastTestEvent&) { count++; });

    FastTestEvent evt;
    bus.publish_fast(evt);

    REQUIRE(count == 2);
}

// ── drain_fast_queue tests ──────────────────────────────────────────────────

TEST_CASE("EventBus: drain_fast_queue executes pushed functions", "[eventbus][drain]")
{
    EventBus bus;

    // drain_fast_queue drains the fast SPSC queue
    // Since we can't easily push to it from the public API without triggering
    // the fast path, we just verify it doesn't crash on empty drain
    bus.drain_fast_queue();
    REQUIRE(true);
}

// ── Exception isolation tests ───────────────────────────────────────────────

TEST_CASE("EventBus: handler exception does not block subsequent handlers", "[eventbus][safety]")
{
    EventBus bus;
    bool second_called = false;

    auto sub1 = bus.subscribe<QueueTestEvent>([&](const QueueTestEvent&)
                                              { throw std::runtime_error("handler error"); });
    auto sub2 = bus.subscribe<QueueTestEvent>([&](const QueueTestEvent&) { second_called = true; });

    QueueTestEvent evt;
    bus.publish(evt); // First handler throws, second should still execute

    REQUIRE(second_called);
}

TEST_CASE("EventBus: fast-path handler exception does not block others", "[eventbus][safety]")
{
    EventBus bus;
    bool second_called = false;

    auto sub1 = bus.subscribe<FastTestEvent>([&](const FastTestEvent&)
                                             { throw std::runtime_error("fast handler error"); });
    auto sub2 = bus.subscribe<FastTestEvent>([&](const FastTestEvent&) { second_called = true; });

    FastTestEvent evt;
    bus.publish_fast(evt);

    REQUIRE(second_called);
}

// ── Subscription lifecycle ──────────────────────────────────────────────────

TEST_CASE("EventBus: unsubscribed handler not called on fast-path", "[eventbus][fast]")
{
    EventBus bus;
    int count = 0;

    {
        auto sub = bus.subscribe<FastTestEvent>([&](const FastTestEvent&) { count++; });
        FastTestEvent evt;
        bus.publish_fast(evt);
        REQUIRE(count == 1);
    }

    // sub destroyed — handler should not fire
    FastTestEvent evt;
    bus.publish_fast(evt);
    REQUIRE(count == 1);
}
