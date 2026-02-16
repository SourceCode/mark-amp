/// bench_arena_hotpaths.cpp — Arena allocator benchmarks
///
/// Phase 14: Compares pmr arena-backed containers vs std containers
/// and measures arena reset throughput and thread_local access overhead.

#include "core/FrameArena.h"

#include <benchmark/benchmark.h>
#include <memory_resource>
#include <string>
#include <vector>

namespace
{

/// BM_Arena_PmrVector_vs_StdVector: allocate+fill a vector 1000 times
void BM_StdVector_Allocate(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<int> vec;
        vec.reserve(256);
        for (int idx = 0; idx < 256; ++idx)
        {
            vec.push_back(idx);
        }
        benchmark::DoNotOptimize(vec.data());
    }
    state.SetItemsProcessed(state.iterations() * 256);
}
BENCHMARK(BM_StdVector_Allocate);

void BM_ArenaVector_Allocate(benchmark::State& state)
{
    markamp::core::FrameArenaPool pool;
    for (auto _ : state)
    {
        auto& arena = pool.get_arena();
        auto vec = arena.make_vector<int>();
        vec.reserve(256);
        for (int idx = 0; idx < 256; ++idx)
        {
            vec.push_back(idx);
        }
        benchmark::DoNotOptimize(vec.data());
        arena.reset();
    }
    state.SetItemsProcessed(state.iterations() * 256);
}
BENCHMARK(BM_ArenaVector_Allocate);

/// BM_Arena_PmrString_vs_StdString: string concatenation
void BM_StdString_Concatenate(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::string result;
        result.reserve(4096);
        for (int idx = 0; idx < 100; ++idx)
        {
            result += "<div class=\"block\">";
            result += "content_";
            result += std::to_string(idx);
            result += "</div>";
        }
        benchmark::DoNotOptimize(result.data());
    }
    state.SetItemsProcessed(state.iterations() * 100);
}
BENCHMARK(BM_StdString_Concatenate);

void BM_ArenaString_Concatenate(benchmark::State& state)
{
    markamp::core::FrameArenaPool pool;
    for (auto _ : state)
    {
        auto& arena = pool.get_arena();
        std::pmr::string result{arena.allocator()};
        result.reserve(4096);
        for (int idx = 0; idx < 100; ++idx)
        {
            result += "<div class=\"block\">";
            result += "content_";
            result += std::to_string(idx);
            result += "</div>";
        }
        benchmark::DoNotOptimize(result.data());
        arena.reset();
    }
    state.SetItemsProcessed(state.iterations() * 100);
}
BENCHMARK(BM_ArenaString_Concatenate);

/// BM_Arena_Reset_Throughput: measure arena reset latency
void BM_Arena_Reset(benchmark::State& state)
{
    markamp::core::FrameArenaPool pool;
    auto& arena = pool.get_arena();
    for (auto _ : state)
    {
        // Fill arena with some data
        auto vec = arena.make_vector<int>();
        vec.resize(64);
        benchmark::DoNotOptimize(vec.data());

        // Measure reset
        arena.reset();
    }
}
BENCHMARK(BM_Arena_Reset);

/// BM_Arena_ThreadLocal_Access: measure get_arena() overhead
void BM_Arena_ThreadLocal_Access(benchmark::State& state)
{
    markamp::core::FrameArenaPool pool;
    for (auto _ : state)
    {
        auto& arena = pool.get_arena();
        benchmark::DoNotOptimize(&arena);
    }
}
BENCHMARK(BM_Arena_ThreadLocal_Access);

} // namespace
