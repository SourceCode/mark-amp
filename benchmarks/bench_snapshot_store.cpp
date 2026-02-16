/// bench_snapshot_store.cpp — Phase 21: SnapshotStore benchmarks
///
/// Measures lock-free read throughput vs mutex-protected baseline.

#include "core/SnapshotStore.h"

#include <benchmark/benchmark.h>
#include <mutex>
#include <string>

namespace
{

/// BM_SnapshotStore_Read: atomic load throughput (lock-free)
void BM_SnapshotStore_Read(benchmark::State& state)
{
    markamp::core::SnapshotStore<std::string> store("hello world");

    for (auto _ : state)
    {
        auto snap = store.read();
        benchmark::DoNotOptimize(snap);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_SnapshotStore_Read);

/// BM_SnapshotStore_Update: write latency
void BM_SnapshotStore_Update(benchmark::State& state)
{
    markamp::core::SnapshotStore<int> store(0);
    int counter = 0;

    for (auto _ : state)
    {
        store.store(++counter);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_SnapshotStore_Update);

/// BM_MutexProtected_Read: baseline comparison with mutex
void BM_MutexProtected_Read(benchmark::State& state)
{
    std::string value = "hello world";
    std::mutex mtx;

    for (auto _ : state)
    {
        std::lock_guard<std::mutex> lock(mtx);
        benchmark::DoNotOptimize(value);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_MutexProtected_Read);

} // namespace
