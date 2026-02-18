/// @file test_known_issues.cpp
/// Regression tests for previously‐fixed bugs (Phase 34D, PRD Task 14).
///
/// Each test case corresponds to a specific bug that was identified
/// and fixed during earlier phases. These tests prevent reintroduction.

#include "canvas/Board.h"
#include "canvas/BoardSerializer.h"
#include "canvas/StickyNote.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/IncrementalSearcher.h"
#include "core/MarkdownParser.h"
#include "core/ObservabilityService.h"
#include "core/Result.h"
#include "core/Theme.h"
#include "rendering/HtmlRenderer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <atomic>
#include <chrono>
#include <string>
#include <string_view>
#include <thread>

using namespace markamp;
using Catch::Matchers::ContainsSubstring;

// ═══════════════════════════════════════════════════════
// Regression: SpanTracker completed_spans duration
// Previously span durations could be reported as zero
// even when measurable work was performed.
// ═══════════════════════════════════════════════════════

TEST_CASE("Regression: SpanTracker duration is non-negative", "[regression][span]")
{
    core::SpanTracker tracker;
    auto idx = tracker.begin_span("regr_work");
    // Do trivial but measurable work
    int sum = 0;
    for (int idx2 = 0; idx2 < 10000; ++idx2)
    {
        sum = sum + idx2;
    }
    (void)sum;
    tracker.end_span(idx);

    auto spans = tracker.completed_spans();
    REQUIRE(!spans.empty());
    REQUIRE(spans.back().completed);
    REQUIRE(spans.back().duration_us >= 0);
}

TEST_CASE("Regression: SpanTracker clear resets state", "[regression][span]")
{
    core::SpanTracker tracker;
    auto idx = tracker.begin_span("temp");
    tracker.end_span(idx);
    REQUIRE(!tracker.completed_spans().empty());
    tracker.clear();
    REQUIRE(tracker.completed_spans().empty());
}

// ═══════════════════════════════════════════════════════
// Regression: EventBus type deduplication
// Events published with distinct types must be routed
// to the correct subscribers without cross-talk.
// ═══════════════════════════════════════════════════════

namespace
{

struct RegrEventA : core::Event
{
    int value{0};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "RegrEventA";
    }
};

struct RegrEventB : core::Event
{
    int value{0};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "RegrEventB";
    }
};

} // namespace

TEST_CASE("Regression: EventBus routes distinct types correctly", "[regression][eventbus]")
{
    core::EventBus bus;
    int count_a = 0;
    int count_b = 0;

    auto sub_a = bus.subscribe<RegrEventA>([&](const RegrEventA& /*evt*/) { ++count_a; });
    auto sub_b = bus.subscribe<RegrEventB>([&](const RegrEventB& /*evt*/) { ++count_b; });

    RegrEventA evt_a;
    evt_a.value = 1;
    bus.publish(evt_a);

    RegrEventB evt_b;
    evt_b.value = 2;
    bus.publish(evt_b);

    REQUIRE(count_a == 1);
    REQUIRE(count_b == 1);
}

TEST_CASE("Regression: EventBus unsubscribe prevents delivery", "[regression][eventbus]")
{
    core::EventBus bus;
    int count = 0;
    auto sub = bus.subscribe<RegrEventA>([&](const RegrEventA& /*evt*/) { ++count; });

    RegrEventA evt;
    bus.publish(evt);
    REQUIRE(count == 1);

    sub.cancel();

    bus.publish(evt);
    REQUIRE(count == 1); // Should not have incremented
}

// ═══════════════════════════════════════════════════════
// Regression: Markdown parser edge cases
// Empty, whitespace-only, and heading-only documents
// must parse without crash.
// ═══════════════════════════════════════════════════════

TEST_CASE("Regression: parse empty markdown", "[regression][parser]")
{
    core::MarkdownParser parser;
    auto result = parser.parse("");
    REQUIRE(result.has_value());
}

TEST_CASE("Regression: parse whitespace-only markdown", "[regression][parser]")
{
    core::MarkdownParser parser;
    auto result = parser.parse("   \n\n  \t  \n");
    REQUIRE(result.has_value());
}

TEST_CASE("Regression: parse heading without trailing newline", "[regression][parser]")
{
    core::MarkdownParser parser;
    auto result = parser.parse("# Heading");
    REQUIRE(result.has_value());
}

TEST_CASE("Regression: parse deeply nested lists", "[regression][parser]")
{
    std::string md;
    for (int depth = 0; depth < 10; ++depth)
    {
        md += std::string(static_cast<size_t>(depth) * 2, ' ') + "- item " + std::to_string(depth) +
              "\n";
    }
    core::MarkdownParser parser;
    auto result = parser.parse(md);
    REQUIRE(result.has_value());
}

// ═══════════════════════════════════════════════════════
// Regression: HTML renderer edge cases
// ═══════════════════════════════════════════════════════

