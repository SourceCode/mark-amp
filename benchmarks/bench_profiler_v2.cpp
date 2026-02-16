/// bench_profiler_v2.cpp — Phase 16 Profiler benchmarks
///
/// Compares ProfileId-based recording (O(1) fast path) vs
/// string-based recording (slow path with map lookup + allocation).

#include "core/Profiler.h"
#include "core/ProfilerIds.h"
#include "core/RingBuffer.h"

#include <benchmark/benchmark.h>
#include <string>

namespace
{

/// BM_Profiler_Record_ById: ProfileId fast path
void BM_Profiler_Record_ById(benchmark::State& state)
{
    markamp::core::Profiler profiler;
    for (auto _ : state)
    {
        profiler.record(markamp::core::ProfileId::EventBus_Publish, 1.234);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_Profiler_Record_ById);

/// BM_Profiler_Record_ByName: string slow path
void BM_Profiler_Record_ByName(benchmark::State& state)
{
    markamp::core::Profiler profiler;
    for (auto _ : state)
    {
        profiler.record("EventBus::publish", 1.234);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_Profiler_Record_ByName);

/// BM_RingBuffer_Write: raw ring buffer push throughput
void BM_RingBuffer_Write(benchmark::State& state)
{
    markamp::core::RingBuffer<double, 1024> ring;
    double val = 0.0;
    for (auto _ : state)
    {
        ring.push(val);
        val += 0.1;
        benchmark::DoNotOptimize(ring.size());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_RingBuffer_Write);

/// BM_ProfileId_NameLookup: constexpr name table lookup
void BM_ProfileId_NameLookup(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto name =
            markamp::core::profile_id_name(markamp::core::ProfileId::SyntaxHighlighter_Tokenize);
        benchmark::DoNotOptimize(name.data());
    }
}
BENCHMARK(BM_ProfileId_NameLookup);

} // namespace
