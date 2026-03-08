/// test_async_highlighter.cpp — Unit tests for AsyncHighlighter
#include "core/AsyncHighlighter.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("LineState: default values", "[async_highlighter]")
{
    LineState ls;
    REQUIRE(ls.state_hash == 0);
    REQUIRE(ls.version == 0);
}
TEST_CASE("HighlightResult: default values", "[async_highlighter]")
{
    HighlightResult hr;
    REQUIRE(hr.version == 0);
}
TEST_CASE("AsyncHighlighter: type compiles", "[async_highlighter]")
{
    static_assert(sizeof(AsyncHighlighter) > 0);
}
