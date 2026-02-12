/// test_performance_infra.cpp
/// Unit tests for all performance infrastructure primitives
/// Patterns #1–#20 from sublime_performance_patterns.md

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>

// ── Core primitives ──
// Note: CoalescingTask.h is excluded from this test because it requires
// <stop_token> which is C++20/C++23 and may not be available in all
// clang analysis modes.
#include "core/DocumentSnapshot.h"
#include "core/FrameArena.h"
#include "core/FrameScheduler.h"
#include "core/LineIndex.h"
#include "core/PieceTable.h"
#include "core/Profiler.h"
#include "core/SPSCQueue.h"
#include "core/SyntaxHighlighter.h"

// ── Rendering ──
#include "rendering/DirtyRegion.h"
#include "rendering/ViewportCache.h"

using namespace markamp::core;
using namespace markamp::rendering;

// ═══════════════════════════════════════════════════════
// SPSCQueue tests
// ═══════════════════════════════════════════════════════

TEST_CASE("SPSCQueue — push and pop", "[spsc]")
{
    SPSCQueue<int, 16> queue;

    SECTION("empty queue returns false on pop")
    {
        int val = 0;
        REQUIRE_FALSE(queue.try_pop(val));
    }

    SECTION("single push-pop round trip")
    {
        REQUIRE(queue.try_push(42));
        int val = 0;
        REQUIRE(queue.try_pop(val));
        REQUIRE(val == 42);
    }

    SECTION("FIFO ordering preserved")
    {
        for (int idx = 0; idx < 10; ++idx)
        {
            REQUIRE(queue.try_push(idx));
        }
        for (int idx = 0; idx < 10; ++idx)
        {
            int val = -1;
            REQUIRE(queue.try_pop(val));
            REQUIRE(val == idx);
        }
    }

    SECTION("full queue returns false on push")
    {
        for (int idx = 0; idx < 16; ++idx)
        {
            [[maybe_unused]] auto pushed = queue.try_push(idx);
        }
        REQUIRE_FALSE(queue.try_push(99));
    }
}

// ═══════════════════════════════════════════════════════
// FrameArena tests
// ═══════════════════════════════════════════════════════

TEST_CASE("FrameArena — allocation and reset", "[arena]")
{
    FrameArena arena;

    SECTION("allocator returns a valid allocator")
    {
        auto alloc = arena.allocator();
        // Basic sanity: can create a pmr vector with this allocator
        auto vec = arena.make_vector<int>();
        REQUIRE(vec.empty());
    }

    SECTION("reset does not throw")
    {
        REQUIRE_NOTHROW(arena.reset());
    }
}

TEST_CASE("ObjectPool — recycle fixed-size objects", "[pool]")
{
    ObjectPool<int> pool;

    SECTION("create returns a valid pointer")
    {
        auto* ptr = pool.create(42);
        REQUIRE(ptr != nullptr);
        REQUIRE(*ptr == 42);
        pool.destroy(ptr);
    }

    SECTION("active_count tracks allocations")
    {
        auto* ptr1 = pool.create(1);
        auto* ptr2 = pool.create(2);
        REQUIRE(pool.active_count() == 2);
        pool.destroy(ptr1);
        REQUIRE(pool.active_count() == 1);
        pool.destroy(ptr2);
        REQUIRE(pool.active_count() == 0);
    }
}

// ═══════════════════════════════════════════════════════
// FrameScheduler tests
// ═══════════════════════════════════════════════════════

TEST_CASE("FrameScheduler — priority task ordering", "[scheduler]")
{
    FrameScheduler scheduler;
    std::vector<int> execution_order;

    scheduler.enqueue(ScheduledTask{.priority = TaskPriority::Background,
                                    .execute = [&]
                                    {
                                        execution_order.push_back(3);
                                        return false;
                                    }});
    scheduler.enqueue(ScheduledTask{.priority = TaskPriority::Input,
                                    .execute = [&]
                                    {
                                        execution_order.push_back(1);
                                        return false;
                                    }});
    scheduler.enqueue(ScheduledTask{.priority = TaskPriority::Paint,
                                    .execute = [&]
                                    {
                                        execution_order.push_back(2);
                                        return false;
                                    }});

    scheduler.run_frame(std::chrono::microseconds(100000)); // generous budget

    REQUIRE(execution_order.size() == 3);
    REQUIRE(execution_order[0] == 1); // Input first
    REQUIRE(execution_order[1] == 2); // Paint second
    REQUIRE(execution_order[2] == 3); // Background last
}

