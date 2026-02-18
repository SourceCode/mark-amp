/// test_query_service.cpp — Unit tests

#include "core/QueryService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("QueryService: compiles", "[query_service]")
{
    static_assert(sizeof(QueryService) > 0);
}
