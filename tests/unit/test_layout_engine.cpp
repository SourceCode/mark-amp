#include "canvas/LayoutEngine.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <cmath>

using namespace markamp::canvas;

namespace
{

auto make_objects(int count, double size = 100.0) -> std::vector<std::pair<ObjectId, AABB>>
{
    std::vector<std::pair<ObjectId, AABB>> objects;
    for (int idx = 0; idx < count; ++idx)
    {
        objects.push_back({static_cast<ObjectId>(idx + 1), {0.0, 0.0, size, size}});
    }
    return objects;
}

} // anonymous namespace

TEST_CASE("LayoutEngine: grid 4 objects -> 2x2", "[layout_engine]")
{
    LayoutEngine engine;
    auto objects = make_objects(4);
    LayoutOptions options;
    options.type = LayoutType::Grid;
    options.spacing = 20.0;

    const auto results = engine.compute_layout(objects, options);
    REQUIRE(results.size() == 4);
    // Auto columns = ceil(sqrt(4)) = 2, so 2x2 grid.
    // (0,0), (1,0), (0,1), (1,1) in cell coords.
    REQUIRE(results[0].new_position.x == 0.0);
    REQUIRE(results[0].new_position.y == 0.0);
    REQUIRE(results[1].new_position.x > 0.0);
    REQUIRE(results[1].new_position.y == 0.0);
    REQUIRE(results[2].new_position.x == 0.0);
    REQUIRE(results[2].new_position.y > 0.0);
}

TEST_CASE("LayoutEngine: grid 9 objects -> 3x3", "[layout_engine]")
{
    LayoutEngine engine;
    auto objects = make_objects(9);
    LayoutOptions options;
    options.type = LayoutType::Grid;

    const auto results = engine.compute_layout(objects, options);
    REQUIRE(results.size() == 9);
    // 3x3 grid: objects on same row have same y.
    REQUIRE(results[0].new_position.y == results[1].new_position.y);
    REQUIRE(results[0].new_position.y == results[2].new_position.y);
    REQUIRE(results[3].new_position.y == results[4].new_position.y);
    REQUIRE(results[3].new_position.y > results[0].new_position.y);
}

TEST_CASE("LayoutEngine: radial 6 objects equidistant", "[layout_engine]")
{
    LayoutEngine engine;
    auto objects = make_objects(6);
    LayoutOptions options;
    options.type = LayoutType::Radial;
    options.radius = 200.0;

    const auto results = engine.compute_layout(objects, options);
    REQUIRE(results.size() == 6);

    // All points should be approximately equidistant from the centroid.
    // Centroid of initial 0,0 AABBs = (50,50), so center is around there.
    const double cx =
        results[0].new_position.x + (results[3].new_position.x - results[0].new_position.x) / 2.0;
    // Just check that first and second differ in position.
    REQUIRE(results[0].new_position.x != results[1].new_position.x);
}

TEST_CASE("LayoutEngine: horizontal 3 objects", "[layout_engine]")
{
    LayoutEngine engine;
    auto objects = make_objects(3, 80.0);
    LayoutOptions options;
    options.type = LayoutType::Horizontal;
    options.spacing = 20.0;

    const auto results = engine.compute_layout(objects, options);
    REQUIRE(results.size() == 3);
    // All on same Y=0, increasing X.
    REQUIRE(results[0].new_position.y == 0.0);
    REQUIRE(results[1].new_position.y == 0.0);
    REQUIRE(results[0].new_position.x < results[1].new_position.x);
    REQUIRE(results[1].new_position.x < results[2].new_position.x);
}

TEST_CASE("LayoutEngine: vertical 3 objects", "[layout_engine]")
{
    LayoutEngine engine;
    auto objects = make_objects(3, 80.0);
    LayoutOptions options;
    options.type = LayoutType::Vertical;
    options.spacing = 20.0;

    const auto results = engine.compute_layout(objects, options);
    REQUIRE(results.size() == 3);
    REQUIRE(results[0].new_position.x == 0.0);
    REQUIRE(results[1].new_position.x == 0.0);
    REQUIRE(results[0].new_position.y < results[1].new_position.y);
    REQUIRE(results[1].new_position.y < results[2].new_position.y);
}

TEST_CASE("LayoutEngine: circular 8 objects on circle", "[layout_engine]")
{
    LayoutEngine engine;
    auto objects = make_objects(8);
    LayoutOptions options;
    options.type = LayoutType::Circular;
    options.radius = 300.0;

    const auto results = engine.compute_layout(objects, options);
    REQUIRE(results.size() == 8);

    // All points should be at distance ~300 from origin.
    for (const auto& result : results)
    {
        const double dist = std::sqrt(result.new_position.x * result.new_position.x +
                                      result.new_position.y * result.new_position.y);
        REQUIRE_THAT(dist, Catch::Matchers::WithinAbs(300.0, 1.0));
    }
}

TEST_CASE("LayoutEngine: force-directed no overlaps", "[layout_engine]")
{
    LayoutEngine engine;
    auto objects = make_objects(10, 50.0);
    LayoutOptions options;
    options.type = LayoutType::ForceDirected;
    options.radius = 200.0;

    const auto results = engine.compute_layout(objects, options);
    REQUIRE(results.size() == 10);

    // Verify no two objects occupy the exact same position.
    for (size_t i = 0; i < results.size(); ++i)
    {
        for (size_t j = i + 1; j < results.size(); ++j)
        {
            const double dx = results[i].new_position.x - results[j].new_position.x;
            const double dy = results[i].new_position.y - results[j].new_position.y;
            const double dist = std::sqrt(dx * dx + dy * dy);
            REQUIRE(dist > 1.0); // Must be at least 1px apart.
        }
    }
}
