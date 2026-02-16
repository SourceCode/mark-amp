/// bench_syntax_highlighter.cpp — SyntaxHighlighter benchmarks
///
/// Phase 09: measures tokenize(), tokenize_soa(), and render_html()
/// for different languages and source sizes.

#include "core/SyntaxHighlighter.h"

#include <benchmark/benchmark.h>
#include <string>

namespace
{

/// Generate synthetic C++ source code of the given size.
auto generate_cpp_source(int lines) -> std::string
{
    std::string src;
    src.reserve(static_cast<size_t>(lines) * 60);

    src += "#include <iostream>\n#include <vector>\n#include <string>\n\n";
    src += "namespace benchmark_test {\n\n";

    for (int idx = 0; idx < lines; ++idx)
    {
        if (idx % 20 == 0)
        {
            src += "class Widget" + std::to_string(idx) + " {\npublic:\n";
            src += "    int value_{0};\n";
            src += "    std::string name_{\"default\"};\n";
        }
        else if (idx % 20 == 19)
        {
            src += "};\n\n";
        }
        else if (idx % 5 == 0)
        {
            src += "    // Comment line " + std::to_string(idx) + "\n";
        }
        else if (idx % 3 == 0)
        {
            src += "    auto compute(int x) -> double { return x * 3.14; }\n";
        }
        else
        {
            src += "    void process(const std::string& input) { /* body */ }\n";
        }
    }

    src += "\n} // namespace benchmark_test\n";
    return src;
}

/// BM_SyntaxHighlighter_Tokenize: tokenize C++ source
void BM_SyntaxHighlighter_Tokenize(benchmark::State& state)
{
    auto lines = static_cast<int>(state.range(0));
    auto source = generate_cpp_source(lines);
    markamp::core::SyntaxHighlighter highlighter;

    for (auto _ : state)
    {
        auto tokens = highlighter.tokenize(source, "cpp");
        benchmark::DoNotOptimize(tokens);
    }

    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(source.size()));
}
BENCHMARK(BM_SyntaxHighlighter_Tokenize)->Arg(50)->Arg(200)->Arg(1000);

/// BM_SyntaxHighlighter_TokenizeSoA: cache-friendly SoA tokenization
void BM_SyntaxHighlighter_TokenizeSoA(benchmark::State& state)
{
    auto lines = static_cast<int>(state.range(0));
    auto source = generate_cpp_source(lines);
    markamp::core::SyntaxHighlighter highlighter;

    for (auto _ : state)
    {
        auto tokens = highlighter.tokenize_soa(source, "cpp");
        benchmark::DoNotOptimize(tokens);
    }

    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(source.size()));
}
BENCHMARK(BM_SyntaxHighlighter_TokenizeSoA)->Arg(50)->Arg(200)->Arg(1000);

/// BM_SyntaxHighlighter_RenderHtml: full tokenize + HTML rendering
void BM_SyntaxHighlighter_RenderHtml(benchmark::State& state)
{
    auto lines = static_cast<int>(state.range(0));
    auto source = generate_cpp_source(lines);
    markamp::core::SyntaxHighlighter highlighter;

    for (auto _ : state)
    {
        auto html = highlighter.render_html(source, "cpp");
        benchmark::DoNotOptimize(html);
    }

    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(source.size()));
}
BENCHMARK(BM_SyntaxHighlighter_RenderHtml)->Arg(50)->Arg(200)->Arg(1000);

} // namespace
