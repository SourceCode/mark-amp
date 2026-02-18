/// @file bench_core.cpp
/// Core benchmark suite (Phase 34D, PRD Task 16).
///
/// Measures throughput of critical subsystems:
/// - EventBus: events/second
/// - Board serialization: serialize/deserialize throughput
/// - Config: read/write throughput
/// - MarkdownParser + HtmlRenderer: paragraphs/second

#include "canvas/Board.h"
#include "canvas/BoardSerializer.h"
#include "canvas/StickyNote.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/MarkdownParser.h"
#include "core/ObservabilityService.h"
#include "rendering/HtmlRenderer.h"

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <sstream>
#include <string>

using namespace markamp;

namespace
{

struct BenchEvent : core::Event
{
    int counter{0};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "BenchEvent";
    }
};

auto generate_markdown(int line_count) -> std::string
{
    std::ostringstream oss;
    for (int idx = 0; idx < line_count; ++idx)
    {
        const int mod = idx % 10;
        if (mod == 0)
        {
            oss << "## Section " << (idx / 10 + 1) << "\n\n";
        }
        else if (mod == 3)
        {
            oss << "- List item " << idx << "\n";
        }
        else if (mod == 6)
        {
            oss << "```cpp\nint x = " << idx << ";\n```\n\n";
        }
        else
        {
            oss << "Paragraph text for line " << idx << ". Lorem ipsum.\n\n";
        }
    }
    return oss.str();
}

} // namespace

// ═══════════════════════════════════════════════════════
// EventBus benchmarks
// ═══════════════════════════════════════════════════════

TEST_CASE("Benchmark: EventBus publish+drain 1000 events", "[benchmark][eventbus]")
{
    core::EventBus bus;
    int received = 0;
    auto sub = bus.subscribe<BenchEvent>([&](const BenchEvent& /*evt*/) { ++received; });

    BENCHMARK("eventbus_1000_publish_drain")
    {
        received = 0;
        for (int idx = 0; idx < 1000; ++idx)
        {
            BenchEvent evt;
            evt.counter = idx;
            bus.publish(evt);
        }
        bus.drain_fast_queue();
        return received;
    };
}

// ═══════════════════════════════════════════════════════
// Board serialization benchmarks
// ═══════════════════════════════════════════════════════

TEST_CASE("Benchmark: Board serialize 50 objects", "[benchmark][board]")
{
    canvas::Board board;
    for (int idx = 0; idx < 50; ++idx)
    {
        auto note = std::make_unique<canvas::StickyNote>();
        note->set_position(static_cast<double>(idx) * 10.0, static_cast<double>(idx) * 10.0);
        note->set_text("Note " + std::to_string(idx));
        board.add_object(std::move(note));
    }

    canvas::BoardSerializer serializer;

    BENCHMARK("serialize_50_objects")
    {
        return serializer.serialize(board);
    };
}

TEST_CASE("Benchmark: Board deserialize 50 objects", "[benchmark][board]")
{
    canvas::Board board;
    for (int idx = 0; idx < 50; ++idx)
    {
        auto note = std::make_unique<canvas::StickyNote>();
        note->set_position(static_cast<double>(idx) * 10.0, static_cast<double>(idx) * 10.0);
        note->set_text("Note " + std::to_string(idx));
        board.add_object(std::move(note));
    }

    canvas::BoardSerializer serializer;
    auto json = serializer.serialize(board);

    BENCHMARK("deserialize_50_objects")
    {
        return serializer.deserialize(json);
    };
}

// ═══════════════════════════════════════════════════════
// Config benchmarks
// ═══════════════════════════════════════════════════════

TEST_CASE("Benchmark: Config 1000 get_int calls", "[benchmark][config]")
{
    core::Config config;
    config.set("bench_key", 42);

    BENCHMARK("config_1000_get_int")
    {
        int sum = 0;
        for (int idx = 0; idx < 1000; ++idx)
        {
            sum += config.get_int("bench_key", 0);
        }
        return sum;
    };
}

// ═══════════════════════════════════════════════════════
// Markdown parse + render benchmarks
// ═══════════════════════════════════════════════════════

TEST_CASE("Benchmark: Parse 100 lines", "[benchmark][parse]")
{
    auto md = generate_markdown(100);
    core::MarkdownParser parser;

    BENCHMARK("parse_100_lines")
    {
        return parser.parse(md);
    };
}

TEST_CASE("Benchmark: Parse 1000 lines", "[benchmark][parse]")
{
    auto md = generate_markdown(1000);
    core::MarkdownParser parser;

    BENCHMARK("parse_1000_lines")
    {
        return parser.parse(md);
    };
}

TEST_CASE("Benchmark: Render 100 lines", "[benchmark][render]")
{
    auto md = generate_markdown(100);
    core::MarkdownParser parser;
    auto doc = parser.parse(md);
    REQUIRE(doc.has_value());

    rendering::HtmlRenderer renderer;

    BENCHMARK("render_100_lines")
    {
        return renderer.render(*doc);
    };
}

TEST_CASE("Benchmark: Render 1000 lines", "[benchmark][render]")
{
    auto md = generate_markdown(1000);
    core::MarkdownParser parser;
    auto doc = parser.parse(md);
    REQUIRE(doc.has_value());

    rendering::HtmlRenderer renderer;

    BENCHMARK("render_1000_lines")
    {
        return renderer.render(*doc);
    };
}

// ═══════════════════════════════════════════════════════
// MetricsCollector benchmarks
// ═══════════════════════════════════════════════════════

TEST_CASE("Benchmark: MetricsCollector 1000 increments", "[benchmark][metrics]")
{
    core::MetricsCollector metrics;

    BENCHMARK("metrics_1000_increment")
    {
        for (int idx = 0; idx < 1000; ++idx)
        {
            metrics.increment("bench_counter");
        }
        return metrics.get("bench_counter").value;
    };
}
