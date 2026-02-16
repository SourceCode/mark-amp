/// bench_eventbus.cpp — Benchmarks for EventBus publish/subscribe throughput
///
/// Per Phase 06 doc: measure publish, publish_fast, and subscribe/unsubscribe
/// cycle throughput, parameterized by subscriber count.

#include "core/EventBus.h"

#include <benchmark/benchmark.h>
#include <functional>
#include <string>
#include <vector>

using markamp::core::Event;
using markamp::core::EventBus;
using markamp::core::Subscription;

namespace
{

struct BenchEvent final : Event
{
    int value{0};
    explicit BenchEvent(int val = 0)
        : value(val)
    {
    }
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "BenchEvent";
    }
};

/// BM_EventBus_Publish: synchronous publish throughput with N subscribers
void BM_EventBus_Publish(benchmark::State& state)
{
    EventBus bus;
    const auto subscriber_count = state.range(0);
    std::vector<Subscription> subs;
    subs.reserve(static_cast<std::size_t>(subscriber_count));

    int sink = 0;
    for (int64_t idx = 0; idx < subscriber_count; ++idx)
    {
        subs.push_back(
            bus.subscribe<BenchEvent>([&sink](const BenchEvent& evt) { sink += evt.value; }));
    }

    const BenchEvent evt(42);
    for (auto _ : state)
    {
        bus.publish(evt);
        benchmark::DoNotOptimize(sink);
    }
    state.SetItemsProcessed(state.iterations() * subscriber_count);
}
BENCHMARK(BM_EventBus_Publish)->Arg(1)->Arg(10)->Arg(100);

/// BM_EventBus_PublishFast: lock-free fast path publish throughput
void BM_EventBus_PublishFast(benchmark::State& state)
{
    EventBus bus;
    const auto subscriber_count = state.range(0);
    std::vector<Subscription> subs;
    subs.reserve(static_cast<std::size_t>(subscriber_count));

    int sink = 0;
    for (int64_t idx = 0; idx < subscriber_count; ++idx)
    {
        subs.push_back(
            bus.subscribe<BenchEvent>([&sink](const BenchEvent& evt) { sink += evt.value; }));
    }

    const BenchEvent evt(42);
    for (auto _ : state)
    {
        bus.publish_fast(evt);
        benchmark::DoNotOptimize(sink);
    }
    state.SetItemsProcessed(state.iterations() * subscriber_count);
}
BENCHMARK(BM_EventBus_PublishFast)->Arg(1)->Arg(10)->Arg(100);

/// BM_EventBus_Subscribe: subscribe/unsubscribe cycle
void BM_EventBus_Subscribe(benchmark::State& state)
{
    EventBus bus;
    for (auto _ : state)
    {
        auto sub = bus.subscribe<BenchEvent>([](const BenchEvent& /*evt*/) {});
        benchmark::DoNotOptimize(sub);
    }
}
BENCHMARK(BM_EventBus_Subscribe);

} // namespace
