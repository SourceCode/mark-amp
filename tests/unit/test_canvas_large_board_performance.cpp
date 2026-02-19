// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/LargeBoardModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Spatial culling counts", "[largeboard][culling]")
{
    LargeBoardModel model;
    model.set_total_objects(10000);
    model.set_visible_count(500);
    REQUIRE(model.visible_count() == 500);
    REQUIRE(model.culled_count() == 9500);
}

TEST_CASE("LOD levels by zoom", "[largeboard][lod]")
{
    LargeBoardModel model;
    REQUIRE(model.lod_for_zoom(1.0) == LodLevel::kFull);
    REQUIRE(model.lod_for_zoom(0.5) == LodLevel::kFull);
    REQUIRE(model.lod_for_zoom(0.3) == LodLevel::kSimplified);
    REQUIRE(model.lod_for_zoom(0.1) == LodLevel::kBoundingBox);
}

TEST_CASE("Tile cache statistics", "[largeboard][cache]")
{
    LargeBoardModel model;
    model.record_cache_hit();
    model.record_cache_hit();
    model.record_cache_hit();
    model.record_cache_miss();
    REQUIRE(model.cache_hits() == 3);
    REQUIRE(model.cache_misses() == 1);
    REQUIRE(model.cache_hit_rate() == 0.75);
}

TEST_CASE("Render telemetry", "[largeboard][telemetry]")
{
    LargeBoardModel model;
    model.set_telemetry({16.0, 120, 500, 9500});
    REQUIRE(model.telemetry().frame_time_ms == 16.0);
    REQUIRE(model.telemetry().draw_calls == 120);
}

TEST_CASE("Cache hit rate zero division", "[largeboard][cache]")
{
    LargeBoardModel model;
    REQUIRE(model.cache_hit_rate() == 0.0);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
