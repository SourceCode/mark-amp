/// test_incremental_rendering.cpp — Phase 18: RenderPipeline integration tests
///
/// Validates the RenderPipeline coordinator: scroll dirty regions,
/// edit invalidation, resize handling, viewport ranges, and frame lifecycle.

#include "rendering/RenderPipeline.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::rendering;

// ══════════════════════════════════════════
// Initialization
// ══════════════════════════════════════════

TEST_CASE("RenderPipeline: initialize sets dimensions", "[render_pipeline]")
{
    RenderPipeline pipeline;
    REQUIRE_FALSE(pipeline.is_initialized());

    pipeline.initialize(800, 600);

    REQUIRE(pipeline.is_initialized());
    REQUIRE(pipeline.scroll_blit().width() == 800);
    REQUIRE(pipeline.scroll_blit().height() == 600);
}

// ══════════════════════════════════════════
// Scroll Events
// ══════════════════════════════════════════

TEST_CASE("RenderPipeline: scroll down produces bottom dirty rect", "[render_pipeline]")
{
    RenderPipeline pipeline;
    pipeline.initialize(800, 600);

    pipeline.on_scroll(0, 50); // scroll down 50px

    REQUIRE(pipeline.dirty_regions().has_dirty());
}

TEST_CASE("RenderPipeline: scroll up produces top dirty rect", "[render_pipeline]")
{
    RenderPipeline pipeline;
    pipeline.initialize(800, 600);

    pipeline.on_scroll(0, -30); // scroll up 30px

    REQUIRE(pipeline.dirty_regions().has_dirty());
}

TEST_CASE("RenderPipeline: horizontal scroll produces dirty rect", "[render_pipeline]")
{
    RenderPipeline pipeline;
    pipeline.initialize(800, 600);

    pipeline.on_scroll(40, 0); // scroll right 40px

    REQUIRE(pipeline.dirty_regions().has_dirty());
}

TEST_CASE("RenderPipeline: large scroll triggers full viewport dirty", "[render_pipeline]")
{
    RenderPipeline pipeline;
    pipeline.initialize(800, 600);

    pipeline.on_scroll(0, 700); // scroll more than viewport height

    auto work = pipeline.begin_frame(1000);
    REQUIRE_FALSE(work.dirty_rects.empty());
    // Full repaint because dirty area >= viewport area
    REQUIRE(work.full_repaint);
}

// ══════════════════════════════════════════
// Edit Events
// ══════════════════════════════════════════

TEST_CASE("RenderPipeline: edit invalidates line wrap cache", "[render_pipeline]")
{
    RenderPipeline pipeline;
    pipeline.initialize(800, 600);

    // Pre-populate wrap cache
    pipeline.line_wrap().set(5, WrapResult{{10, 20}, 80});
    REQUIRE(pipeline.line_wrap().is_valid(5, 80));

    // Edit line 5
    pipeline.on_edit(5, 6);

    REQUIRE_FALSE(pipeline.line_wrap().is_valid(5, 80));
    REQUIRE(pipeline.dirty_regions().has_dirty());
}

TEST_CASE("RenderPipeline: edit invalidates layout cache", "[render_pipeline]")
{
    RenderPipeline pipeline;
    pipeline.initialize(800, 600);

    // Pre-populate layout cache
    pipeline.layout_cache().set(10, LineMeasurement{20, 16, 1, 12345});
    REQUIRE(pipeline.layout_cache().get(10).has_value());

    // Edit lines 10-12
    pipeline.on_edit(10, 12);

    REQUIRE_FALSE(pipeline.layout_cache().get(10).has_value());
}

// ══════════════════════════════════════════
// Resize Events
// ══════════════════════════════════════════

TEST_CASE("RenderPipeline: resize invalidates all caches", "[render_pipeline]")
{
    RenderPipeline pipeline;
    pipeline.initialize(800, 600);

    // Populate some wrap data
    pipeline.line_wrap().set(0, WrapResult{{10}, 80});
    pipeline.line_wrap().set(1, WrapResult{{20, 40}, 80});

    pipeline.on_resize(1024, 768);

    REQUIRE(pipeline.scroll_blit().width() == 1024);
    REQUIRE(pipeline.scroll_blit().height() == 768);

    // Wrap cache should be invalidated
    REQUIRE(pipeline.line_wrap().size() == 0);
}

// ══════════════════════════════════════════
// Frame Lifecycle
// ══════════════════════════════════════════

TEST_CASE("RenderPipeline: begin_frame returns viewport range", "[render_pipeline]")
{
    RenderPipeline pipeline;
    pipeline.initialize(800, 600);
    pipeline.set_viewport(50, 30);

    auto work = pipeline.begin_frame(500);

    // render_start should include prefetch margin (default 10)
    REQUIRE(work.render_start_line == 40);
    REQUIRE(work.render_end_line == 90);
}

TEST_CASE("RenderPipeline: begin_frame clamps to total lines", "[render_pipeline]")
{
    RenderPipeline pipeline;
    pipeline.initialize(800, 600);
    pipeline.set_viewport(490, 30);

    auto work = pipeline.begin_frame(500);

    REQUIRE(work.render_end_line == 500);
}

TEST_CASE("RenderPipeline: end_frame resets scroll tracking", "[render_pipeline]")
{
    RenderPipeline pipeline;
    pipeline.initialize(800, 600);

    pipeline.on_scroll(0, 50);
    REQUIRE(pipeline.scroll_blit().accumulated_dy() == 50);

    pipeline.end_frame();
    REQUIRE(pipeline.scroll_blit().accumulated_dy() == 0);
}

TEST_CASE("RenderPipeline: begin_frame consumes dirty rects", "[render_pipeline]")
{
    RenderPipeline pipeline;
    pipeline.initialize(800, 600);

    pipeline.on_scroll(0, 20);
    REQUIRE(pipeline.dirty_regions().has_dirty());

    auto work = pipeline.begin_frame(100);
    REQUIRE_FALSE(work.dirty_rects.empty());

    // After begin_frame, dirty regions should be consumed
    REQUIRE_FALSE(pipeline.dirty_regions().has_dirty());
}

TEST_CASE("RenderPipeline: sequential scrolls accumulate dirty regions", "[render_pipeline]")
{
    RenderPipeline pipeline;
    pipeline.initialize(800, 600);

    pipeline.on_scroll(0, 10); // small scroll down
    pipeline.on_scroll(0, -5); // small scroll up

    auto work = pipeline.begin_frame(100);
    // Should have accumulated dirty rects from both scrolls
    REQUIRE_FALSE(work.dirty_rects.empty());
}