// ═══════════════════════════════════════════════════════
// PieceTable tests
// ═══════════════════════════════════════════════════════

TEST_CASE("PieceTable — basic operations", "[piecetable]")
{
    PieceTable table("Hello World");

    SECTION("initial text is preserved")
    {
        REQUIRE(table.text() == "Hello World");
        REQUIRE(table.size() == 11);
    }

    SECTION("insert at position")
    {
        table.insert(5, ", Beautiful");
        REQUIRE(table.text() == "Hello, Beautiful World");
    }

    SECTION("erase characters")
    {
        table.erase(5, 6); // erase " World"
        REQUIRE(table.text() == "Hello");
    }

    SECTION("insert at beginning")
    {
        table.insert(0, ">> ");
        REQUIRE(table.text() == ">> Hello World");
    }

    SECTION("insert at end")
    {
        table.insert(11, "!");
        REQUIRE(table.text() == "Hello World!");
    }
}

// ═══════════════════════════════════════════════════════
// LineIndex tests
// ═══════════════════════════════════════════════════════

TEST_CASE("LineIndex — offset to line/column", "[lineindex]")
{
    LineIndex index;
    index.rebuild("Hello\nWorld\nFoo\n");

    SECTION("line count is correct")
    {
        REQUIRE(index.line_count() == 4); // 3 newlines → 4 lines
    }

    SECTION("offset to line/column")
    {
        auto [line, col] = index.offset_to_line_col(0);
        REQUIRE(line == 0);
        REQUIRE(col == 0);

        auto [line2, col2] = index.offset_to_line_col(6); // 'W' in "World"
        REQUIRE(line2 == 1);
        REQUIRE(col2 == 0);
    }

    SECTION("line to offset")
    {
        REQUIRE(index.line_start(0) == 0);
        REQUIRE(index.line_start(1) == 6);
        REQUIRE(index.line_start(2) == 12);
    }
}

// ═══════════════════════════════════════════════════════
// DocumentSnapshot tests
// ═══════════════════════════════════════════════════════

TEST_CASE("SnapshotStore — publish and consume", "[snapshot]")
{
    SnapshotStore store;

    SECTION("initial state is empty")
    {
        auto snap = store.current();
        REQUIRE(snap == nullptr);
    }

    SECTION("publish makes snapshot available")
    {
        auto content = std::make_shared<const std::string>("Hello");
        store.publish(DocumentSnapshot{.version = 1, .content = content});
        auto snap = store.current();
        REQUIRE(snap != nullptr);
        REQUIRE(*snap->content == "Hello");
        REQUIRE(snap->version == 1);
    }

    SECTION("newer version replaces older")
    {
        auto v1content = std::make_shared<const std::string>("v1");
        auto v2content = std::make_shared<const std::string>("v2");
        store.publish(DocumentSnapshot{.version = 1, .content = v1content});
        store.publish(DocumentSnapshot{.version = 2, .content = v2content});
        auto snap = store.current();
        REQUIRE(*snap->content == "v2");
        REQUIRE(snap->version == 2);
    }
}

// ═══════════════════════════════════════════════════════
// FrameHistogram tests
// ═══════════════════════════════════════════════════════

TEST_CASE("FrameHistogram — recording and percentile", "[histogram]")
{
    FrameHistogram hist;

    SECTION("empty histogram returns 0 percentile")
    {
        REQUIRE(hist.percentile(0.5) == Catch::Approx(0.0));
        REQUIRE(hist.total_count() == 0);
    }

    SECTION("single record")
    {
        hist.record(5.0); // 5ms bucket
        REQUIRE(hist.total_count() == 1);
        REQUIRE(hist.percentile(1.0) == Catch::Approx(5.0));
    }

    SECTION("multiple records return correct percentile")
    {
        for (int idx = 0; idx < 100; ++idx)
        {
            hist.record(static_cast<double>(idx % 10));
        }
        REQUIRE(hist.total_count() == 100);
        // p50 should be around 4-5ms
        double p50 = hist.percentile(0.5);
        REQUIRE(p50 >= 4.0);
        REQUIRE(p50 <= 5.0);
    }

    SECTION("reset clears all buckets")
    {
        hist.record(1.0);
        hist.record(2.0);
        hist.reset();
        REQUIRE(hist.total_count() == 0);
    }
}

// ═══════════════════════════════════════════════════════
// PerformanceBudget constants tests
// ═══════════════════════════════════════════════════════

