/// test_theme_scope_mapper.cpp — Unit tests
#include "core/ThemeScopeMapper.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("FontStyleFlag: enum values", "[theme_scope_mapper]")
{
    REQUIRE(static_cast<uint8_t>(FontStyleFlag::kBold) != static_cast<uint8_t>(FontStyleFlag::kItalic));
}

TEST_CASE("ThemeScopeMapper: compiles", "[theme_scope_mapper]")
{
    static_assert(sizeof(ThemeScopeMapper) > 0);
}
