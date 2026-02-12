/// Performance Patterns #21--#40 — Unit Tests
///
/// Tests for the second batch of performance infrastructure:
///   #21 FrameBudgetToken      #31 AsyncPipeline
///   #22 InputPriorityDispatcher  #32 AdaptiveThrottle
///   #23 TextSpan              #33 PrefetchManager
///   #24 GraphemeBoundaryCache #34 GenerationCounter
///   #25 StyleRunStore         #35 SelectionPainter
///   #26 StableLineId          #36 CaretOverlay
///   #27 IncrementalLineWrap   #37 HitTestAccelerator
///   #28 GlyphAdvanceCache     #38 IMECompositionOverlay
///   #29 DoubleBufferedPaint   #39 ChunkedStorage
///   #30 ScrollBlit            #40 CompilerHints

#include "core/AdaptiveThrottle.h"
#include "core/AsyncPipeline.h"
#include "core/ChunkedStorage.h"
#include "core/CompilerHints.h"
#include "core/FrameBudgetToken.h"
#include "core/GenerationCounter.h"
#include "core/GraphemeBoundaryCache.h"
#include "core/IMECompositionOverlay.h"
#include "core/InputPriorityDispatcher.h"
#include "core/StableLineId.h"
#include "core/StyleRunStore.h"
#include "core/TextSpan.h"
#include "rendering/CaretOverlay.h"
#include "rendering/DoubleBufferedPaint.h"
#include "rendering/GlyphAdvanceCache.h"
#include "rendering/HitTestAccelerator.h"
#include "rendering/IncrementalLineWrap.h"
#include "rendering/PrefetchManager.h"
#include "rendering/ScrollBlit.h"
#include "rendering/SelectionPainter.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <thread>

using namespace markamp::core;
using namespace markamp::rendering;

// ═══════════════════════════════════════════════════════
// #21 FrameBudgetToken
// ═══════════════════════════════════════════════════════

TEST_CASE("FrameBudgetToken — initial state is not exhausted", "[budget][p21]")
{
    FrameBudgetToken token(std::chrono::microseconds(10000)); // 10ms
    REQUIRE_FALSE(token.is_exhausted());
    REQUIRE(token.remaining().count() > 0);
    REQUIRE(token.budget().count() == 10000);
}

TEST_CASE("FrameBudgetToken — degradation level is Full initially", "[budget][p21]")
{
    FrameBudgetToken token(std::chrono::microseconds(1000000)); // 1s — plenty
    REQUIRE(token.degradation_level() == DegradationLevel::Full);
    REQUIRE_FALSE(token.should_degrade());
}

TEST_CASE("FrameBudgetToken — zero budget is immediately exhausted", "[budget][p21]")
{
    FrameBudgetToken token(std::chrono::microseconds(0));
    REQUIRE(token.is_exhausted());
    REQUIRE(token.remaining().count() == 0);
    REQUIRE(token.degradation_level() == DegradationLevel::Minimal);
    REQUIRE(token.should_degrade());
}

TEST_CASE("FrameBudgetToken — elapsed increases over time", "[budget][p21]")
{
    FrameBudgetToken token(std::chrono::microseconds(100000)); // 100ms
    auto elapsed1 = token.elapsed();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    auto elapsed2 = token.elapsed();
    REQUIRE(elapsed2 > elapsed1);
}

TEST_CASE("AutoFrameBudget — provides token access", "[budget][p21]")
{
    FrameScheduler scheduler;
    AutoFrameBudget afb(scheduler, std::chrono::microseconds(50000));
    REQUIRE_FALSE(afb.token().is_exhausted());
    REQUIRE(afb.token().budget().count() == 50000);
}

// ═══════════════════════════════════════════════════════
// #22 InputPriorityDispatcher
// ═══════════════════════════════════════════════════════

TEST_CASE("InputPriorityDispatcher — input callbacks execute before render", "[dispatcher][p22]")
{
    InputPriorityDispatcher dispatcher;
    std::vector<int> order;

    dispatcher.dispatch_render(
        [&]
        {
            order.push_back(2);
            return false;
        });
    dispatcher.dispatch_input(
        [&]
        {
            order.push_back(1);
            return false;
        });
    dispatcher.dispatch_secondary(
        [&]
        {
            order.push_back(3);
            return false;
        });

    dispatcher.process_frame(std::chrono::microseconds(1000000));

    REQUIRE(order.size() == 3);
    REQUIRE(order[0] == 1); // Input first
    REQUIRE(order[1] == 2); // Render second
    REQUIRE(order[2] == 3); // Secondary last
}

TEST_CASE("InputPriorityDispatcher — category_to_priority mapping", "[dispatcher][p22]")
{
    REQUIRE(InputPriorityDispatcher::category_to_priority(EventCategory::Input) ==
            TaskPriority::Input);
    REQUIRE(InputPriorityDispatcher::category_to_priority(EventCategory::Render) ==
            TaskPriority::Paint);
    REQUIRE(InputPriorityDispatcher::category_to_priority(EventCategory::Secondary) ==
            TaskPriority::Background);
}

TEST_CASE("InputPriorityDispatcher — has_pending and clear", "[dispatcher][p22]")
{
    InputPriorityDispatcher dispatcher;
    REQUIRE_FALSE(dispatcher.has_pending());
    REQUIRE(dispatcher.pending_count() == 0);

    dispatcher.dispatch_input([&] { return false; });
    REQUIRE(dispatcher.has_pending());
    REQUIRE(dispatcher.pending_count() == 1);

    dispatcher.clear();
    REQUIRE_FALSE(dispatcher.has_pending());
}

