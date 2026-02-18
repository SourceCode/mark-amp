/// test_content_indexer.cpp — Unit tests

#include "core/ContentIndexer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("PlainTextExtractor: compiles", "[content_indexer]")
{
    static_assert(sizeof(PlainTextExtractor) > 0);
}

TEST_CASE("MarkdownTextExtractor: compiles", "[content_indexer]")
{
    static_assert(sizeof(MarkdownTextExtractor) > 0);
}
