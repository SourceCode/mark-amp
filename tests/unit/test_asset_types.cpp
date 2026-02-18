/// test_asset_types.cpp — Unit tests
#include "core/AssetTypes.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AssetCategory: enum values", "[asset_types]")
{
    REQUIRE(static_cast<int>(AssetCategory::Image) != static_cast<int>(AssetCategory::Document));
}

TEST_CASE("AssetCategory: compiles", "[asset_types]")
{
    static_assert(sizeof(AssetCategory) > 0);
}