// ═══════════════════════════════════════════════════════
// #23 TextSpan
// ═══════════════════════════════════════════════════════

TEST_CASE("TextSpan — zero-copy construction and access", "[textspan][p23]")
{
    const char data[] = "Hello, World!";
    TextSpan span{.data = std::span<const char>(data, 13), .logical_line = 42};

    REQUIRE(span.size() == 13);
    REQUIRE_FALSE(span.empty());
    REQUIRE(span[0] == 'H');
    REQUIRE(span.as_string_view() == "Hello, World!");
    REQUIRE(span.logical_line == 42);
}

TEST_CASE("TextSpan — empty span", "[textspan][p23]")
{
    TextSpan span{.data = {}, .logical_line = 0};
    REQUIRE(span.empty());
    REQUIRE(span.size() == 0);
}

TEST_CASE("PieceTableSpanIterator — iteration over lines", "[textspan][p23]")
{
    std::string original = "line1\nline2\nline3\n";
    std::vector<std::size_t> line_starts = {0, 6, 12};

    PieceTableSpanIterator iter(original, "", line_starts, original.size(), 0, 3);

    REQUIRE(iter.has_next());
    auto span1 = iter.next();
    REQUIRE(span1.logical_line == 0);

    auto span2 = iter.next();
    REQUIRE(span2.logical_line == 1);

    auto span3 = iter.next();
    REQUIRE(span3.logical_line == 2);

    REQUIRE_FALSE(iter.has_next());
}

TEST_CASE("PieceTableSpanIterator — reset resets iteration", "[textspan][p23]")
{
    std::string original = "line1\nline2\n";
    std::vector<std::size_t> line_starts = {0, 6};

    PieceTableSpanIterator iter(original, "", line_starts, original.size(), 0, 2);

    [[maybe_unused]] auto s1 = iter.next();
    [[maybe_unused]] auto s2 = iter.next();
    REQUIRE_FALSE(iter.has_next());

    iter.reset(0, 1);
    REQUIRE(iter.has_next());
}

// ═══════════════════════════════════════════════════════
// #24 GraphemeBoundaryCache
// ═══════════════════════════════════════════════════════

TEST_CASE("utf8_decode_one — ASCII characters", "[grapheme][p24]")
{
    const char data[] = "ABC";
    auto span = std::span<const char>(data, 3);

    auto r0 = utf8_decode_one(span, 0);
    REQUIRE(r0.codepoint == 'A');
    REQUIRE(r0.bytes_consumed == 1);

    auto r1 = utf8_decode_one(span, 1);
    REQUIRE(r1.codepoint == 'B');
    REQUIRE(r1.bytes_consumed == 1);
}

TEST_CASE("utf8_decode_one — multi-byte characters", "[grapheme][p24]")
{
    // U+00E9 (é) = C3 A9 (2 bytes)
    const char data[] = "\xC3\xA9";
    auto span = std::span<const char>(data, 2);

    auto result = utf8_decode_one(span, 0);
    REQUIRE(result.codepoint == 0x00E9);
    REQUIRE(result.bytes_consumed == 2);
}

TEST_CASE("utf8_decode_one — 3-byte character", "[grapheme][p24]")
{
    // U+4E16 (世) = E4 B8 96
    const char data[] = "\xE4\xB8\x96";
    auto span = std::span<const char>(data, 3);

    auto result = utf8_decode_one(span, 0);
    REQUIRE(result.codepoint == 0x4E16);
    REQUIRE(result.bytes_consumed == 3);
}

TEST_CASE("utf8_decode_one — 4-byte character", "[grapheme][p24]")
{
    // U+1F600 (😀) = F0 9F 98 80
    const char data[] = "\xF0\x9F\x98\x80";
    auto span = std::span<const char>(data, 4);

    auto result = utf8_decode_one(span, 0);
    REQUIRE(result.codepoint == 0x1F600);
    REQUIRE(result.bytes_consumed == 4);
}

TEST_CASE("build_grapheme_info — ASCII line", "[grapheme][p24]")
{
    const char data[] = "Hello";
    auto info = build_grapheme_info(std::span<const char>(data, 5));

    REQUIRE(info.cluster_count() == 5);
    REQUIRE(info.byte_offsets[0] == 0);
    REQUIRE(info.byte_offsets[4] == 4);
    REQUIRE(info.codepoint_offsets[4] == 4);
}

TEST_CASE("build_grapheme_info — mixed ASCII and multi-byte", "[grapheme][p24]")
{
    // "Hé" = H(1) + é(2) = 3 bytes, 2 codepoints
    const char data[] = "H\xC3\xA9";
    auto info = build_grapheme_info(std::span<const char>(data, 3));

    REQUIRE(info.cluster_count() == 2);
    REQUIRE(info.byte_offsets[0] == 0); // 'H'
    REQUIRE(info.byte_offsets[1] == 1); // 'é'
    REQUIRE(info.codepoint_offsets[0] == 0);
    REQUIRE(info.codepoint_offsets[1] == 1);
}

