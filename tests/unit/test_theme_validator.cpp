/// test_theme_validator.cpp
#include "core/ThemeValidator.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("ThemeValidator: type compiles", "[theme_validator]")
{
    static_assert(sizeof(ThemeValidator) > 0);
}
