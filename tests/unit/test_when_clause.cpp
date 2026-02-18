/// test_when_clause.cpp — Unit tests
#include "core/WhenClause.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("WhenClauseParser: compiles", "[when_clause]")
{
    static_assert(sizeof(WhenClauseParser) > 0);
}

TEST_CASE("WhenClauseEvaluator: compiles", "[when_clause]")
{
    static_assert(sizeof(WhenClauseEvaluator) > 0);
}

TEST_CASE("WhenClauseNode: compiles", "[when_clause]")
{
    static_assert(sizeof(WhenClauseNode) > 0);
}
