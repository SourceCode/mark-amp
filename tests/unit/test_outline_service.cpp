/// test_outline_service.cpp — Unit tests

#include "core/OutlineService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("OutlineService: compiles", "[outline_service]")
{
    static_assert(sizeof(OutlineService) > 0);
}