TEST_CASE("PerformanceBudget — constants are sane", "[budget]")
{
    REQUIRE(budget::kKeystroke.count() > 0);
    REQUIRE(budget::kScroll.count() > 0);
    REQUIRE(budget::kPreviewUpdate.count() > 0);
    REQUIRE(budget::kFileOpen.count() > 0);
    REQUIRE(budget::kSearchIteration.count() > 0);

    // Keystroke should be tighter than scroll
    REQUIRE(budget::kKeystroke < budget::kScroll);
    // Scroll should be tighter than preview
    REQUIRE(budget::kScroll < budget::kPreviewUpdate);
}

// ═══════════════════════════════════════════════════════
// TokenArraySoA tests
// ═══════════════════════════════════════════════════════

TEST_CASE("TokenArraySoA — SoA token storage", "[soa]")
{
    SECTION("push_back and size")
    {
        TokenArraySoA soa;
        soa.push_back(TokenType::Keyword, 0, 5);
        soa.push_back(TokenType::Text, 5, 3);
        REQUIRE(soa.size() == 2);
        REQUIRE(soa.types[0] == TokenType::Keyword);
        REQUIRE(soa.starts[0] == 0);
        REQUIRE(soa.lengths[0] == 5);
    }

    SECTION("clear empties all arrays")
    {
        TokenArraySoA soa;
        soa.push_back(TokenType::String, 0, 10);
        soa.clear();
        REQUIRE(soa.empty());
    }

    SECTION("tokenize_soa produces SoA from tokenize")
    {
        SyntaxHighlighter highlighter;
        auto soa = highlighter.tokenize_soa("int x = 5;", "cpp");
        REQUIRE(soa.size() > 0);
        // "int" should be classified as a Type in the C++ language def
        REQUIRE(soa.types[0] == TokenType::Type);
    }
}

// ═══════════════════════════════════════════════════════
// DirtyRegion tests
// ═══════════════════════════════════════════════════════

TEST_CASE("DirtyRegionAccumulator — rect management", "[dirty]")
{
    DirtyRegionAccumulator acc;

    SECTION("starts clean")
    {
        REQUIRE_FALSE(acc.has_dirty());
        REQUIRE(acc.count() == 0);
    }

    SECTION("single invalidate")
    {
        acc.invalidate({0, 0, 100, 50});
        REQUIRE(acc.has_dirty());
        REQUIRE(acc.count() == 1);
    }

    SECTION("nearby rects are merged")
    {
        acc.invalidate({0, 0, 100, 50});
        acc.invalidate({90, 40, 200, 100}); // overlaps
        REQUIRE(acc.count() == 1);
    }

    SECTION("distant rects remain separate")
    {
        acc.invalidate({0, 0, 10, 10});
        acc.invalidate({500, 500, 510, 510}); // far away
        REQUIRE(acc.count() == 2);
    }

    SECTION("consume returns and clears")
    {
        acc.invalidate({0, 0, 100, 100});
        auto rects = acc.consume();
        REQUIRE(rects.size() == 1);
        REQUIRE_FALSE(acc.has_dirty());
    }
}

TEST_CASE("Rect — geometry operations", "[rect]")
{
    Rect rect_a{0, 0, 100, 100};
    Rect rect_b{50, 50, 150, 150};

    SECTION("width and height")
    {
        REQUIRE(rect_a.width() == 100);
        REQUIRE(rect_a.height() == 100);
    }

    SECTION("intersection detection")
    {
        REQUIRE(rect_a.intersects(rect_b));
        Rect rect_c{200, 200, 300, 300};
        REQUIRE_FALSE(rect_a.intersects(rect_c));
    }

    SECTION("union combines rects")
    {
        auto combined = rect_a.united(rect_b);
        REQUIRE(combined.left == 0);
        REQUIRE(combined.top == 0);
        REQUIRE(combined.right == 150);
        REQUIRE(combined.bottom == 150);
    }
}

// ═══════════════════════════════════════════════════════
// ViewportCache (LRUCache) tests
// ═══════════════════════════════════════════════════════

