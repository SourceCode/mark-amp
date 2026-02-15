#include "canvas/Quadtree.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Quadtree insert and query", "[canvas][quadtree]")
{
    const AABB world_bounds{-1000.0, -1000.0, 1000.0, 1000.0};
    Quadtree tree(world_bounds);

    SECTION("empty tree returns no results")
    {
        REQUIRE(tree.size() == 0);
        REQUIRE(tree.query_region({0.0, 0.0, 100.0, 100.0}).empty());
        REQUIRE(tree.query_point({50.0, 50.0}).empty());
    }

    SECTION("insert and query single object")
    {
        tree.insert(1, {10.0, 10.0, 50.0, 50.0});
        REQUIRE(tree.size() == 1);

        const auto results = tree.query_region({0.0, 0.0, 100.0, 100.0});
        REQUIRE(results.size() == 1);
        REQUIRE(results[0] == 1);
    }

    SECTION("query misses non-overlapping region")
    {
        tree.insert(1, {10.0, 10.0, 50.0, 50.0});
        const auto results = tree.query_region({200.0, 200.0, 300.0, 300.0});
        REQUIRE(results.empty());
    }

    SECTION("multiple objects with spatial filtering")
    {
        tree.insert(1, {0.0, 0.0, 10.0, 10.0});
        tree.insert(2, {50.0, 50.0, 60.0, 60.0});
        tree.insert(3, {500.0, 500.0, 510.0, 510.0});
        REQUIRE(tree.size() == 3);

        // Query region that overlaps only first two.
        const auto results = tree.query_region({-5.0, -5.0, 65.0, 65.0});
        REQUIRE(results.size() == 2);
    }

    SECTION("point query (hit test)")
    {
        tree.insert(1, {0.0, 0.0, 100.0, 100.0});
        tree.insert(2, {50.0, 50.0, 150.0, 150.0});

        // Point at (75, 75) overlaps both objects.
        const auto results = tree.query_point({75.0, 75.0});
        REQUIRE(results.size() == 2);

        // Point at (25, 25) overlaps only first.
        const auto results2 = tree.query_point({25.0, 25.0});
        REQUIRE(results2.size() == 1);
        REQUIRE(results2[0] == 1);
    }
}

TEST_CASE("Quadtree remove and update", "[canvas][quadtree]")
{
    const AABB world_bounds{-1000.0, -1000.0, 1000.0, 1000.0};
    Quadtree tree(world_bounds);

    tree.insert(1, {0.0, 0.0, 10.0, 10.0});
    tree.insert(2, {20.0, 20.0, 30.0, 30.0});

    SECTION("remove reduces size")
    {
        REQUIRE(tree.size() == 2);
        REQUIRE(tree.remove(1));
        REQUIRE(tree.size() == 1);

        const auto results = tree.query_region({0.0, 0.0, 10.0, 10.0});
        REQUIRE(results.empty());
    }

    SECTION("remove nonexistent returns false")
    {
        REQUIRE_FALSE(tree.remove(999));
    }

    SECTION("update moves object")
    {
        // Object 1 starts at (0,0)-(10,10).
        tree.update(1, {100.0, 100.0, 110.0, 110.0});

        // Old location should be empty.
        const auto old_results = tree.query_region({0.0, 0.0, 10.0, 10.0});
        REQUIRE(old_results.empty());

        // New location should find it.
        const auto new_results = tree.query_region({95.0, 95.0, 115.0, 115.0});
        REQUIRE(new_results.size() == 1);
        REQUIRE(new_results[0] == 1);
    }
}

TEST_CASE("Quadtree clear and rebuild", "[canvas][quadtree]")
{
    const AABB world_bounds{-1000.0, -1000.0, 1000.0, 1000.0};
    Quadtree tree(world_bounds);

    tree.insert(1, {0.0, 0.0, 10.0, 10.0});
    tree.insert(2, {20.0, 20.0, 30.0, 30.0});

    SECTION("clear empties tree")
    {
        tree.clear();
        REQUIRE(tree.size() == 0);
    }

    SECTION("rebuild from entries")
    {
        std::vector<Quadtree::Entry> entries = {
            {10, {0.0, 0.0, 5.0, 5.0}},
            {20, {50.0, 50.0, 55.0, 55.0}},
            {30, {100.0, 100.0, 105.0, 105.0}},
        };
        tree.rebuild(entries);
        REQUIRE(tree.size() == 3);
    }
}

TEST_CASE("Quadtree handles many objects", "[canvas][quadtree]")
{
    const AABB world_bounds{0.0, 0.0, 10000.0, 10000.0};
    Quadtree tree(world_bounds);

    // Insert 100 small objects spread across the world.
    for (int idx = 0; idx < 100; ++idx)
    {
        const double pos_x = static_cast<double>(idx) * 100.0;
        const double pos_y = static_cast<double>(idx) * 100.0;
        tree.insert(static_cast<ObjectId>(idx + 1), {pos_x, pos_y, pos_x + 10.0, pos_y + 10.0});
    }

    REQUIRE(tree.size() == 100);

    // Small query should return only nearby objects.
    const auto results = tree.query_region({0.0, 0.0, 50.0, 50.0});
    REQUIRE(results.size() == 1); // Only object at (0,0)-(10,10)
}
