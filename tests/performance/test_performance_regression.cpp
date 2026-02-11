#include "core/HtmlSanitizer.h"
#include "core/MarkdownParser.h"
#include "core/Profiler.h"
#include "rendering/HtmlRenderer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <sstream>
#include <string>

namespace
{

/// Generate realistic mixed-content markdown with the given number of lines.
auto generate_markdown(int line_count) -> std::string
{
    std::ostringstream oss;

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
        else if (mod == 10)
        {
            oss << "```cpp\nint x = " << idx << ";\n```\n\n";
        }
        else if (mod == 15)
        {
            oss << "> A blockquote about line " << idx << ".\n\n";
        }
        else
        {
            oss << "Paragraph text for line " << idx << ". Lorem ipsum dolor sit amet.\n\n";
        }
    }
    return oss.str();
}

/// Measure execution time of a callable in milliseconds.
template <typename Func>
auto measure_ms(Func&& func) -> double
{
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

} // namespace

// ═══════════════════════════════════════════════════════
// Parse Regression Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Regression: Parse 1K lines under 10ms", "[performance][parse]")
{
    auto markdown = generate_markdown(1000);
    markamp::core::MarkdownParser parser;

    // Warm up
    auto warmup = parser.parse(markdown);
    REQUIRE(warmup.has_value());

    // Measure 5 iterations, take the median
    constexpr int kIterations = 5;
    std::array<double, kIterations> times{};
    for (int iter = 0; iter < kIterations; ++iter)
    {
        times.at(static_cast<size_t>(iter)) = measure_ms(
            [&]()
            {
                auto result = parser.parse(markdown);
                (void)result;
            });
    }

    // Sort to get median
    std::sort(times.begin(), times.end());
    const double median_ms = times.at(kIterations / 2);

    INFO("Parse 1K median: " << median_ms << " ms");
    CHECK(median_ms < 8.0); // Tightened: 8ms threshold (Improvement 30)
}

TEST_CASE("Regression: Parse 10K lines under 100ms", "[performance][parse]")
{
    auto markdown = generate_markdown(10000);
    markamp::core::MarkdownParser parser;

    // Warm up
    auto warmup = parser.parse(markdown);
    REQUIRE(warmup.has_value());

    constexpr int kIterations = 5;
    std::array<double, kIterations> times{};
    for (int iter = 0; iter < kIterations; ++iter)
    {
        times.at(static_cast<size_t>(iter)) = measure_ms(
            [&]()
            {
                auto result = parser.parse(markdown);
                (void)result;
            });
    }

    std::sort(times.begin(), times.end());
    const double median_ms = times.at(kIterations / 2);

    INFO("Parse 10K median: " << median_ms << " ms");
    CHECK(median_ms < 80.0); // Tightened: 80ms threshold (Improvement 30)
}

// ═══════════════════════════════════════════════════════
// Render Regression Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("Regression: Render 1K lines under 50ms", "[performance][render]")
{
    auto markdown = generate_markdown(1000);
    markamp::core::MarkdownParser parser;
    auto doc = parser.parse(markdown);
    REQUIRE(doc.has_value());

    markamp::rendering::HtmlRenderer renderer;

    // Warm up
    auto warmup = renderer.render(*doc);
    REQUIRE(!warmup.empty());

    constexpr int kIterations = 5;
    std::array<double, kIterations> times{};
    for (int iter = 0; iter < kIterations; ++iter)
    {
        times.at(static_cast<size_t>(iter)) = measure_ms(
            [&]()
            {
                auto html = renderer.render(*doc);
                (void)html;
            });
    }

    std::sort(times.begin(), times.end());
    const double median_ms = times.at(kIterations / 2);

    INFO("Render 1K median: " << median_ms << " ms");
    CHECK(median_ms < 40.0); // Tightened: 40ms threshold (Improvement 30)
}

// ═══════════════════════════════════════════════════════
// Sanitizer Regression Test (Improvement 30)
// ═══════════════════════════════════════════════════════

TEST_CASE("Regression: Sanitize 1K lines under 20ms", "[performance][sanitizer]")
{
    auto markdown = generate_markdown(1000);
    markamp::core::MarkdownParser parser;
    auto doc = parser.parse(markdown);
    REQUIRE(doc.has_value());

    markamp::rendering::HtmlRenderer renderer;
    auto html = renderer.render(*doc);
    REQUIRE(!html.empty());

    markamp::core::HtmlSanitizer sanitizer;

    // Warm up
    auto warmup = sanitizer.sanitize(html);
    REQUIRE(!warmup.empty());

    constexpr int kIterations = 5;
    std::array<double, kIterations> times{};
    for (int iter = 0; iter < kIterations; ++iter)
    {
        times.at(static_cast<size_t>(iter)) = measure_ms(
            [&]()
            {
                auto result = sanitizer.sanitize(html);
                (void)result;
            });
    }

    std::sort(times.begin(), times.end());
    const double median_ms = times.at(kIterations / 2);

    INFO("Sanitize 1K median: " << median_ms << " ms");
    CHECK(median_ms < 20.0); // 20ms threshold
}

// ═══════════════════════════════════════════════════════
// Profiler Infrastructure Test
// ═══════════════════════════════════════════════════════

TEST_CASE("Regression: Profiler overhead negligible", "[performance][profiler]")
{
    auto& profiler = markamp::core::Profiler::instance();
    profiler.reset();

    constexpr int kIterations = 1000;
    double total_ms = measure_ms(
        [&]()
        {
            for (int iter = 0; iter < kIterations; ++iter)
            {
                auto timer = profiler.scope("overhead_test");
            }
        });

    double per_scope_us = (total_ms * 1000.0) / static_cast<double>(kIterations);

    INFO("Per-scope overhead: " << per_scope_us << " us");
    CHECK(per_scope_us < 100.0); // Under 100 microseconds per scope
}

TEST_CASE("Regression: Memory tracking returns positive value", "[performance][profiler]")
{
    double mem_mb = markamp::core::Profiler::memory_usage_mb();
    INFO("Memory usage: " << mem_mb << " MB");
    CHECK(mem_mb > 0.0);
}

TEST_CASE("Regression: Profiler results are accurate", "[performance][profiler]")
{
    auto& profiler = markamp::core::Profiler::instance();
    profiler.reset();

    // Record known durations
    profiler.record("test_section", 5.0);
    profiler.record("test_section", 10.0);
    profiler.record("test_section", 15.0);

    auto results = profiler.results();
    REQUIRE(results.size() == 1);

    auto& result = results[0];
    CHECK(result.name == "test_section");
    CHECK(result.call_count == 3);
    CHECK(result.avg_ms == Catch::Approx(10.0));
    CHECK(result.min_ms == Catch::Approx(5.0));
    CHECK(result.max_ms == Catch::Approx(15.0));

    profiler.reset();
    auto empty = profiler.results();
    CHECK(empty.empty());
}
