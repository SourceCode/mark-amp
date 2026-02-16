/// bench_branch_elimination.cpp — Phase 17: Strategy dispatch benchmarks
///
/// Compares resolved function pointer calls vs inline branching
/// to quantify the benefit of ResolvedConfig.

#include "core/ResolvedConfig.h"

#include <benchmark/benchmark.h>
#include <string>

namespace
{

/// BM_ResolvedConfig_StrategyCall: Call wrap strategy through function pointer
void BM_ResolvedConfig_StrategyCall(benchmark::State& state)
{
    markamp::core::Config::CachedValues cached;
    cached.word_wrap = true;
    cached.word_wrap_column = 80;

    markamp::core::ResolvedConfig resolved;
    resolved.rebuild(cached);

    std::string line(120, 'x');
    // Insert spaces for wrap points
    line[30] = ' ';
    line[60] = ' ';
    line[90] = ' ';

    for (auto _ : state)
    {
        auto result = resolved.wrap_strategy(line, resolved.wrap_column);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_ResolvedConfig_StrategyCall);

/// BM_InlineBranch_WrapCheck: Inline branch check per call (simulated)
void BM_InlineBranch_WrapCheck(benchmark::State& state)
{
    bool word_wrap = true;
    uint32_t wrap_column = 80;

    std::string line(120, 'x');
    line[30] = ' ';
    line[60] = ' ';
    line[90] = ' ';

    for (auto _ : state)
    {
        std::size_t result = 0;
        if (word_wrap)
        {
            result = markamp::core::strategy::wrap_at_column(line, wrap_column);
        }
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_InlineBranch_WrapCheck);

/// BM_ResolvedConfig_Rebuild: Measure rebuild latency
void BM_ResolvedConfig_Rebuild(benchmark::State& state)
{
    markamp::core::Config::CachedValues cached;
    cached.word_wrap = true;
    cached.word_wrap_column = 80;
    cached.auto_indent = true;
    cached.show_whitespace = true;
    cached.dim_whitespace = false;
    cached.tab_size = 4;
    cached.font_size = 14;

    markamp::core::ResolvedConfig resolved;

    for (auto _ : state)
    {
        resolved.rebuild(cached);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_ResolvedConfig_Rebuild);

/// BM_IndentStrategy_MatchIndent: Measure indent detection throughput
void BM_IndentStrategy_MatchIndent(benchmark::State& state)
{
    std::string line = "    function() { return 42; }";

    for (auto _ : state)
    {
        auto result = markamp::core::strategy::match_indent(line);
        benchmark::DoNotOptimize(result);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_IndentStrategy_MatchIndent);

} // namespace
