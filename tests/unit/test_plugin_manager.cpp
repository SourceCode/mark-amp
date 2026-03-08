/// test_plugin_manager.cpp
#include "core/PluginManager.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("PluginManager: type compiles", "[plugin_manager]")
{
    static_assert(sizeof(PluginManager) > 0);
}
