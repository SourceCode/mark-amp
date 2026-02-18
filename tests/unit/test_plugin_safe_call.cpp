/// test_plugin_safe_call.cpp — Unit tests

#include "core/PluginSafeCall.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("PluginCallStatus: enum values", "[plugin_safe_call]")
{
    REQUIRE(static_cast<uint8_t>(PluginCallStatus::Success) != static_cast<uint8_t>(PluginCallStatus::Timeout));
}

TEST_CASE("PluginCallResult: success", "[plugin_safe_call]")
{
    PluginCallResult<int> result;
    result.status = PluginCallStatus::Success;
    result.value = 42;
    REQUIRE(result.value == 42);
}

TEST_CASE("PluginCallResult<void>: success", "[plugin_safe_call]")
{
    PluginCallResult<void> result;
    result.status = PluginCallStatus::Success;
    REQUIRE(result.status == PluginCallStatus::Success);
}
