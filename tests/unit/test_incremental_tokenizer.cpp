/// test_incremental_tokenizer.cpp — Unit tests
#include "core/IncrementalTokenizer.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("LineTokens: default construction", "[incremental_tokenizer]")
{
    LineTokens lt;
    REQUIRE(lt.tokens.empty());
}

TEST_CASE("IncrementalTokenizer: compiles", "[incremental_tokenizer]")
{
    static_assert(sizeof(IncrementalTokenizer) > 0);
}
