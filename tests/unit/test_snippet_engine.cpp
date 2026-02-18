/// test_snippet_engine.cpp — Unit tests

#include "core/SnippetEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("Snippet: default values", "[snippet_engine]")
{
    Snippet sn;
    REQUIRE(sn.prefix.empty());
    REQUIRE(sn.body.empty());
}

TEST_CASE("SnippetTabStop: default", "[snippet_engine]")
{
    SnippetTabStop stop;
    REQUIRE(stop.index == 0);
}

TEST_CASE("SnippetEngine: default construction", "[snippet_engine]")
{
    SnippetEngine engine;
    (void)engine;
}
