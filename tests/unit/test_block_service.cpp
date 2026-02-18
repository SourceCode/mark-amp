/// test_block_service.cpp — Unit tests

#include "core/BlockService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("OperationAction enum values", "[block_service]")
{
    REQUIRE(static_cast<uint8_t>(OperationAction::Insert) != static_cast<uint8_t>(OperationAction::Delete));
}

TEST_CASE("Operation: default field values", "[block_service]")
{
    Operation op;
    REQUIRE(op.id.empty());
    REQUIRE(op.parent_id.empty());
    REQUIRE(op.data.empty());
}

TEST_CASE("Transaction: starts with empty do_operations", "[block_service]")
{
    Transaction tx;
    REQUIRE(tx.do_operations.empty());
}
