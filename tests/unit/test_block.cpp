/// test_block.cpp — Unit tests
#include "core/Block.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("BlockType: enum values", "[block]")
{
    REQUIRE(static_cast<int>(BlockType::Document) != static_cast<int>(BlockType::Paragraph));
}

TEST_CASE("BlockSubType: enum values", "[block]")
{
    REQUIRE(static_cast<int>(BlockSubType::None) != static_cast<int>(BlockSubType::Heading1));
}

TEST_CASE("BlockType: compiles", "[block]")
{
    static_assert(sizeof(BlockType) > 0);
}

TEST_CASE("BlockSubType: compiles", "[block]")
{
    static_assert(sizeof(BlockSubType) > 0);
}