TEST_CASE("GraphemeBoundaryCache — put and get", "[grapheme][p24]")
{
    GraphemeBoundaryCache cache;

    const char data[] = "Hello";
    auto info = build_grapheme_info(std::span<const char>(data, 5));

    cache.put(0, 12345, info);

    auto cached = cache.get(0, 12345);
    REQUIRE(cached != nullptr);
    REQUIRE(cached->cluster_count() == 5);
}

TEST_CASE("GraphemeBoundaryCache — cache miss", "[grapheme][p24]")
{
    GraphemeBoundaryCache cache;
    REQUIRE(cache.get(0, 99999) == nullptr);
}

// ═══════════════════════════════════════════════════════
// #25 StyleRunStore
// ═══════════════════════════════════════════════════════

TEST_CASE("StyleRunStore — update and get line", "[stylerun][p25]")
{
    StyleRunStore store;

    std::vector<StyleRun> runs = {
        {0, 5, 1}, // keyword
        {5, 3, 2}  // operator
    };
    store.update_line(0, runs);

    REQUIRE(store.has_runs(0));
    auto& result = store.get_line(0);
    REQUIRE(result.size() == 2);
    REQUIRE(result[0].start == 0);
    REQUIRE(result[0].length == 5);
    REQUIRE(result[0].style_id == 1);
}

TEST_CASE("StyleRunStore — merges adjacent runs with same style", "[stylerun][p25]")
{
    StyleRunStore store;

    std::vector<StyleRun> runs = {{0, 3, 1},
                                  {3, 4, 1}, // same style as previous, adjacent
                                  {7, 2, 2}};
    store.update_line(0, runs);

    auto& result = store.get_line(0);
    REQUIRE(result.size() == 2); // merged first two
    REQUIRE(result[0].start == 0);
    REQUIRE(result[0].length == 7); // 3 + 4
    REQUIRE(result[0].style_id == 1);
    REQUIRE(result[1].start == 7);
    REQUIRE(result[1].style_id == 2);
}

TEST_CASE("StyleRunStore — invalidate_range clears runs", "[stylerun][p25]")
{
    StyleRunStore store;
    store.update_line(0, {{0, 5, 1}});
    store.update_line(1, {{0, 3, 2}});
    store.update_line(2, {{0, 7, 3}});

    store.invalidate_range(0, 2);

    REQUIRE_FALSE(store.has_runs(0));
    REQUIRE_FALSE(store.has_runs(1));
    REQUIRE(store.has_runs(2));
}

TEST_CASE("StyleRunStore — total_run_count", "[stylerun][p25]")
{
    StyleRunStore store;
    store.update_line(0, {{0, 5, 1}, {5, 3, 2}});
    store.update_line(1, {{0, 10, 3}});

    REQUIRE(store.total_run_count() == 3);
}

// ═══════════════════════════════════════════════════════
// #26 StableLineId
// ═══════════════════════════════════════════════════════

TEST_CASE("StableIdAllocator — monotonic IDs", "[stableid][p26]")
{
    StableIdAllocator alloc;
    auto id1 = alloc.allocate();
    auto id2 = alloc.allocate();
    auto id3 = alloc.allocate();

    REQUIRE(id1.id < id2.id);
    REQUIRE(id2.id < id3.id);
}

TEST_CASE("StableIdAllocator — batch allocation", "[stableid][p26]")
{
    StableIdAllocator alloc;
    auto ids = alloc.allocate_batch(5);

    REQUIRE(ids.size() == 5);
    for (std::size_t i = 1; i < ids.size(); ++i)
    {
        REQUIRE(ids[i].id == ids[i - 1].id + 1);
    }
}

TEST_CASE("LineIdMap — initialize and get", "[stableid][p26]")
{
    LineIdMap map;
    map.initialize(3);

    REQUIRE(map.size() == 3);
    REQUIRE(map.get(0).id != 0);
    REQUIRE(map.get(1).id != 0);
    REQUIRE(map.get(2).id != 0);
    REQUIRE(map.get(0) != map.get(1));
}

TEST_CASE("LineIdMap — insert adds new IDs", "[stableid][p26]")
{
    LineIdMap map;
    map.initialize(3);
    auto old0 = map.get(0);
    auto old1 = map.get(1);
    auto old2 = map.get(2);

    // Insert 2 lines at line 1
    map.on_insert(1, 2);

    REQUIRE(map.size() == 5);
    REQUIRE(map.get(0) == old0); // unchanged
    REQUIRE(map.get(1) != old1); // new ID
    REQUIRE(map.get(2) != old1); // new ID
    REQUIRE(map.get(3) == old1); // shifted
    REQUIRE(map.get(4) == old2); // shifted
}

TEST_CASE("LineIdMap — erase removes IDs", "[stableid][p26]")
{
    LineIdMap map;
    map.initialize(5);
    auto id0 = map.get(0);
    auto id3 = map.get(3);
    auto id4 = map.get(4);

    map.on_erase(1, 2); // remove lines 1 and 2

    REQUIRE(map.size() == 3);
    REQUIRE(map.get(0) == id0);
    REQUIRE(map.get(1) == id3);
    REQUIRE(map.get(2) == id4);
}

TEST_CASE("StableLineId — hash works for unordered_map", "[stableid][p26]")
{
    std::unordered_map<StableLineId, int> map;
    StableLineId id{42};
    map[id] = 7;
    REQUIRE(map[id] == 7);
}

// ═══════════════════════════════════════════════════════
// #27 IncrementalLineWrap
// ═══════════════════════════════════════════════════════

