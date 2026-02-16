/// bench_html_renderer.cpp — HtmlRenderer benchmarks
///
/// Phase 09: measures HTML rendering from a pre-parsed MarkdownDocument AST.

#include "core/MarkdownParser.h"
#include "rendering/HtmlRenderer.h"

#include <benchmark/benchmark.h>

namespace
{

/// Generate a synthetic markdown document with the given paragraph count.
auto generate_markdown(int paragraphs) -> std::string
{
    std::string md;
    md.reserve(static_cast<size_t>(paragraphs) * 200);

    md += "# Benchmark Document\n\n";

    for (int idx = 0; idx < paragraphs; ++idx)
    {
        if (idx % 10 == 0)
        {
            md += "## Section " + std::to_string(idx / 10 + 1) + "\n\n";
        }

        md += "Paragraph " + std::to_string(idx + 1) +
              " with **bold**, *italic*, `code`, and [link](https://example.com). "
              "Some extra text for realistic content measurement.\n\n";

        if (idx % 5 == 0)
        {
            md += "- Item A\n- Item B\n- Item C\n\n";
        }
        if (idx % 7 == 0)
        {
            md += "> Blockquote with *emphasis* and **strong text**.\n\n";
        }
        if (idx % 8 == 0)
        {
            md += "```python\ndef func():\n    return 42\n```\n\n";
        }
    }

    return md;
}

/// BM_HtmlRenderer_Render: render a pre-parsed AST to HTML
void BM_HtmlRenderer_Render(benchmark::State& state)
{
    auto paragraphs = static_cast<int>(state.range(0));
    auto markdown = generate_markdown(paragraphs);

    markamp::core::MarkdownParser parser;
    auto doc = parser.parse(markdown);

    if (!doc.has_value())
    {
        state.SkipWithError("Failed to parse markdown");
        return;
    }

    markamp::rendering::HtmlRenderer renderer;

    for (auto _ : state)
    {
        auto html = renderer.render(doc.value());
        benchmark::DoNotOptimize(html);
    }

    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(markdown.size()));
}
BENCHMARK(BM_HtmlRenderer_Render)->Arg(10)->Arg(100)->Arg(500);

/// BM_HtmlRenderer_FullPipeline: parse + render end-to-end
void BM_HtmlRenderer_FullPipeline(benchmark::State& state)
{
    auto paragraphs = static_cast<int>(state.range(0));
    auto markdown = generate_markdown(paragraphs);

    markamp::core::MarkdownParser parser;
    markamp::rendering::HtmlRenderer renderer;

    for (auto _ : state)
    {
        auto doc = parser.parse(markdown);
        if (doc.has_value())
        {
            auto html = renderer.render(doc.value());
            benchmark::DoNotOptimize(html);
        }
    }

    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(markdown.size()));
}
BENCHMARK(BM_HtmlRenderer_FullPipeline)->Arg(10)->Arg(100)->Arg(500);

/// BM_HtmlRenderer_EscapeHtml: static HTML escaping throughput
void BM_HtmlRenderer_EscapeHtml(benchmark::State& state)
{
    std::string input =
        "<div class=\"test\">&amp; O'Brien's \"quote\" <script>alert(1)</script></div>";

    for (auto _ : state)
    {
        auto escaped = markamp::rendering::HtmlRenderer::escape_html(input);
        benchmark::DoNotOptimize(escaped);
    }

    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(input.size()));
}
BENCHMARK(BM_HtmlRenderer_EscapeHtml);

/// BM_HtmlRenderer_Slugify: heading slug generation
void BM_HtmlRenderer_Slugify(benchmark::State& state)
{
    std::string heading = "This Is a Complex Heading with ÜTF-8 Characters & Symbols!";

    for (auto _ : state)
    {
        auto slug = markamp::rendering::HtmlRenderer::slugify(heading);
        benchmark::DoNotOptimize(slug);
    }
}
BENCHMARK(BM_HtmlRenderer_Slugify);

} // namespace
