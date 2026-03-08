/// test_grouping_service.cpp
#include "canvas/GroupingService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("GroupingService: type compiles", "[grouping_service]")
{
    static_assert(sizeof(GroupingService) > 0);
}
