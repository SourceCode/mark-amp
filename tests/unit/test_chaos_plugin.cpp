/// test_chaos_plugin.cpp
#include "core/ChaosPlugin.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("ChaosPlugin: type compiles", "[chaos_plugin]")
{
    static_assert(sizeof(ChaosPlugin) > 0);
}
