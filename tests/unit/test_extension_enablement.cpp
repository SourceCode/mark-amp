/// test_extension_enablement.cpp — Unit tests

#include "core/ExtensionEnablement.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ExtensionEnablementService: compiles", "[extension_enablement]")
{
    static_assert(sizeof(ExtensionEnablementService) > 0);
}
