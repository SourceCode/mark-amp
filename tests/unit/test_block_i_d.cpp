/// test_block_i_d.cpp — Unit tests for BlockId (duplicate stub)
#include "core/BlockRef.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("BlockId: default value is empty string", "[block_id]")
{
    BlockId id;
    REQUIRE(id.value.empty());
    REQUIRE_FALSE(id.is_valid());
}

TEST_CASE("BlockId: valid format passes validation", "[block_id]")
{
    BlockId id{"20240315143022-abc1234"};
    REQUIRE(id.is_valid());
}

TEST_CASE("BlockId: string comparison works", "[block_id]")
{
    BlockId a{"20240101000000-abcdefg"};
    BlockId b{"20240101000000-abcdefg"};
    REQUIRE(a == b);
}

TEST_CASE("BlockId: ordering uses string comparison", "[block_id]")
{
    BlockId a{"20240101000000-aaaaaaa"};
    BlockId b{"20240201000000-aaaaaaa"};
    REQUIRE(a < b);
}

TEST_CASE("BlockId: generate produces valid unique IDs", "[block_id]")
{
    auto id1 = BlockId::generate();
    auto id2 = BlockId::generate();
    REQUIRE(id1.is_valid());
    REQUIRE(id2.is_valid());
    REQUIRE_FALSE(id1 == id2);
}

TEST_CASE("BlockIdHash: deterministic for same input", "[block_id]")
{
    BlockIdHash hasher;
    BlockId id{"20240101120000-abcdefg"};
    REQUIRE(hasher(id) == hasher(id));
}
