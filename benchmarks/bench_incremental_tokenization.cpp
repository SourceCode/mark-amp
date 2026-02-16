/// bench_incremental_tokenization.cpp — Phase 28: Incremental vs full tokenization

#include "core/IncrementalTokenizer.h"
#include "core/SyntaxHighlighter.h"

#include <benchmark/benchmark.h>
#include <string>
#include <vector>

namespace
{

auto make_lines(std::size_t count) -> std::vector<std::string>
{
    std::vector<std::string> lines;
    lines.reserve(count);
    for (std::size_t idx = 0; idx < count; ++idx)
    {
        lines.push_back("int variable_" + std::to_string(idx) + " = " + std::to_string(idx) + ";");
    }
    return lines;
}

/// Full tokenization of all lines
void BM_FullTokenize(benchmark::State& state)
{
    markamp::core::SyntaxHighlighter highlighter;
    auto lines = make_lines(static_cast<std::size_t>(state.range(0)));

    for (auto _ : state)
    {
        for (const auto& line : lines)
        {
            auto tokens = highlighter.tokenize(line, "cpp");
            benchmark::DoNotOptimize(tokens);
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_FullTokenize)->Range(64, 1024);

/// Incremental: change 1 line out of N
void BM_IncrementalTokenize_SingleLine(benchmark::State& state)
{
    markamp::core::SyntaxHighlighter highlighter;
    markamp::core::IncrementalTokenizer tokenizer(highlighter, "cpp");

    auto lines = make_lines(static_cast<std::size_t>(state.range(0)));
    tokenizer.set_content(lines);
    tokenizer.retokenize(); // warm up cache

    for (auto _ : state)
    {
        tokenizer.invalidate_line(0);
        auto count = tokenizer.retokenize();
        benchmark::DoNotOptimize(count);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_IncrementalTokenize_SingleLine)->Range(64, 1024);

/// Incremental: change 10% of lines
void BM_IncrementalTokenize_TenPercent(benchmark::State& state)
{
    markamp::core::SyntaxHighlighter highlighter;
    markamp::core::IncrementalTokenizer tokenizer(highlighter, "cpp");

    auto total = static_cast<std::size_t>(state.range(0));
    auto lines = make_lines(total);
    tokenizer.set_content(lines);
    tokenizer.retokenize();

    auto dirty_count = total / 10;

    for (auto _ : state)
    {
        for (std::size_t idx = 0; idx < dirty_count; ++idx)
        {
            tokenizer.invalidate_line(idx);
        }
        auto count = tokenizer.retokenize();
        benchmark::DoNotOptimize(count);
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(dirty_count));
}
BENCHMARK(BM_IncrementalTokenize_TenPercent)->Range(64, 1024);

} // namespace
