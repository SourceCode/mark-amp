/// test_block_tree_index.cpp — Unit tests

#include "core/BlockTreeIndex.h"

#include <catch2/catch_test_macros.hpp>

#include <vector>

using namespace markamp::core;

TEST_CASE("BlockTreeEntry: default values", "[block_tree_index]")
{
    BlockTreeEntry entry;
    REQUIRE(entry.id.empty());
    REQUIRE(entry.root_id.empty());
    REQUIRE(entry.parent_id.empty());
    REQUIRE(entry.path.empty());
}