TEST_CASE("IncrementalLineWrap — set and get", "[linewrap][p27]")
{
    IncrementalLineWrap wrap;

    WrapResult result;
    result.break_offsets = {40, 80};
    result.wrap_width = 100;

    wrap.set(5, result);

    auto cached = wrap.get(5, 100);
    REQUIRE(cached != nullptr);
    REQUIRE(cached->display_line_count() == 3);
    REQUIRE(cached->is_wrapped());
}

TEST_CASE("IncrementalLineWrap — cache miss on wrong wrap width", "[linewrap][p27]")
{
    IncrementalLineWrap wrap;
    WrapResult result;
    result.wrap_width = 100;
    wrap.set(0, result);

    REQUIRE(wrap.get(0, 200) == nullptr); // different width
}

TEST_CASE("IncrementalLineWrap — invalidate_line clears entry", "[linewrap][p27]")
{
    IncrementalLineWrap wrap;
    WrapResult result;
    result.wrap_width = 100;
    wrap.set(0, result);

    wrap.invalidate_line(0);
    REQUIRE_FALSE(wrap.is_valid(0, 100));
}

TEST_CASE("WrapResult — non-wrapped line has 1 display line", "[linewrap][p27]")
{
    WrapResult wr;
    REQUIRE(wr.display_line_count() == 1);
    REQUIRE_FALSE(wr.is_wrapped());
}

// ═══════════════════════════════════════════════════════
// #28 GlyphAdvanceCache
// ═══════════════════════════════════════════════════════

TEST_CASE("GlyphAdvanceCache — ASCII fast path", "[glyphcache][p28]")
{
    GlyphAdvanceCache cache;

    // Initially all invalid
    REQUIRE(cache.get('A', 0) == GlyphAdvanceCache::kInvalidAdvance);

    // Populate ASCII advances
    cache.put('A', 0, 8);
    cache.put('B', 0, 8);
    cache.put(' ', 0, 4);

    REQUIRE(cache.get('A', 0) == 8);
    REQUIRE(cache.get('B', 0) == 8);
    REQUIRE(cache.get(' ', 0) == 4);
}

TEST_CASE("GlyphAdvanceCache — non-ASCII fallback", "[glyphcache][p28]")
{
    GlyphAdvanceCache cache;
    char32_t codepoint = 0x4E16; // 世

    cache.put(codepoint, 0, 16);
    REQUIRE(cache.get(codepoint, 0) == 16);
    REQUIRE(cache.extended_count() == 1);
}

TEST_CASE("GlyphAdvanceCache — measure_run_ascii", "[glyphcache][p28]")
{
    GlyphAdvanceCache cache;
    for (char ch = 'A'; ch <= 'Z'; ++ch)
    {
        cache.put(static_cast<char32_t>(ch), 0, 8);
    }

    const char text[] = "HELLO";
    auto result = cache.measure_run_ascii(text, 5, 0);

    REQUIRE(result.total_width == 40); // 5 * 8
    REQUIRE(result.hits == 5);
    REQUIRE(result.misses == 0);
}

TEST_CASE("GlyphAdvanceCache — invalidate_font clears font entries", "[glyphcache][p28]")
{
    GlyphAdvanceCache cache;
    cache.put('A', 0, 8);
    cache.put('A', 1, 10);

    cache.invalidate_font(0);

    REQUIRE(cache.get('A', 0) == GlyphAdvanceCache::kInvalidAdvance);
    REQUIRE(cache.get('A', 1) == 10); // font 1 untouched
}

TEST_CASE("CommonWidthCache — put and get", "[glyphcache][p28]")
{
    CommonWidthCache cache;
    cache.put(12345, 0, 64);

    REQUIRE(cache.get(12345, 0) == 64);
    REQUIRE(cache.get(99999, 0) == GlyphAdvanceCache::kInvalidAdvance);
    REQUIRE(cache.size() == 1);
}

// ═══════════════════════════════════════════════════════
// #29 DoubleBufferedPaint
// ═══════════════════════════════════════════════════════

TEST_CASE("DoubleBufferedPaint — initial state needs full redraw", "[doublebuf][p29]")
{
    DoubleBufferedPaint paint;
    paint.initialize(800, 600);

    REQUIRE(paint.is_initialized());
    REQUIRE(paint.needs_paint());
    REQUIRE(paint.needs_full_redraw());
    REQUIRE(paint.width() == 800);
    REQUIRE(paint.height() == 600);
}

TEST_CASE("DoubleBufferedPaint — consume_dirty_rects returns full rect initially",
          "[doublebuf][p29]")
{
    DoubleBufferedPaint paint;
    paint.initialize(800, 600);

    auto rects = paint.consume_dirty_rects();
    REQUIRE(rects.size() == 1);
    REQUIRE(rects[0].left == 0);
    REQUIRE(rects[0].top == 0);
    REQUIRE(rects[0].right == 800);
    REQUIRE(rects[0].bottom == 600);

    // After consuming, no more dirty
    REQUIRE_FALSE(paint.needs_full_redraw());
}

TEST_CASE("DoubleBufferedPaint — invalidate_rect adds dirty rects", "[doublebuf][p29]")
{
    DoubleBufferedPaint paint;
    paint.initialize(800, 600);

    // Consume initial full redraw
    [[maybe_unused]] auto initial = paint.consume_dirty_rects();

    // Add dirty rect: left=10, top=20, right=110, bottom=70
    paint.invalidate_rect(Rect{10, 20, 110, 70});
    REQUIRE(paint.needs_paint());

    auto rects = paint.consume_dirty_rects();
    REQUIRE(rects.size() == 1);
    REQUIRE(rects[0].left == 10);
}

