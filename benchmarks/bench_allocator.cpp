/// bench_allocator.cpp — Allocator performance benchmarks
///
/// Phase 10: compares allocation patterns to establish a baseline.
/// When mimalloc is enabled, these benchmarks compare against the
/// system allocator baseline captured without mimalloc.

#include "core/AllocatorConfig.h"

#include <array>
#include <benchmark/benchmark.h>
#include <memory>
#include <string>
#include <vector>

namespace
{

/// BM_Allocator_SmallAlloc: many small allocations (16-64 bytes)
void BM_Allocator_SmallAlloc(benchmark::State& state)
{
    for (auto _ : state)
    {
        for (int idx = 0; idx < 1000; ++idx)
        {
            auto ptr = std::make_unique<std::array<char, 64>>();
            benchmark::DoNotOptimize(ptr.get());
        }
    }
    state.SetItemsProcessed(state.iterations() * 1000);
}
BENCHMARK(BM_Allocator_SmallAlloc);

/// BM_Allocator_MediumAlloc: medium allocations (~4KB page-sized)
void BM_Allocator_MediumAlloc(benchmark::State& state)
{
    for (auto _ : state)
    {
        for (int idx = 0; idx < 100; ++idx)
        {
            auto vec = std::vector<char>(4096, 'A');
            benchmark::DoNotOptimize(vec.data());
        }
    }
    state.SetItemsProcessed(state.iterations() * 100);
}
BENCHMARK(BM_Allocator_MediumAlloc);

/// BM_Allocator_LargeAlloc: large allocations (1MB+)
void BM_Allocator_LargeAlloc(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto vec = std::vector<char>(1024 * 1024, 'B');
        benchmark::DoNotOptimize(vec.data());
    }
}
BENCHMARK(BM_Allocator_LargeAlloc);

/// BM_Allocator_MixedPattern: realistic mixed allocation pattern
void BM_Allocator_MixedPattern(benchmark::State& state)
{
    for (auto _ : state)
    {
        // Many small strings (SSO + heap)
        std::vector<std::string> strings;
        strings.reserve(500);
        for (int idx = 0; idx < 500; ++idx)
        {
            strings.emplace_back(30 + (idx % 200), 'x');
        }

        // Some vectors
        std::vector<std::vector<int>> vecs;
        vecs.reserve(20);
        for (int idx = 0; idx < 20; ++idx)
        {
            vecs.emplace_back(100 + idx * 50, idx);
        }

        benchmark::DoNotOptimize(strings.data());
        benchmark::DoNotOptimize(vecs.data());
    }
}
BENCHMARK(BM_Allocator_MixedPattern);

/// BM_Allocator_AllocFree: rapid alloc/free churn
void BM_Allocator_AllocFree(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<std::unique_ptr<char[]>> ptrs;
        ptrs.reserve(1000);

        // Allocate 1000 blocks
        for (int idx = 0; idx < 1000; ++idx)
        {
            ptrs.push_back(std::make_unique<char[]>(128));
        }

        // Free in LIFO order (popping from back)
        while (!ptrs.empty())
        {
            benchmark::DoNotOptimize(ptrs.back().get());
            ptrs.pop_back();
        }
    }
    state.SetItemsProcessed(state.iterations() * 2000); // alloc + free
}
BENCHMARK(BM_Allocator_AllocFree);

} // namespace
