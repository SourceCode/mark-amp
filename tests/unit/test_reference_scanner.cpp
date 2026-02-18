/// test_reference_scanner.cpp — Unit tests
#include "core/ReferenceScanner.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("ReferenceScanner: compiles", "[reference_scanner]")
{
    static_assert(sizeof(ReferenceScanner) > 0);
}
