/// test_search_service.cpp — Unit tests

#include "core/SearchService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SearchService: compiles", "[search_service]")
{
    static_assert(sizeof(SearchService) > 0);
}
