/// test_diff_types.cpp — Unit tests
#include "core/DiffTypes.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("DiffType: enum values", "[diff_types]")
{
    REQUIRE(static_cast<int>(DiffType::Unchanged) != static_cast<int>(DiffType::Added));
}

TEST_CASE("DiffViewMode: enum values", "[diff_types]")
{
    REQUIRE(static_cast<int>(DiffViewMode::Inline) != static_cast<int>(DiffViewMode::SideBySide));
}

TEST_CASE("DiffType: compiles", "[diff_types]")
{
    static_assert(sizeof(DiffType) > 0);
}

TEST_CASE("DiffViewMode: compiles", "[diff_types]")
{
    static_assert(sizeof(DiffViewMode) > 0);
}
