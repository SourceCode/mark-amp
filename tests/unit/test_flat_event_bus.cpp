/// test_flat_event_bus.cpp — Unit tests for FlatEventBus
#include "core/FlatEventBus.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("FlatEventBus: type compiles", "[flat_event_bus]")
{
    static_assert(sizeof(FlatEventBus) > 0);
}
