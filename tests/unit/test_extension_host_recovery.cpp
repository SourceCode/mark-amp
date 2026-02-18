/// test_extension_host_recovery.cpp — Unit tests

#include "core/ExtensionHostRecovery.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ExtensionError: compiles", "[extension_host_recovery]")
{
    static_assert(sizeof(ExtensionError) > 0);
}

TEST_CASE("ExtensionHostRecovery: compiles", "[extension_host_recovery]")
{
    static_assert(sizeof(ExtensionHostRecovery) > 0);
}
