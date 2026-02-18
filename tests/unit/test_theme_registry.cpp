/// test_theme_registry.cpp — Unit tests

#include "core/ThemeRegistry.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ThemeRegistry: default construction", "[theme_registry]")
{
    ThemeRegistry reg;
    REQUIRE(reg.theme_count() == 0);
}
