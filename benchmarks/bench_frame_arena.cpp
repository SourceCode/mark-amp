/// bench_frame_arena.cpp — Benchmarks for FrameArena vs heap allocation
///
/// Per Phase 06 doc: measure arena alloc/reset cycle vs new/delete,
/// many small allocations per frame, and single large allocation.

#include "core/FrameArena.h"

#include <benchmark/benchmark.h>
#include <cstddef>
#include <memory>
#include <vector>

using markamp::core::FrameArena;

namespace
{

/// BM_FrameArena_AllocReset: arena alloc+reset cycle vs heap new/delete
void BM_FrameArena_AllocReset(benchmark::State& state)
{
    FrameArena arena;
    for (auto _ : state)
    {
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
BENCHMARK(BM_FrameArena_AllocReset);

/// BM_Heap_AllocDelete: baseline comparison — heap new/delete
void BM_Heap_AllocDelete(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto vec = std::make_unique<std::vector<int>>();
        vec->reserve(256);
        for (int idx = 0; idx < 256; ++idx)
        {
            vec->push_back(idx);
        }
        benchmark::DoNotOptimize(vec->data());
    }
    state.SetItemsProcessed(state.iterations() * 256);
}
BENCHMARK(BM_Heap_AllocDelete);

/// BM_FrameArena_ManySmallAllocs: 1000 small allocations per frame
void BM_FrameArena_ManySmallAllocs(benchmark::State& state)
{
    FrameArena arena(256 * 1024); // 256KB to fit 1000 vectors
    for (auto _ : state)
    {
        for (int idx = 0; idx < 1000; ++idx)
        {
            auto vec = arena.make_vector<std::byte>();
            vec.resize(16);
            benchmark::DoNotOptimize(vec.data());
        }
        arena.reset();
    }
    state.SetItemsProcessed(state.iterations() * 1000);
}
BENCHMARK(BM_FrameArena_ManySmallAllocs);

/// BM_FrameArena_LargeAlloc: single large allocation
void BM_FrameArena_LargeAlloc(benchmark::State& state)
{
    FrameArena arena(128 * 1024); // 128KB
    for (auto _ : state)
    {
        auto vec = arena.make_vector<char>();
        vec.resize(32 * 1024); // 32KB
        benchmark::DoNotOptimize(vec.data());
        arena.reset();
    }
}
BENCHMARK(BM_FrameArena_LargeAlloc);

} // namespace
