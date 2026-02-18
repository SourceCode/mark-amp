/// test_block_database.cpp — Unit tests

#include "core/BlockDatabase.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("BlockDatabase: compiles", "[block_database]")
{
    static_assert(sizeof(BlockDatabase) > 0);
}