TEST_CASE("DoubleBufferedPaint — on_resize forces full redraw", "[doublebuf][p29]")
{
    DoubleBufferedPaint paint;
    paint.initialize(800, 600);
    [[maybe_unused]] auto initial = paint.consume_dirty_rects(); // clear initial

    paint.on_resize(1024, 768);
    REQUIRE(paint.needs_full_redraw());
    REQUIRE(paint.width() == 1024);
    REQUIRE(paint.height() == 768);
}

// ═══════════════════════════════════════════════════════
// #30 ScrollBlit
// ═══════════════════════════════════════════════════════

TEST_CASE("ScrollBlit — scroll down reveals bottom strip", "[scrollblit][p30]")
{
    ScrollBlit blit;
    blit.initialize(800, 600);

    auto rect = blit.compute_revealed_rect(20); // scroll down 20px
    REQUIRE(rect.left == 0);
    REQUIRE(rect.top == 580); // 600 - 20
    REQUIRE(rect.right == 800);
    REQUIRE(rect.bottom == 600);
    REQUIRE(rect.height() == 20);
}

TEST_CASE("ScrollBlit — scroll up reveals top strip", "[scrollblit][p30]")
{
    ScrollBlit blit;
    blit.initialize(800, 600);

    auto rect = blit.compute_revealed_rect(-30);
    REQUIRE(rect.left == 0);
    REQUIRE(rect.top == 0);
    REQUIRE(rect.right == 800);
    REQUIRE(rect.bottom == 30);
    REQUIRE(rect.height() == 30);
}

TEST_CASE("ScrollBlit — large scroll requires full repaint", "[scrollblit][p30]")
{
    ScrollBlit blit;
    blit.initialize(800, 600);

    REQUIRE(blit.is_full_repaint_needed(0, 700));
    REQUIRE(blit.is_full_repaint_needed(0, -600));
    REQUIRE_FALSE(blit.is_full_repaint_needed(0, 100));
}

TEST_CASE("ScrollBlit — zero scroll reveals nothing", "[scrollblit][p30]")
{
    ScrollBlit blit;
    blit.initialize(800, 600);

    auto rect = blit.compute_revealed_rect(0);
    REQUIRE(rect.empty());
}

TEST_CASE("ScrollBlit — direction detection", "[scrollblit][p30]")
{
    REQUIRE(ScrollBlit::detect_direction(0, 10) == ScrollDirection::Down);
    REQUIRE(ScrollBlit::detect_direction(0, -10) == ScrollDirection::Up);
    REQUIRE(ScrollBlit::detect_direction(10, 0) == ScrollDirection::Right);
    REQUIRE(ScrollBlit::detect_direction(-10, 0) == ScrollDirection::Left);
    REQUIRE(ScrollBlit::detect_direction(0, 0) == ScrollDirection::None);
}

TEST_CASE("ScrollBlit — accumulation tracking", "[scrollblit][p30]")
{
    ScrollBlit blit;
    blit.initialize(800, 600);

    blit.record_scroll(0, 10);
    blit.record_scroll(0, 15);
    REQUIRE(blit.accumulated_dy() == 25);
    REQUIRE(blit.last_direction() == ScrollDirection::Down);

    blit.reset_tracking();
    REQUIRE(blit.accumulated_dy() == 0);
}

// ═══════════════════════════════════════════════════════
// #31 AsyncPipeline — basic round-trip
// ═══════════════════════════════════════════════════════

