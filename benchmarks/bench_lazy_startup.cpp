/// bench_lazy_startup.cpp — LazyService benchmarks
///
/// Phase 12: Compares eager vs lazy construction overhead and
/// measures first-access latency for lazy services.

#include "core/LazyService.h"

#include <benchmark/benchmark.h>
#include <memory>
#include <string>
#include <vector>

namespace
{

/// Simulated extension service with non-trivial constructor.
struct MockExtensionService
{
    std::vector<std::string> registry;
    std::string service_name;

    explicit MockExtensionService(const std::string& name)
        : service_name(name)
    {
        // Simulate some initialization work
        registry.reserve(100);
        for (int idx = 0; idx < 100; ++idx)
        {
            registry.emplace_back("handler_" + std::to_string(idx));
        }
    }
};

/// BM_EagerConstruction: construct 21 services eagerly
void BM_EagerConstruction(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<std::unique_ptr<MockExtensionService>> services;
        services.reserve(21);
        for (int idx = 0; idx < 21; ++idx)
        {
            services.push_back(
                std::make_unique<MockExtensionService>("service_" + std::to_string(idx)));
        }
        benchmark::DoNotOptimize(services.data());
    }
    state.SetItemsProcessed(state.iterations() * 21);
}
BENCHMARK(BM_EagerConstruction);

/// BM_LazyConstruction_Setup: create 21 LazyService wrappers (no construction)
void BM_LazyConstruction_Setup(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<std::unique_ptr<markamp::core::LazyService<MockExtensionService>>> services;
        services.reserve(21);
        for (int idx = 0; idx < 21; ++idx)
        {
            auto name = "service_" + std::to_string(idx);
            services.push_back(std::make_unique<markamp::core::LazyService<MockExtensionService>>(
                [name]() { return std::make_unique<MockExtensionService>(name); }));
        }
        benchmark::DoNotOptimize(services.data());
    }
    state.SetItemsProcessed(state.iterations() * 21);
}
BENCHMARK(BM_LazyConstruction_Setup);

/// BM_LazyConstruction_FirstAccess: lazy init on first get_or_create()
void BM_LazyConstruction_FirstAccess(benchmark::State& state)
{
    for (auto _ : state)
    {
        markamp::core::LazyService<MockExtensionService> lazy{
            []() { return std::make_unique<MockExtensionService>("test"); }};

        auto* svc = lazy.get_or_create();
        benchmark::DoNotOptimize(svc);
    }
}
BENCHMARK(BM_LazyConstruction_FirstAccess);

/// BM_LazyConstruction_CachedAccess: access after construction (hot path)
void BM_LazyConstruction_CachedAccess(benchmark::State& state)
{
    markamp::core::LazyService<MockExtensionService> lazy{
        []() { return std::make_unique<MockExtensionService>("cached"); }};

    // Trigger construction
    lazy.get_or_create();

    for (auto _ : state)
    {
        auto* svc = lazy.get_or_create();
        benchmark::DoNotOptimize(svc);
    }
}
BENCHMARK(BM_LazyConstruction_CachedAccess);

/// BM_LazyConstruction_SubsetAccess: only 5 of 21 services accessed
void BM_LazyConstruction_SubsetAccess(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<std::unique_ptr<markamp::core::LazyService<MockExtensionService>>> services;
        services.reserve(21);
        for (int idx = 0; idx < 21; ++idx)
        {
            auto name = "service_" + std::to_string(idx);
            services.push_back(std::make_unique<markamp::core::LazyService<MockExtensionService>>(
                [name]() { return std::make_unique<MockExtensionService>(name); }));
        }

        // Only access 5 of 21 services (realistic usage)
        for (int idx = 0; idx < 5; ++idx)
        {
            auto* svc = services[static_cast<size_t>(idx)]->get_or_create();
            benchmark::DoNotOptimize(svc);
        }
    }
    state.SetItemsProcessed(state.iterations() * 5);
}
BENCHMARK(BM_LazyConstruction_SubsetAccess);

} // namespace
