/// test_text_span.cpp — Unit tests
#include <vector>
#include "core/TextSpan.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("TextSpan: compiles", "[text_span]")
{
    static_assert(sizeof(TextSpan) > 0);
}
