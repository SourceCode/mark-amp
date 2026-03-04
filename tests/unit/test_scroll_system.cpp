/// @file test_scroll_system.cpp
/// @brief Phase 42 — Unit tests for scroll system models.

#include "ui/ScrollPhysicsEngine.h"
#include "ui/ScrollPositionStore.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ============================================================================
// ScrollPhysicsModel
// ============================================================================

TEST_CASE("ScrollPhysicsModel: immediate scroll", "[scroll_system]")
{
    ScrollPhysicsModel model;
    model.set_content_range(100, 1000);
    model.scroll_to(500, ScrollMode::kImmediate);
    REQUIRE(model.position() == 500.0);
    REQUIRE_FALSE(model.is_animating());
}

TEST_CASE("ScrollPhysicsModel: smooth scroll converges", "[scroll_system]")
{
    ScrollPhysicsModel model;
    model.set_content_range(100, 1000);
    model.scroll_to(500, ScrollMode::kSmooth);
    REQUIRE(model.is_animating());

    // Simulate ticks until convergence
    for (int i = 0; i < 500; ++i)
    {
        if (!model.tick(16.0))
            break;
    }
    REQUIRE_FALSE(model.is_animating());
    REQUIRE(model.position() == 500.0);
}

TEST_CASE("ScrollPhysicsModel: fling decelerates", "[scroll_system]")
{
    ScrollPhysicsModel model;
    model.set_content_range(100, 10000);
    model.set_position(100);
    model.fling(1000.0);
    REQUIRE(model.is_animating());

    double prev = model.position();
    for (int i = 0; i < 300; ++i)
    {
        if (!model.tick(16.0))
            break;
    }
    REQUIRE(model.position() > prev);
    REQUIRE_FALSE(model.is_animating());
}

TEST_CASE("ScrollPhysicsModel: max_scroll calculation", "[scroll_system]")
{
    ScrollPhysicsModel model;
    model.set_content_range(200, 500);
    REQUIRE(model.max_scroll() == 300.0);
}

TEST_CASE("ScrollPhysicsModel: content smaller than viewport", "[scroll_system]")
{
    ScrollPhysicsModel model;
    model.set_content_range(500, 200);
    REQUIRE(model.max_scroll() == 0.0);
}

TEST_CASE("ScrollPhysicsModel: stop cancels animation", "[scroll_system]")
{
    ScrollPhysicsModel model;
    model.set_content_range(100, 1000);
    model.fling(500.0);
    model.stop();
    REQUIRE_FALSE(model.is_animating());
}

TEST_CASE("ScrollPhysicsModel: overscroll detection", "[scroll_system]")
{
    ScrollPhysicsModel model;
    model.set_content_range(100, 1000);
    model.set_position(-10);
    REQUIRE(model.is_overscrolling());
    REQUIRE(model.overscroll_distance() == -10.0);
}

TEST_CASE("ScrollPhysicsModel: friction parameter", "[scroll_system]")
{
    ScrollPhysicsModel model;
    model.set_friction(0.98);
    REQUIRE(model.friction() == 0.98);
}

// ============================================================================
// ScrollPositionStore
// ============================================================================

TEST_CASE("ScrollPositionStore: save and restore", "[scroll_system]")
{
    ScrollPositionStore store;
    store.save("/path/to/file.md", {.line = 42, .column = 10, .pixel_offset = 5.0});

    auto pos = store.restore("/path/to/file.md");
    REQUIRE(pos.line == 42);
    REQUIRE(pos.column == 10);
    REQUIRE(pos.pixel_offset == 5.0);
}

TEST_CASE("ScrollPositionStore: missing file returns default", "[scroll_system]")
{
    ScrollPositionStore store;
    auto pos = store.restore("/nonexistent.md");
    REQUIRE(pos.line == 0);
    REQUIRE(pos.column == 0);
}

TEST_CASE("ScrollPositionStore: has check", "[scroll_system]")
{
    ScrollPositionStore store;
    REQUIRE_FALSE(store.has("/test.md"));
    store.save("/test.md", {.line = 1});
    REQUIRE(store.has("/test.md"));
}

TEST_CASE("ScrollPositionStore: remove", "[scroll_system]")
{
    ScrollPositionStore store;
    store.save("/test.md", {.line = 1});
    store.remove("/test.md");
    REQUIRE_FALSE(store.has("/test.md"));
    REQUIRE(store.count() == 0);
}

TEST_CASE("ScrollPositionStore: clear", "[scroll_system]")
{
    ScrollPositionStore store;
    store.save("/a.md", {.line = 1});
    store.save("/b.md", {.line = 2});
    REQUIRE(store.count() == 2);
    store.clear();
    REQUIRE(store.count() == 0);
}

TEST_CASE("ScrollPositionStore: eviction", "[scroll_system]")
{
    ScrollPositionStore store;
    store.set_max_entries(2);
    store.save("/a.md", {.line = 1});
    store.save("/b.md", {.line = 2});
    store.save("/c.md", {.line = 3});
    REQUIRE(store.count() == 2);
}
