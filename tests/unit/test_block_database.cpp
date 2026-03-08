/// test_block_database.cpp — Unit tests for BlockDatabase
#include "core/BlockDatabase.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("BlockDatabase: type compiles", "[block_database]")
{
    static_assert(sizeof(BlockDatabase) > 0);
}
TEST_CASE("DBOperation: enum values", "[block_database]")
{
    REQUIRE(DBOperation::UpsertBlock != DBOperation::DeleteBlock);
    REQUIRE(DBOperation::UpsertRef != DBOperation::DeleteRef);
}
