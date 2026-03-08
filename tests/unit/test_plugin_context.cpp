/// test_plugin_context.cpp
#include "core/PluginContext.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("PluginContext: type compiles", "[plugin_context]")
{
    static_assert(sizeof(PluginContext) > 0);
}
