/// bench_config.cpp — Config benchmarks
///
/// Phase 09: measures get/set operations and config reconstruction cost.

#include "core/Config.h"

#include <benchmark/benchmark.h>
#include <string>

namespace
{

/// BM_Config_GetString: cached string lookup
void BM_Config_GetString(benchmark::State& state)
{
    markamp::core::Config config;

    for (auto _ : state)
    {
        auto result = config.get_string("theme", "default");
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_Config_GetString);

/// BM_Config_GetBool: cached bool lookup
void BM_Config_GetBool(benchmark::State& state)
{
    markamp::core::Config config;

    for (auto _ : state)
    {
        auto result = config.get_bool("auto_save", false);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_Config_GetBool);

/// BM_Config_GetInt: cached int lookup
void BM_Config_GetInt(benchmark::State& state)
{
    markamp::core::Config config;

    for (auto _ : state)
    {
        auto result = config.get_int("font_size", 14);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_Config_GetInt);

/// BM_Config_SetAndGet: set + get cycle (triggers cache rebuild)
void BM_Config_SetAndGet(benchmark::State& state)
{
    markamp::core::Config config;
    int counter = 0;

    for (auto _ : state)
    {
        config.set("font_size", ++counter);
        auto result = config.get_int("font_size", 14);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_Config_SetAndGet);

/// BM_Config_CachedAccess: O(1) cached struct access
void BM_Config_CachedAccess(benchmark::State& state)
{
    markamp::core::Config config;

    for (auto _ : state)
    {
        const auto& cached = config.cached();
        auto font_size = cached.font_size;
        auto theme = cached.theme;
        auto auto_save = cached.auto_save;
        benchmark::DoNotOptimize(font_size);
        benchmark::DoNotOptimize(theme);
        benchmark::DoNotOptimize(auto_save);
    }
}
BENCHMARK(BM_Config_CachedAccess);

} // namespace
