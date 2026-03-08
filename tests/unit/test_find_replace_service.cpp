/// test_find_replace_service.cpp — Unit tests
#include "core/FindReplaceService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("FindReplaceService: type compiles", "[find_replace_service]")
{
    static_assert(sizeof(FindReplaceService) > 0);
}
