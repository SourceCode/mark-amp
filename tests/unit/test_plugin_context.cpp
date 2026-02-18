/// test_plugin_context.cpp — Unit tests

#include "core/PluginContext.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("PluginContext: compiles", "[plugin_context]")
{
    // PluginContext requires complex wiring; verify it compiles
    static_assert(sizeof(PluginContext) > 0);
}
