/// test_ref_parser.cpp — Unit tests
#include "core/RefParser.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("RefParser: compiles", "[ref_parser]")
{
    static_assert(sizeof(RefParser) > 0);
}