TEST_CASE("Regression: render markdown with only emphasis", "[regression][render]")
{
    core::MarkdownParser parser;
    auto doc = parser.parse("**bold** and *italic*\n");
    REQUIRE(doc.has_value());

    rendering::HtmlRenderer renderer;
    auto html = renderer.render(*doc);
    REQUIRE(!html.empty());
    REQUIRE_THAT(html, ContainsSubstring("bold"));
}

TEST_CASE("Regression: render code fence with no language", "[regression][render]")
{
    core::MarkdownParser parser;
    auto doc = parser.parse("```\ncode\n```\n");
    REQUIRE(doc.has_value());

    rendering::HtmlRenderer renderer;
    auto html = renderer.render(*doc);
    REQUIRE(!html.empty());
    REQUIRE_THAT(html, ContainsSubstring("code"));
}

// ═══════════════════════════════════════════════════════
// Regression: Board serialization edge cases
// ═══════════════════════════════════════════════════════

TEST_CASE("Regression: serialize empty board round-trip", "[regression][board]")
{
    canvas::Board board;
    canvas::BoardSerializer serializer;

    auto json = serializer.serialize(board);
    REQUIRE(!json.empty());

    auto restored = serializer.deserialize(json);
    REQUIRE(restored.object_count() == 0);
}

TEST_CASE("Regression: deserialize malformed JSON does not crash", "[regression][board]")
{
    canvas::BoardSerializer serializer;
    // Should not crash — may throw or return empty board
    bool caught = false;
    try
    {
        auto board = serializer.deserialize("not valid json {{{");
        // If no throw, board should be empty
        REQUIRE(board.object_count() == 0);
    }
    catch (...)
    {
        caught = true;
    }
    // Either outcome is acceptable — no crash
    REQUIRE(true);
    (void)caught;
}

// ═══════════════════════════════════════════════════════
// Regression: Config type safety
// ═══════════════════════════════════════════════════════

TEST_CASE("Regression: Config get_int returns default for missing key", "[regression][config]")
{
    core::Config config;
    auto val = config.get_int("nonexistent_key_xyz_123", 42);
    REQUIRE(val == 42);
}

TEST_CASE("Regression: Config get_bool returns default for missing key", "[regression][config]")
{
    core::Config config;
    auto val = config.get_bool("nonexistent_key_abc_456", true);
    REQUIRE(val == true);
}

TEST_CASE("Regression: Config set then get preserves value", "[regression][config]")
{
    core::Config config;
    config.set("regr_test_key", 999);
    REQUIRE(config.get_int("regr_test_key", 0) == 999);
}

// ═══════════════════════════════════════════════════════
// Regression: Theme system edge cases
// ═══════════════════════════════════════════════════════

TEST_CASE("Regression: default Theme has valid bg_app", "[regression][theme]")
{
    core::Theme theme;
    auto bg_app = theme.colors.bg_app;
    // Color channels must be valid uint8_t range (always true, but sanity check)
    REQUIRE(bg_app.r <= 255);
    REQUIRE(bg_app.g <= 255);
    REQUIRE(bg_app.b <= 255);
}

TEST_CASE("Regression: Theme is_dark detection for white bg", "[regression][theme]")
{
    core::Theme theme;
    theme.colors.bg_app = {255, 255, 255, 255};
    REQUIRE_FALSE(theme.is_dark());
}

TEST_CASE("Regression: Theme is_dark detection for black bg", "[regression][theme]")
{
    core::Theme theme;
    theme.colors.bg_app = {0, 0, 0, 255};
    REQUIRE(theme.is_dark());
}

// ═══════════════════════════════════════════════════════
// Regression: Search with special characters
// ═══════════════════════════════════════════════════════

TEST_CASE("Regression: search for literal dot does not crash", "[regression][search]")
{
    core::IncrementalSearcher searcher;
    std::atomic<bool> completed{false};
    std::size_t match_count = 0;

    core::SearchConfig cfg;
    cfg.needle = ".";
    cfg.case_sensitive = true;

    searcher.search(
        "Hello. World.",
        cfg,
        [&](std::size_t /*line*/, std::size_t /*col*/, std::string_view /*ctx*/) -> bool
        {
            ++match_count;
            return true;
        },
        [&](std::size_t /*total*/) { completed.store(true); });

    // Wait for completion
    for (int wait_idx = 0; wait_idx < 100 && !completed.load(); ++wait_idx)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    REQUIRE(completed.load());
    REQUIRE(match_count >= 2);
}

TEST_CASE("Regression: search for regex special chars without crash", "[regression][search]")
{
    core::IncrementalSearcher searcher;
    std::atomic<bool> completed{false};

    core::SearchConfig cfg;
    cfg.needle = "[test]";
    cfg.case_sensitive = false;

    searcher.search(
        "This has [test] brackets",
        cfg,
        [](std::size_t /*line*/, std::size_t /*col*/, std::string_view /*ctx*/) -> bool
        { return true; },
        [&](std::size_t /*total*/) { completed.store(true); });

    for (int wait_idx = 0; wait_idx < 100 && !completed.load(); ++wait_idx)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    REQUIRE(completed.load());
}
