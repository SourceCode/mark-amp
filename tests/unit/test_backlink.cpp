/// test_backlink.cpp — Unit tests
#include "core/Backlink.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("BacklinkType: compiles", "[backlink]")
{
    static_assert(sizeof(BacklinkType) > 0);
}