TEST_CASE("LRUCache — basic operations", "[lru]")
{
    LRUCache<int, std::string, 4> cache;

    SECTION("miss returns nullopt")
    {
        REQUIRE_FALSE(cache.get(1).has_value());
    }

    SECTION("put and get round-trip")
    {
        cache.put(1, "one");
        auto result = cache.get(1);
        REQUIRE(result.has_value());
        REQUIRE(result->get() == "one");
    }

    SECTION("LRU eviction at capacity")
    {
        cache.put(1, "one");
        cache.put(2, "two");
        cache.put(3, "three");
        cache.put(4, "four");
        cache.put(5, "five"); // should evict 1

        REQUIRE_FALSE(cache.get(1).has_value());
        REQUIRE(cache.get(5).has_value());
    }

    SECTION("accessing promotes to MRU")
    {
        cache.put(1, "one");
        cache.put(2, "two");
        cache.put(3, "three");
        cache.put(4, "four");

        // Access 1 to promote it
        [[maybe_unused]] auto promoted = cache.get(1);
        // Now insert 5 — should evict 2 (not 1)
        cache.put(5, "five");

        REQUIRE(cache.get(1).has_value());       // promoted, still present
        REQUIRE_FALSE(cache.get(2).has_value()); // evicted
    }
}

// ═══════════════════════════════════════════════════════
// ViewportState tests
// ═══════════════════════════════════════════════════════

TEST_CASE("ViewportState — render range with prefetch", "[viewport]")
{
    ViewportState viewport;
    viewport.first_visible_line = 50;
    viewport.visible_line_count = 30;
    viewport.prefetch_margin = 10;

    constexpr std::size_t total_lines = 1000;

    REQUIRE(viewport.render_start(total_lines) == 40);
    REQUIRE(viewport.render_end(total_lines) == 90);
}

TEST_CASE("ViewportState — clamps to bounds", "[viewport]")
{
    ViewportState viewport;
    viewport.first_visible_line = 5;
    viewport.visible_line_count = 30;
    viewport.prefetch_margin = 10;

    // render_start should clamp to 0 (5 - 10 would be negative)
    REQUIRE(viewport.render_start(100) == 0);

    // render_end near end of document
    viewport.first_visible_line = 90;
    viewport.visible_line_count = 30;
    REQUIRE(viewport.render_end(100) == 100); // clamped to total
}

// ═══════════════════════════════════════════════════════
// LazyCache tests
// ═══════════════════════════════════════════════════════

TEST_CASE("LazyCache — deferred computation", "[lazy]")
{
    LazyCache<int> cache;
    int compute_count = 0;

    SECTION("computes on first access")
    {
        auto& val = cache.get(
            [&]
            {
                ++compute_count;
                return 42;
            });
        REQUIRE(val == 42);
        REQUIRE(compute_count == 1);
    }

    SECTION("does not recompute if valid")
    {
        [[maybe_unused]] const auto& val1 = cache.get(
            [&]
            {
                ++compute_count;
                return 10;
            });
        [[maybe_unused]] const auto& val2 = cache.get(
            [&]
            {
                ++compute_count;
                return 20;
            });
        REQUIRE(compute_count == 1); // only computed once
    }

    SECTION("recomputes after invalidation")
    {
        [[maybe_unused]] const auto& val1 = cache.get(
            [&]
            {
                ++compute_count;
                return 10;
            });
        cache.invalidate();
        [[maybe_unused]] const auto& val2 = cache.get(
            [&]
            {
                ++compute_count;
                return 20;
            });
        REQUIRE(compute_count == 2);
    }
}

// ═══════════════════════════════════════════════════════
// LineLayoutCache tests
// ═══════════════════════════════════════════════════════

TEST_CASE("LineLayoutCache — per-line measurement", "[linelayout]")
{
    LineLayoutCache cache;

    SECTION("miss returns nullopt")
    {
        auto result = cache.get(0);
        REQUIRE(!result);
    }

    SECTION("set and get round-trip")
    {
        cache.set(5, LineMeasurement{20, 16, 1, 12345});
        auto result = cache.get(5);
        REQUIRE(static_cast<bool>(result));
        const auto& meas = result->get();
        REQUIRE(meas.height_px == 20);
        REQUIRE(meas.content_hash == 12345);
    }

    SECTION("invalidate_range clears specific lines")
    {
        cache.set(0, LineMeasurement{20, 16, 1, 0});
        cache.set(1, LineMeasurement{20, 16, 1, 0});
        cache.set(2, LineMeasurement{20, 16, 1, 0});

        cache.invalidate_range(1, 2);
        REQUIRE(static_cast<bool>(cache.get(0)));
        REQUIRE(!cache.get(1));
        REQUIRE(static_cast<bool>(cache.get(2)));
    }
}
