/// test_plugin_manager.cpp — Unit tests

#include "core/PluginManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("PluginManager: compiles", "[plugin_manager]")
{
    static_assert(sizeof(PluginManager) > 0);
}
