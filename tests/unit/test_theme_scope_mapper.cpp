/// test_theme_scope_mapper.cpp
#include "core/ThemeScopeMapper.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;
TEST_CASE("ThemeScopeMapper: type compiles", "[theme_scope_mapper]")
{
    static_assert(sizeof(ThemeScopeMapper) > 0);
}
