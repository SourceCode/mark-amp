/// test_event_type_id.cpp — Unit tests
#include "core/EventTypeId.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("EventTypeId: compiles", "[event_type_id]")
{
    static_assert(sizeof(EventTypeId) > 0);
}
