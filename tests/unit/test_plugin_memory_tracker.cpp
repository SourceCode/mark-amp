/// test_plugin_memory_tracker.cpp — Unit tests
#include "core/PluginMemoryTracker.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("PluginMemoryTracker: compiles", "[plugin_memory_tracker]")
{
    static_assert(sizeof(PluginMemoryTracker) > 0);
}
