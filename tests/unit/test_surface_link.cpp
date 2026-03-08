/// test_surface_link.cpp
#include "core/SurfaceLink.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("SurfaceLink: type compiles", "[surface_link]")
{
    static_assert(sizeof(SurfaceLink) > 0);
}
