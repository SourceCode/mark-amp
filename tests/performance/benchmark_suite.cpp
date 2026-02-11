#include "core/HtmlSanitizer.h"
#include "core/MarkdownParser.h"
#include "core/Profiler.h"
#include "rendering/HtmlRenderer.h"

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include <string>

namespace
{

/// Generate realistic mixed-content markdown with the given number of lines.
auto generate_markdown(int line_count) -> std::string
{
    std::ostringstream oss;
    oss.str().reserve(static_cast<size_t>(line_count) * 60);

    for (int idx = 0; idx < line_count; ++idx)
    {
        const int mod = idx % 20;
        if (mod == 0)
        {
            oss << "## Section " << (idx / 20 + 1) << "\n\n";
        }
        else if (mod == 1)
        {
            oss << "This is a **bold** and *italic* paragraph with `inline code`.\n\n";
        }
        else if (mod == 5)
        {
            oss << "- List item " << idx << "\n";
        }
        else if (mod == 6)
        {
            oss << "- Another list item with [a link](https://example.com)\n";
        }
        else if (mod == 10)
        {
            oss << "```cpp\nint x = " << idx << ";\n```\n\n";
        }
        else if (mod == 15)
        {
            oss << "> A blockquote with some wisdom about line " << idx << ".\n\n";
        }
        else if (mod == 18)
        {
            oss << "| Col A | Col B | Col C |\n|-------|-------|-------|\n| " << idx
                << " | data | more |\n\n";
        }
        else
        {
            oss << "Regular paragraph text for line " << idx << ". Lorem ipsum dolor sit amet.\n\n";
        }
    }
    return oss.str();
}

} // namespace

// ═══════════════════════════════════════════════════════
// Markdown Parse Benchmarks
// ═══════════════════════════════════════════════════════

TEST_CASE("Benchmark: Markdown parse 100 lines", "[benchmark][parse]")
{
    auto markdown = generate_markdown(100);
    markamp::core::MarkdownParser parser;

    BENCHMARK("parse_100_lines")
    {
        return parser.parse(markdown);
    };
}

TEST_CASE("Benchmark: Markdown parse 1000 lines", "[benchmark][parse]")
{
    auto markdown = generate_markdown(1000);
    markamp::core::MarkdownParser parser;

    BENCHMARK("parse_1000_lines")
    {
        return parser.parse(markdown);
    };
}

TEST_CASE("Benchmark: Markdown parse 10000 lines", "[benchmark][parse]")
{
    auto markdown = generate_markdown(10000);
    markamp::core::MarkdownParser parser;

    BENCHMARK("parse_10000_lines")
    {
        return parser.parse(markdown);
    };
}

// ═══════════════════════════════════════════════════════
// HTML Render Benchmarks
// ═══════════════════════════════════════════════════════

TEST_CASE("Benchmark: HTML render 100 lines", "[benchmark][render]")
{
    auto markdown = generate_markdown(100);
    markamp::core::MarkdownParser parser;
    auto doc = parser.parse(markdown);
    REQUIRE(doc.has_value());

    markamp::rendering::HtmlRenderer renderer;

    BENCHMARK("render_100_lines")
    {
        return renderer.render(*doc);
    };
}

TEST_CASE("Benchmark: HTML render 1000 lines", "[benchmark][render]")
{
    auto markdown = generate_markdown(1000);
    markamp::core::MarkdownParser parser;
    auto doc = parser.parse(markdown);
    REQUIRE(doc.has_value());

    markamp::rendering::HtmlRenderer renderer;

    BENCHMARK("render_1000_lines")
    {
        return renderer.render(*doc);
    };
}

TEST_CASE("Benchmark: HTML render 10000 lines", "[benchmark][render]")
{
    auto markdown = generate_markdown(10000);
    markamp::core::MarkdownParser parser;
    auto doc = parser.parse(markdown);
    REQUIRE(doc.has_value());

    markamp::rendering::HtmlRenderer renderer;

    BENCHMARK("render_10000_lines")
    {
        return renderer.render(*doc);
    };
}

// ═══════════════════════════════════════════════════════
// Profiler Benchmarks
// ═══════════════════════════════════════════════════════

TEST_CASE("Benchmark: Profiler overhead", "[benchmark][profiler]")
{
    auto& profiler = markamp::core::Profiler::instance();
    profiler.reset();

    BENCHMARK("profiler_scope_overhead")
    {
        auto timer = profiler.scope("bench_scope");
        return 42; // trivial work
    };
}

TEST_CASE("Benchmark: Profiler memory_usage_mb", "[benchmark][profiler]")
{
    BENCHMARK("memory_usage_mb")
    {
        return markamp::core::Profiler::memory_usage_mb();
    };
}

// ═══════════════════════════════════════════════════════
// Sanitizer Benchmarks (Improvement 29)
// ═══════════════════════════════════════════════════════

TEST_CASE("Benchmark: HtmlSanitizer 1K lines", "[benchmark][sanitizer]")
{
    auto markdown = generate_markdown(1000);
    markamp::core::MarkdownParser parser;
    auto doc = parser.parse(markdown);
    REQUIRE(doc.has_value());

    markamp::rendering::HtmlRenderer renderer;
    auto html = renderer.render(*doc);
    REQUIRE(!html.empty());

    markamp::core::HtmlSanitizer sanitizer;

    BENCHMARK("sanitize_1000_lines")
    {
        return sanitizer.sanitize(html);
    };
}

// ═══════════════════════════════════════════════════════
// Footnote Preprocessor Benchmarks (Improvement 29)
// ═══════════════════════════════════════════════════════

TEST_CASE("Benchmark: FootnotePreprocessor 1K lines", "[benchmark][footnote]")
{
    // Add footnotes to the markdown
    std::string markdown = generate_markdown(1000);
    markdown += "\nSee reference[^1] and another[^2].\n\n";
    markdown += "[^1]: First footnote definition.\n";
    markdown += "[^2]: Second footnote definition.\n";

    markamp::rendering::FootnotePreprocessor processor;

    BENCHMARK("footnote_1000_lines")
    {
        return processor.process(markdown);
    };
}
