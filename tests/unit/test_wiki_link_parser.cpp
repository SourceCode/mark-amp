/// test_wiki_link_parser.cpp — Unit tests
#include "core/WikiLinkParser.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("WikiLinkParser: compiles", "[wiki_link_parser]")
{
    static_assert(sizeof(WikiLinkParser) > 0);
}
