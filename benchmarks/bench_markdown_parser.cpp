/// bench_markdown_parser.cpp — MarkdownParser benchmarks
///
/// Phase 09: measures parse() and render_html() for various document sizes.

#include "core/MarkdownParser.h"

#include <benchmark/benchmark.h>
#include <string>

namespace
{

/// Generate a synthetic markdown document with the given number of paragraphs.
auto generate_markdown(int paragraphs) -> std::string
{
    std::string md;
    md.reserve(static_cast<size_t>(paragraphs) * 200);

    md += "# Test Document\n\n";
    md += "A benchmark document with headings, lists, and code blocks.\n\n";

    for (int idx = 0; idx < paragraphs; ++idx)
    {
        if (idx % 10 == 0)
        {
            md += "## Section " + std::to_string(idx / 10 + 1) + "\n\n";
        }

        md += "This is paragraph " + std::to_string(idx + 1) +
              " with **bold text**, *italic*, and `inline code`. "
              "It contains [a link](https://example.com) and some normal text to "
              "simulate a realistic document.\n\n";

        if (idx % 5 == 0)
        {
            md += "- List item one\n- List item two\n- List item three\n\n";
        }

        if (idx % 8 == 0)
        {
            md += "```cpp\nint main() {\n    return 0;\n}\n```\n\n";
        }
    }

    return md;
}

/// BM_MarkdownParser_Parse: parse markdown to AST
void BM_MarkdownParser_Parse(benchmark::State& state)
{
    auto paragraphs = static_cast<int>(state.range(0));
    auto markdown = generate_markdown(paragraphs);
    markamp::core::MarkdownParser parser;

    for (auto _ : state)
    {
        auto result = parser.parse(markdown);
        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(markdown.size()));
}
BENCHMARK(BM_MarkdownParser_Parse)->Arg(10)->Arg(100)->Arg(500);

/// BM_MarkdownParser_RenderHtml: parse + render to HTML
void BM_MarkdownParser_RenderHtml(benchmark::State& state)
{
    auto paragraphs = static_cast<int>(state.range(0));
    auto markdown = generate_markdown(paragraphs);
    markamp::core::MarkdownParser parser;

    for (auto _ : state)
    {
        auto doc = parser.parse(markdown);
        if (doc.has_value())
        {
            auto html = parser.render_html(doc.value());
            benchmark::DoNotOptimize(html);
        }
    }

    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(markdown.size()));
}
BENCHMARK(BM_MarkdownParser_RenderHtml)->Arg(10)->Arg(100)->Arg(500);

} // namespace
