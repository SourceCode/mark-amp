/// test_query_service.cpp
#include "core/QueryService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("QueryService: type compiles", "[query_service]")
{
    static_assert(sizeof(QueryService) > 0);
}
