/// bench_profiler.cpp — Benchmarks for Profiler record/scope/report overhead
///
/// Per Phase 06 doc: measure record() throughput, MARKAMP_PROFILE_SCOPE
/// overhead, and report generation time.

#include "core/Profiler.h"

#include <benchmark/benchmark.h>
#include <string>

using markamp::core::Profiler;

namespace
{

/// BM_Profiler_Record: raw record() throughput
void BM_Profiler_Record(benchmark::State& state)
{
    Profiler profiler;
    for (auto _ : state)
    {
        profiler.record("bench_metric", 1.5);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Profiler_Record);

/// BM_Profiler_Scope: MARKAMP_PROFILE_SCOPE overhead (no-op timing)
void BM_Profiler_Scope(benchmark::State& state)
{
    for (auto _ : state)
    {
        MARKAMP_PROFILE_SCOPE("bench_scope");
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_Profiler_Scope);

/// BM_Profiler_Report: report generation time after N records
void BM_Profiler_Report(benchmark::State& state)
{
    Profiler profiler;
    const auto record_count = state.range(0);
    for (int64_t idx = 0; idx < record_count; ++idx)
    {
        profiler.record("metric_" + std::to_string(idx % 10), static_cast<double>(idx));
    }

    for (auto _ : state)
    {
        auto results = profiler.results();
        benchmark::DoNotOptimize(results);
    }
}
BENCHMARK(BM_Profiler_Report)->Arg(100)->Arg(1000)->Arg(5000);

} // namespace
