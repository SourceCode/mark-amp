/// test_ial_parser.cpp — Unit tests
#include "core/IALParser.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("IALParser: compiles", "[ial_parser]")
{
    static_assert(sizeof(IALParser) > 0);
}

TEST_CASE("IALDiff: compiles", "[ial_parser]")
{
    static_assert(sizeof(IALDiff) > 0);
}
