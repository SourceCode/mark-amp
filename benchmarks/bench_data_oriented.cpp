/// bench_data_oriented.cpp — Phase 23: SoA vs AoS iteration benchmarks
///
/// Measures cache-line performance difference between data-oriented and
/// object-oriented storage patterns.

#include "core/SoAStorage.h"

#include <benchmark/benchmark.h>
#include <cstdint>
#include <string>
#include <vector>

namespace
{

// AoS (Array-of-Structs) baseline
struct EntityAoS
{
    uint32_t id;
    float x;
    float y;
    float z;
    std::string name;
    bool active;
};

/// BM_AoS_Iterate: iterate over all positions in AoS layout
void BM_AoS_Iterate(benchmark::State& state)
{
    auto count = static_cast<std::size_t>(state.range(0));
    std::vector<EntityAoS> entities(count);
    for (std::size_t idx = 0; idx < count; ++idx)
    {
        entities[idx] = {static_cast<uint32_t>(idx),
                         static_cast<float>(idx),
                         static_cast<float>(idx) * 2.0F,
                         static_cast<float>(idx) * 3.0F,
                         "entity",
                         true};
    }

    for (auto _ : state)
    {
        float sum = 0.0F;
        for (const auto& entity : entities)
        {
            sum += entity.x + entity.y + entity.z;
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}
BENCHMARK(BM_AoS_Iterate)->Range(64, 8192);

/// BM_SoA_Iterate: iterate over positions in SoA layout (cache-friendly)
void BM_SoA_Iterate(benchmark::State& state)
{
    auto count = static_cast<std::size_t>(state.range(0));
    markamp::core::SoAStore<uint32_t, float, float, float, std::string, bool> store;
    store.reserve(count);

    for (std::size_t idx = 0; idx < count; ++idx)
    {
        store.add(static_cast<uint32_t>(idx),
                  static_cast<float>(idx),
                  static_cast<float>(idx) * 2.0F,
                  static_cast<float>(idx) * 3.0F,
                  "entity",
                  true);
    }

    for (auto _ : state)
    {
        float sum = 0.0F;
        const auto& xs = store.column<1>();
        const auto& ys = store.column<2>();
        const auto& zs = store.column<3>();
        for (std::size_t idx = 0; idx < count; ++idx)
        {
            sum += xs[idx] + ys[idx] + zs[idx];
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}
BENCHMARK(BM_SoA_Iterate)->Range(64, 8192);

/// BM_SoA_AddRemove: add and remove throughput
void BM_SoA_AddRemove(benchmark::State& state)
{
    markamp::core::SoAStore<int, float> store;

    for (auto _ : state)
    {
        store.add(42, 3.14F);
        if (store.size() > 100)
        {
            store.remove(0);
        }
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SoA_AddRemove);

} // namespace
