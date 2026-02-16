/// test_eventbus_v2.cpp — Phase 20: FlatEventBus tests
///
/// Validates EventTypeId-based subscribe/publish, handler ordering,
/// RAII subscription cleanup, and multi-subscriber behavior.

#include "core/EventTypeId.h"
#include "core/FlatEventBus.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::core;

// Helper event for testing
namespace
{
struct TestEvent : Event
{
    std::string payload;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TestEvent";
    }
};
} // namespace

// ══════════════════════════════════════════
// EventTypeId Tests
// ══════════════════════════════════════════

TEST_CASE("EventTypeId: name lookup returns correct name", "[flat_eventbus]")
{
    REQUIRE(event_type_name(EventTypeId::ThemeChanged) == "ThemeChanged");
    REQUIRE(event_type_name(EventTypeId::EditorContentChanged) == "EditorContentChanged");
    REQUIRE(event_type_name(EventTypeId::AppReady) == "AppReady");
}

TEST_CASE("EventTypeId: unknown ID returns Unknown", "[flat_eventbus]")
{
    REQUIRE(event_type_name(static_cast<EventTypeId>(999)) == "Unknown");
}

// ══════════════════════════════════════════
// FlatEventBus Subscribe/Publish
// ══════════════════════════════════════════

TEST_CASE("FlatEventBus: subscribe and publish delivers event", "[flat_eventbus]")
{
    FlatEventBus bus;
    bool received = false;

    auto sub =
        bus.subscribe(EventTypeId::ThemeChanged, [&](const Event& /*evt*/) { received = true; });

    TestEvent event;
    event.payload = "midnight-neon";
    bus.publish(EventTypeId::ThemeChanged, event);

    REQUIRE(received);
}

TEST_CASE("FlatEventBus: publish with no subscribers is safe", "[flat_eventbus]")
{
    FlatEventBus bus;
    TestEvent event;
    // Should not crash
    bus.publish(EventTypeId::FileSaved, event);
    REQUIRE(bus.subscriber_count(EventTypeId::FileSaved) == 0);
}

TEST_CASE("FlatEventBus: multiple subscribers all receive event", "[flat_eventbus]")
{
    FlatEventBus bus;
    int count = 0;

    auto sub1 = bus.subscribe(EventTypeId::FileOpened, [&](const Event& /*evt*/) { ++count; });
    auto sub2 = bus.subscribe(EventTypeId::FileOpened, [&](const Event& /*evt*/) { ++count; });
    auto sub3 = bus.subscribe(EventTypeId::FileOpened, [&](const Event& /*evt*/) { ++count; });

    TestEvent event;
    bus.publish(EventTypeId::FileOpened, event);

    REQUIRE(count == 3);
    REQUIRE(bus.subscriber_count(EventTypeId::FileOpened) == 3);
}

TEST_CASE("FlatEventBus: handlers execute in subscription order", "[flat_eventbus]")
{
    FlatEventBus bus;
    std::vector<int> order;

    auto sub1 =
        bus.subscribe(EventTypeId::AppReady, [&](const Event& /*evt*/) { order.push_back(1); });
    auto sub2 =
        bus.subscribe(EventTypeId::AppReady, [&](const Event& /*evt*/) { order.push_back(2); });
    auto sub3 =
        bus.subscribe(EventTypeId::AppReady, [&](const Event& /*evt*/) { order.push_back(3); });

    TestEvent event;
    bus.publish(EventTypeId::AppReady, event);

    REQUIRE(order == std::vector<int>{1, 2, 3});
}

TEST_CASE("FlatEventBus: different event types are independent", "[flat_eventbus]")
{
    FlatEventBus bus;
    bool theme_received = false;
    bool file_received = false;

    auto sub1 = bus.subscribe(EventTypeId::ThemeChanged,
                              [&](const Event& /*evt*/) { theme_received = true; });
    auto sub2 =
        bus.subscribe(EventTypeId::FileOpened, [&](const Event& /*evt*/) { file_received = true; });

    TestEvent event;
    bus.publish(EventTypeId::ThemeChanged, event);

    REQUIRE(theme_received);
    REQUIRE_FALSE(file_received);
}

// ══════════════════════════════════════════
// Subscription Lifecycle
// ══════════════════════════════════════════

TEST_CASE("FlatEventBus: subscription RAII cleanup", "[flat_eventbus]")
{
    FlatEventBus bus;

    {
        auto sub = bus.subscribe(EventTypeId::SettingChanged, [](const Event& /*evt*/) {});
        REQUIRE(bus.subscriber_count(EventTypeId::SettingChanged) == 1);
    }
    // sub destroyed — should have unsubscribed
    REQUIRE(bus.subscriber_count(EventTypeId::SettingChanged) == 0);
}

TEST_CASE("FlatEventBus: move subscription transfers ownership", "[flat_eventbus]")
{
    FlatEventBus bus;

    auto sub1 = bus.subscribe(EventTypeId::Notification, [](const Event& /*evt*/) {});
    REQUIRE(bus.subscriber_count(EventTypeId::Notification) == 1);

    auto sub2 = std::move(sub1);
    REQUIRE(bus.subscriber_count(EventTypeId::Notification) == 1);
}

TEST_CASE("FlatEventBus: total_subscriber_count sums all slots", "[flat_eventbus]")
{
    FlatEventBus bus;

    auto sub1 = bus.subscribe(EventTypeId::ThemeChanged, [](const Event& /*evt*/) {});
    auto sub2 = bus.subscribe(EventTypeId::FileOpened, [](const Event& /*evt*/) {});
    auto sub3 = bus.subscribe(EventTypeId::FileOpened, [](const Event& /*evt*/) {});

    REQUIRE(bus.total_subscriber_count() == 3);
}

TEST_CASE("FlatEventBus: subscriber_count for invalid ID returns 0", "[flat_eventbus]")
{
    FlatEventBus bus;
    REQUIRE(bus.subscriber_count(static_cast<EventTypeId>(9999)) == 0);
}