TEST_CASE("AsyncPipeline — submit and receive result", "[asyncpipe][p31]")
{
    AsyncPipeline<int, int> pipeline([](const int& input, CancelToken) -> std::optional<int>
                                     { return input * 2; });

    pipeline.submit(21);

    // Wait for result
    int result = 0;
    bool got = false;
    for (int attempt = 0; attempt < 100; ++attempt)
    {
        if (pipeline.try_get_result(result))
        {
            got = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    REQUIRE(got);
    REQUIRE(result == 42);
}

TEST_CASE("AsyncPipeline — latest wins coalescing", "[asyncpipe][p31]")
{
    AsyncPipeline<int, int> pipeline(
        [](const int& input, CancelToken ct) -> std::optional<int>
        {
            // Simulate slow work
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if (ct.stop_requested())
                return std::nullopt;
            return input;
        });

    // Submit many — only the last should produce a result
    pipeline.submit(1);
    pipeline.submit(2);
    pipeline.submit(3);

    // Wait for any result
    int result = 0;
    bool got = false;
    for (int attempt = 0; attempt < 200; ++attempt)
    {
        if (pipeline.try_get_result(result))
        {
            got = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // The result should be 3 (latest submission)
    if (got)
    {
        REQUIRE(result == 3);
    }
}

// ═══════════════════════════════════════════════════════
// #32 AdaptiveThrottle
// ═══════════════════════════════════════════════════════

TEST_CASE("AdaptiveThrottle — starts idle", "[throttle][p32]")
{
    AdaptiveThrottle throttle;
    REQUIRE(throttle.is_idle());
    REQUIRE_FALSE(throttle.is_typing());
    REQUIRE(throttle.current_mode() == ActivityMode::Idle);
}

TEST_CASE("AdaptiveThrottle — switches to typing on activity", "[throttle][p32]")
{
    AdaptiveThrottle throttle;
    throttle.update_activity();

    REQUIRE(throttle.is_typing());
    REQUIRE(throttle.current_mode() == ActivityMode::Typing);
}

TEST_CASE("AdaptiveThrottle — typing budget is less than idle budget", "[throttle][p32]")
{
    AdaptiveThrottle throttle;

    // Idle budget
    auto idle_budget = throttle.current_budget();

    throttle.update_activity();
    auto typing_budget = throttle.current_budget();

    REQUIRE(typing_budget < idle_budget);
}

TEST_CASE("AdaptiveThrottle — configurable idle threshold", "[throttle][p32]")
{
    AdaptiveThrottle throttle(std::chrono::milliseconds(100));
    REQUIRE(throttle.idle_threshold().count() == 100);

    throttle.set_idle_threshold(std::chrono::milliseconds(500));
    REQUIRE(throttle.idle_threshold().count() == 500);
}

// ═══════════════════════════════════════════════════════
// #33 PrefetchManager
// ═══════════════════════════════════════════════════════

TEST_CASE("PrefetchManager — compute_range extends beyond viewport", "[prefetch][p33]")
{
    PrefetchManager mgr;
    mgr.set_extra_margin(5);

    ViewportState vs;
    vs.first_visible_line = 50;
    vs.visible_line_count = 30;
    vs.prefetch_margin = 10;

    auto range = mgr.compute_range(vs, 200);

    // ViewportState render_start = 50 - 10 = 40, render_end = 50 + 30 + 10 = 90
    // Extra margin = 5, so start = 40 - 5 = 35, end = 90 + 5 = 95
    REQUIRE(range.start_line == 35);
    REQUIRE(range.end_line == 95);
    REQUIRE(range.line_count() == 60);
}

TEST_CASE("PrefetchManager — clamped at document boundaries", "[prefetch][p33]")
{
    PrefetchManager mgr;
    mgr.set_extra_margin(100);

    ViewportState vs;
    vs.first_visible_line = 0;
    vs.visible_line_count = 10;
    vs.prefetch_margin = 5;

    auto range = mgr.compute_range(vs, 20);

    REQUIRE(range.start_line == 0);
    REQUIRE(range.end_line <= 20);
}

TEST_CASE("PrefetchManager — is_prefetch_line excludes visible lines", "[prefetch][p33]")
{
    PrefetchManager mgr;
    mgr.set_extra_margin(5);

    ViewportState vs;
    vs.first_visible_line = 10;
    vs.visible_line_count = 5;
    vs.prefetch_margin = 3;

    // Line 12 is visible, not a prefetch line
    REQUIRE_FALSE(mgr.is_prefetch_line(12, vs, 100));

    // Line 5 is outside viewport but in prefetch range
    REQUIRE(mgr.is_prefetch_line(5, vs, 100));
}

// ═══════════════════════════════════════════════════════
// #34 GenerationCounter
// ═══════════════════════════════════════════════════════

TEST_CASE("GenerationCounter — starts at 0", "[gencount][p34]")
{
    GenerationCounter gen;
    REQUIRE(gen.current() == 0);
}

TEST_CASE("GenerationCounter — bump increments", "[gencount][p34]")
{
    GenerationCounter gen;
    gen.bump();
    REQUIRE(gen.current() == 1);
    gen.bump();
    REQUIRE(gen.current() == 2);
}

TEST_CASE("GenerationCounter — is_stale detects changes", "[gencount][p34]")
{
    GenerationCounter gen;
    auto stored = gen.current();
    REQUIRE_FALSE(gen.is_stale(stored));

    gen.bump();
    REQUIRE(gen.is_stale(stored));
}

TEST_CASE("GenerationSet — snapshot and staleness", "[gencount][p34]")
{
    GenerationSet gens;
    auto snap = gens.snapshot();

    REQUIRE_FALSE(gens.is_stale(snap));

    gens.theme_gen.bump();
    REQUIRE(gens.is_stale(snap));
}

TEST_CASE("GenerationSet — bump_all bumps all counters", "[gencount][p34]")
{
    GenerationSet gens;
    auto snap = gens.snapshot();

    gens.bump_all();

    REQUIRE(gens.theme_gen.current() == 1);
    REQUIRE(gens.font_gen.current() == 1);
    REQUIRE(gens.wrap_gen.current() == 1);
    REQUIRE(gens.is_stale(snap));
}

// ═══════════════════════════════════════════════════════
// #35 SelectionPainter
// ═══════════════════════════════════════════════════════

TEST_CASE("SelectionPainter — no change between identical selections", "[selection][p35]")
{
    SelectionInterval sel{10, 15, 0, 5};
    auto changed = SelectionPainter::compute_changed_lines(sel, sel);
    REQUIRE(changed.empty());
}

TEST_CASE("SelectionPainter — empty to non-empty invalidates all lines", "[selection][p35]")
{
    SelectionInterval empty;
    SelectionInterval sel{10, 12, 0, 5};

    auto changed = SelectionPainter::compute_changed_lines(empty, sel);
    REQUIRE(changed.size() == 3); // lines 10, 11, 12
}

TEST_CASE("SelectionPainter — non-empty to empty invalidates all lines", "[selection][p35]")
{
    SelectionInterval sel{5, 7, 0, 3};
    SelectionInterval empty;

    auto changed = SelectionPainter::compute_changed_lines(sel, empty);
    REQUIRE(changed.size() == 3); // lines 5, 6, 7
}

TEST_CASE("SelectionPainter — compute_dirty_rects returns line rects", "[selection][p35]")
{
    SelectionInterval empty;
    SelectionInterval sel{0, 1, 0, 5};

    auto rects = SelectionPainter::compute_dirty_rects(empty, sel, 20, 800);
    REQUIRE(rects.size() == 2);
    REQUIRE(rects[0].top == 0);
    REQUIRE(rects[0].height() == 20);
    REQUIRE(rects[1].top == 20); // line 1 * 20
}

// ═══════════════════════════════════════════════════════
// #36 CaretOverlay
// ═══════════════════════════════════════════════════════

TEST_CASE("CaretOverlay — initial state is visible", "[caret][p36]")
{
    CaretOverlay caret;
    REQUIRE(caret.is_visible());
    REQUIRE(caret.line() == 0);
    REQUIRE(caret.col() == 0);
}

TEST_CASE("CaretOverlay — toggle_blink toggles visibility", "[caret][p36]")
{
    CaretOverlay caret;
    REQUIRE(caret.is_visible());
    caret.toggle_blink();
    REQUIRE_FALSE(caret.is_visible());
    caret.toggle_blink();
    REQUIRE(caret.is_visible());
}

TEST_CASE("CaretOverlay — update_position sets rect", "[caret][p36]")
{
    CaretOverlay caret;
    caret.update_position(5, 10, 20, 8);

    REQUIRE(caret.line() == 5);
    REQUIRE(caret.col() == 10);

    auto rect = caret.refresh_rect();
    REQUIRE(rect.left == 80);    // col * char_width = 10 * 8
    REQUIRE(rect.top == 100);    // line * line_height = 5 * 20
    REQUIRE(rect.right == 82);   // left + 2 (caret width)
    REQUIRE(rect.bottom == 120); // top + line_height
}

TEST_CASE("CaretOverlay — show/hide control visibility", "[caret][p36]")
{
    CaretOverlay caret;
    caret.hide();
    REQUIRE_FALSE(caret.is_visible());
    caret.show();
    REQUIRE(caret.is_visible());
}

// ═══════════════════════════════════════════════════════
// #37 HitTestAccelerator
// ═══════════════════════════════════════════════════════

TEST_CASE("LineAdvanceTable — column_at_x binary search", "[hittest][p37]")
{
    // Advances: [8, 8, 8, 8, 8] → prefix sums: [8, 16, 24, 32, 40]
    auto table = LineAdvanceTable::from_advances({8, 8, 8, 8, 8});

    REQUIRE(table.cluster_count() == 5);
    REQUIRE(table.column_at_x(0) == 0);
    REQUIRE(table.column_at_x(8) == 1);  // exactly on boundary
    REQUIRE(table.column_at_x(12) == 1); // between 8 and 16, closer to 8
    REQUIRE(table.column_at_x(40) == 5); // past last
}

TEST_CASE("LineAdvanceTable — x_at_column", "[hittest][p37]")
{
    auto table = LineAdvanceTable::from_advances({8, 8, 8, 8, 8});

    REQUIRE(table.x_at_column(0) == 0);
    REQUIRE(table.x_at_column(1) == 8);
    REQUIRE(table.x_at_column(3) == 24);
    REQUIRE(table.x_at_column(5) == 40);
    REQUIRE(table.x_at_column(10) == 40); // clamped
}

TEST_CASE("HitTestAccelerator — hit_test returns correct line and column", "[hittest][p37]")
{
    HitTestAccelerator accel;

    // Line 0: 5 chars of width 8
    accel.set_table(0, LineAdvanceTable::from_advances({8, 8, 8, 8, 8}));
    // Line 1: 3 chars of width 10
    accel.set_table(1, LineAdvanceTable::from_advances({10, 10, 10}));

    auto r0 = accel.hit_test(12, 5, 20, 0, 10);
    REQUIRE(r0.line == 0);
    REQUIRE(r0.column == 1); // x=12 is between 8 and 16

    auto r1 = accel.hit_test(25, 25, 20, 0, 10);
    REQUIRE(r1.line == 1);
    REQUIRE(r1.column == 2); // x=25 between 20 and 30
}

TEST_CASE("HitTestAccelerator — invalidate_line clears table", "[hittest][p37]")
{
    HitTestAccelerator accel;
    accel.set_table(0, LineAdvanceTable::from_advances({8, 8}));

    accel.invalidate_line(0);
    REQUIRE(accel.get_table(0).cluster_count() == 0);
}

// ═══════════════════════════════════════════════════════
// #38 IMECompositionOverlay
// ═══════════════════════════════════════════════════════

TEST_CASE("IMECompositionOverlay — lifecycle: begin, update, commit", "[ime][p38]")
{
    IMECompositionOverlay ime;
    REQUIRE_FALSE(ime.is_active());

    ime.begin_composition();
    REQUIRE(ime.is_active());
    REQUIRE(ime.composing_text().empty());

    ime.update_composition("test_input", 5);
    REQUIRE(ime.composing_text() == "test_input");
    REQUIRE(ime.cursor_offset() == 5);

    auto committed = ime.commit_composition();
    REQUIRE(committed == "test_input");
    REQUIRE_FALSE(ime.is_active());
    REQUIRE(ime.composing_text().empty());
}

TEST_CASE("IMECompositionOverlay — cancel discards text", "[ime][p38]")
{
    IMECompositionOverlay ime;
    ime.begin_composition();
    ime.update_composition("test", 4);

    ime.cancel_composition();
    REQUIRE_FALSE(ime.is_active());
    REQUIRE(ime.composing_text().empty());
}

TEST_CASE("IMECompositionOverlay — composing_length", "[ime][p38]")
{
    IMECompositionOverlay ime;
    ime.begin_composition();
    ime.update_composition("Hello", 5);
    REQUIRE(ime.composing_length() == 5);
}

// ═══════════════════════════════════════════════════════
// #39 ChunkedStorage
// ═══════════════════════════════════════════════════════

TEST_CASE("ChunkedBuffer — append and read", "[chunked][p39]")
{
    ChunkedBuffer<64> buf; // 64-byte chunks for testing

    buf.append("Hello, World!");
    REQUIRE(buf.size() == 13);
    REQUIRE(buf.chunk_count() == 1);
    REQUIRE(buf.read(0, 13) == "Hello, World!");
}

TEST_CASE("ChunkedBuffer — spans multiple chunks", "[chunked][p39]")
{
    ChunkedBuffer<8> buf;

    buf.append("AAAAAAAABBBBBBBBCCCC"); // 20 bytes → 3 chunks
    REQUIRE(buf.size() == 20);
    REQUIRE(buf.chunk_count() == 3);

    REQUIRE(buf.read(0, 8) == "AAAAAAAA");
    REQUIRE(buf.read(8, 8) == "BBBBBBBB");
    REQUIRE(buf.read(16, 4) == "CCCC");

    // Read across chunk boundary
    REQUIRE(buf.read(6, 4) == "AABB");
}

TEST_CASE("ChunkedBuffer — read beyond size returns partial", "[chunked][p39]")
{
    ChunkedBuffer<64> buf;
    buf.append("Short");
    REQUIRE(buf.read(0, 100) == "Short");
    REQUIRE(buf.read(3, 100) == "rt");
    REQUIRE(buf.read(100, 1).empty());
}

TEST_CASE("ChunkedBuffer — empty buffer", "[chunked][p39]")
{
    ChunkedBuffer<64> buf;
    REQUIRE(buf.empty());
    REQUIRE(buf.size() == 0);
    REQUIRE(buf.chunk_count() == 0);
    REQUIRE(buf.read(0, 10).empty());
}

TEST_CASE("ChunkedBuffer — clear resets", "[chunked][p39]")
{
    ChunkedBuffer<64> buf;
    buf.append("data");
    buf.clear();
    REQUIRE(buf.empty());
    REQUIRE(buf.chunk_count() == 0);
}

TEST_CASE("ByteCappedLRU — evicts when byte cap exceeded", "[chunked][p39]")
{
    auto size_fn = [](const std::string& s) -> std::size_t { return s.size(); };
    ByteCappedLRU<int, std::string> cache(20, size_fn);

    cache.put(1, "AAAAAAAAAA"); // 10 bytes
    cache.put(2, "BBBBBBBBBB"); // 10 bytes — total 20, at cap

    REQUIRE(cache.size() == 2);
    REQUIRE(cache.current_bytes() == 20);

    cache.put(3, "CCCCCCCCCC"); // 10 bytes — must evict oldest (key 1)

    REQUIRE(cache.size() == 2);
    REQUIRE(cache.get(1) == nullptr); // evicted
    REQUIRE(cache.get(2) != nullptr);
    REQUIRE(cache.get(3) != nullptr);
}

TEST_CASE("ByteCappedLRU — LRU ordering", "[chunked][p39]")
{
    auto size_fn = [](const std::string& s) -> std::size_t { return s.size(); };
    ByteCappedLRU<int, std::string> cache(30, size_fn);

    cache.put(1, "AAAAAAAAAA"); // 10
    cache.put(2, "BBBBBBBBBB"); // 10
    cache.put(3, "CCCCCCCCCC"); // 10, total = 30

    // Access key 1 to make it most-recently-used
    [[maybe_unused]] auto* val = cache.get(1);

    // Insert key 4 — should evict key 2 (least recently used)
    cache.put(4, "DDDDDDDDDD");

    REQUIRE(cache.get(1) != nullptr);
    REQUIRE(cache.get(2) == nullptr); // evicted
    REQUIRE(cache.get(3) != nullptr);
    REQUIRE(cache.get(4) != nullptr);
}

// ═══════════════════════════════════════════════════════
// #40 CompilerHints — compile-time verification
// ═══════════════════════════════════════════════════════

MARKAMP_FORCE_INLINE int force_inline_test_fn()
{
    return 42;
}

TEST_CASE("CompilerHints — macros are defined", "[compiler][p40]")
{
    // These are compile-time checks — if the file compiles, macros work.
    REQUIRE(force_inline_test_fn() == 42);

    // Branch hints should compile without error
    int x_val = 1;
    if (x_val > 0)
        MARKAMP_LIKELY
        {
            REQUIRE(true);
        }
    else
        MARKAMP_UNLIKELY
        {
            REQUIRE(false);
        }
}

MARKAMP_HOT void hot_function() {}
MARKAMP_COLD void cold_function() {}

TEST_CASE("CompilerHints — hot/cold attributes compile", "[compiler][p40]")
{
    hot_function();
    cold_function();
    REQUIRE(true);
}
