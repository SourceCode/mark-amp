/// bench_eventbus_v2.cpp — Phase 20: FlatEventBus benchmarks
///
/// Compares FlatEventBus O(1) dispatch vs original EventBus map lookup.

#include "core/EventTypeId.h"
#include "core/FlatEventBus.h"

#include <benchmark/benchmark.h>
#include <string_view>

namespace
{

struct BenchEvent : markamp::core::Event
{
    int value{42};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "BenchEvent";
    }
};

/// BM_FlatEventBus_Publish: O(1) event dispatch
void BM_FlatEventBus_Publish(benchmark::State& state)
{
    markamp::core::FlatEventBus bus;
    auto sub = bus.subscribe(markamp::core::EventTypeId::EditorContentChanged,
                             [](const markamp::core::Event& /*evt*/) {});

    BenchEvent event;
    for (auto _ : state)
    {
        bus.publish(markamp::core::EventTypeId::EditorContentChanged, event);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_FlatEventBus_Publish);

/// BM_FlatEventBus_PublishMultiHandler: dispatch to multiple handlers
void BM_FlatEventBus_PublishMultiHandler(benchmark::State& state)
{
    markamp::core::FlatEventBus bus;
    auto sub1 = bus.subscribe(markamp::core::EventTypeId::CursorPositionChanged,
                              [](const markamp::core::Event& /*evt*/) {});
    auto sub2 = bus.subscribe(markamp::core::EventTypeId::CursorPositionChanged,
                              [](const markamp::core::Event& /*evt*/) {});
    auto sub3 = bus.subscribe(markamp::core::EventTypeId::CursorPositionChanged,
                              [](const markamp::core::Event& /*evt*/) {});

    BenchEvent event;
    for (auto _ : state)
    {
        bus.publish(markamp::core::EventTypeId::CursorPositionChanged, event);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_FlatEventBus_PublishMultiHandler);

/// BM_FlatEventBus_Subscribe: subscribe throughput
void BM_FlatEventBus_Subscribe(benchmark::State& state)
{
    markamp::core::FlatEventBus bus;

    for (auto _ : state)
    {
        auto sub = bus.subscribe(markamp::core::EventTypeId::ThemeChanged,
                                 [](const markamp::core::Event& /*evt*/) {});
        benchmark::DoNotOptimize(sub);
    }
}
BENCHMARK(BM_FlatEventBus_Subscribe);

} // namespace
