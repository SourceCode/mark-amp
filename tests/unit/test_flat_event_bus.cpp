// test_flat_event_bus.cpp — 10 tests for FlatEventBus
#include "core/FlatEventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct FlatTestEvent : Event
{
    int value{0};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FlatTestEvent";
    }
};

TEST_CASE("FlatEventBus starts with zero subscribers", "[flat_event_bus]")
{
    FlatEventBus bus;
    CHECK(bus.subscriber_count(EventTypeId::ThemeChanged) == 0);
    CHECK(bus.total_subscriber_count() == 0);
}

TEST_CASE("FlatEventBus subscribe increments count", "[flat_event_bus]")
{
    FlatEventBus bus;
    auto sub = bus.subscribe(EventTypeId::ThemeChanged, [](const Event&) {});
    CHECK(bus.subscriber_count(EventTypeId::ThemeChanged) == 1);
}

TEST_CASE("FlatEventBus publish delivers to subscriber", "[flat_event_bus]")
{
    FlatEventBus bus;
    int received = 0;
    auto sub = bus.subscribe(EventTypeId::ThemeChanged, [&received](const Event&) { ++received; });
    FlatTestEvent evt;
    bus.publish(EventTypeId::ThemeChanged, evt);
    CHECK(received == 1);
}

TEST_CASE("FlatEventBus subscription auto-unsubscribes on destroy", "[flat_event_bus]")
{
    FlatEventBus bus;
    {
        auto sub = bus.subscribe(EventTypeId::FileSaved, [](const Event&) {});
        CHECK(bus.subscriber_count(EventTypeId::FileSaved) == 1);
    }
    CHECK(bus.subscriber_count(EventTypeId::FileSaved) == 0);
}

TEST_CASE("FlatEventBus different EventTypeIds are independent", "[flat_event_bus]")
{
    FlatEventBus bus;
    int theme_count = 0, file_count = 0;
    auto s1 =
        bus.subscribe(EventTypeId::ThemeChanged, [&theme_count](const Event&) { ++theme_count; });
    auto s2 = bus.subscribe(EventTypeId::FileSaved, [&file_count](const Event&) { ++file_count; });
    FlatTestEvent evt;
    bus.publish(EventTypeId::ThemeChanged, evt);
    CHECK(theme_count == 1);
    CHECK(file_count == 0);
}

TEST_CASE("FlatEventBus multiple subscribers on same type", "[flat_event_bus]")
{
    FlatEventBus bus;
    int count = 0;
    auto s1 = bus.subscribe(EventTypeId::AppReady, [&count](const Event&) { ++count; });
    auto s2 = bus.subscribe(EventTypeId::AppReady, [&count](const Event&) { ++count; });
    FlatTestEvent evt;
    bus.publish(EventTypeId::AppReady, evt);
    CHECK(count == 2);
}

TEST_CASE("FlatEventBus total_subscriber_count aggregates", "[flat_event_bus]")
{
    FlatEventBus bus;
    auto s1 = bus.subscribe(EventTypeId::ThemeChanged, [](const Event&) {});
    auto s2 = bus.subscribe(EventTypeId::FileSaved, [](const Event&) {});
    auto s3 = bus.subscribe(EventTypeId::AppReady, [](const Event&) {});
    CHECK(bus.total_subscriber_count() == 3);
}

TEST_CASE("FlatEventBus publish to empty type is safe", "[flat_event_bus]")
{
    FlatEventBus bus;
    FlatTestEvent evt;
    REQUIRE_NOTHROW(bus.publish(EventTypeId::ThemeChanged, evt));
}

TEST_CASE("FlatEventBus move subscription transfers ownership", "[flat_event_bus]")
{
    FlatEventBus bus;
    auto sub1 = bus.subscribe(EventTypeId::ThemeChanged, [](const Event&) {});
    CHECK(bus.subscriber_count(EventTypeId::ThemeChanged) == 1);
    auto sub2 = std::move(sub1);
    CHECK(bus.subscriber_count(EventTypeId::ThemeChanged) == 1);
}

TEST_CASE("FlatEventBus subscriber_count returns 0 for unused types", "[flat_event_bus]")
{
    FlatEventBus bus;
    CHECK(bus.subscriber_count(EventTypeId::CanvasViewportChanged) == 0);
    CHECK(bus.subscriber_count(EventTypeId::KernelStarted) == 0);
}
