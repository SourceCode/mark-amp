/// test_backlink_service.cpp — Unit tests

#include "core/BacklinkService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("BacklinkService: compiles", "[backlink_service]")
{
    static_assert(sizeof(BacklinkService) > 0);
}
