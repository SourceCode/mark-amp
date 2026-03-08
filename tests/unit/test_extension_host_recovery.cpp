/// test_extension_host_recovery.cpp
#include "core/ExtensionHostRecovery.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("ExtensionHostRecovery: type compiles", "[extension_host_recovery]")
{
    static_assert(sizeof(ExtensionHostRecovery) > 0);
}
