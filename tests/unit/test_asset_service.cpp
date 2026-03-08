/// test_asset_service.cpp — Unit tests for AssetService
#include "core/AssetService.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("AssetService: type compiles", "[asset_service]")
{
    static_assert(sizeof(AssetService) > 0);
}
TEST_CASE("AssetInfo: default values", "[asset_service]")
{
    AssetInfo info;
    REQUIRE(info.asset_id.empty());
}
