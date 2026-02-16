/// bench_spsc_queue.cpp — Benchmarks for SPSCQueue push/pop throughput
///
/// Per Phase 06 doc: measure single-producer single-consumer throughput
/// and batch push operations, parameterized by message size.

#include "core/SPSCQueue.h"

#include <benchmark/benchmark.h>
#include <cstddef>
#include <string>

using markamp::core::SPSCQueue;

namespace
{

struct SmallMessage
{
    int value{0};
};

struct MediumMessage
{
    int value{0};
    double data[8]{};
};

struct LargeMessage
{
    int value{0};
    char payload[256]{};
};

/// BM_SPSCQueue_PushPop: push/pop roundtrip for small messages
void BM_SPSCQueue_PushPop_Small(benchmark::State& state)
{
    SPSCQueue<SmallMessage, 1024> queue;
    SmallMessage msg{42};
    SmallMessage out{};

    for (auto _ : state)
    {
        queue.try_push(msg);
        queue.try_pop(out);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_SPSCQueue_PushPop_Small);

/// BM_SPSCQueue_PushPop: push/pop roundtrip for medium messages
void BM_SPSCQueue_PushPop_Medium(benchmark::State& state)
{
    SPSCQueue<MediumMessage, 1024> queue;
    MediumMessage msg{};
    msg.value = 42;
    MediumMessage out{};

    for (auto _ : state)
    {
        queue.try_push(msg);
        queue.try_pop(out);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_SPSCQueue_PushPop_Medium);

/// BM_SPSCQueue_PushPop: push/pop roundtrip for large messages
void BM_SPSCQueue_PushPop_Large(benchmark::State& state)
{
    SPSCQueue<LargeMessage, 256> queue;
    LargeMessage msg{};
    msg.value = 42;
    LargeMessage out{};

    for (auto _ : state)
    {
        queue.try_push(msg);
        queue.try_pop(out);
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_SPSCQueue_PushPop_Large);

/// BM_SPSCQueue_BatchPush: batch push then drain
void BM_SPSCQueue_BatchPush(benchmark::State& state)
{
    SPSCQueue<SmallMessage, 1024> queue;
    const auto batch_size = state.range(0);

    for (auto _ : state)
    {
        for (int64_t idx = 0; idx < batch_size; ++idx)
        {
            queue.try_push(SmallMessage{static_cast<int>(idx)});
        }
        SmallMessage out{};
        for (int64_t idx = 0; idx < batch_size; ++idx)
        {
            queue.try_pop(out);
        }
        benchmark::DoNotOptimize(out);
    }
    state.SetItemsProcessed(state.iterations() * batch_size);
}
BENCHMARK(BM_SPSCQueue_BatchPush)->Arg(10)->Arg(100)->Arg(500);

} // namespace
