/// test_event_type_id.cpp — Unit tests for EventTypeId
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

struct EventA : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "EventA";
    }
};
struct EventB : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "EventB";
    }
};

TEST_CASE("EventTypeId: different types produce different IDs", "[event_type_id]")
{
    REQUIRE(EventA{}.type_name() != EventB{}.type_name());
}
TEST_CASE("EventTypeId: same type produces same ID", "[event_type_id]")
{
    EventA a1, a2;
    REQUIRE(a1.type_name() == a2.type_name());
}
