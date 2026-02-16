/// bench_theme_registry.cpp — ThemeRegistry benchmarks
///
/// Phase 09: measures theme lookup and listing operations after initialization.

#include "core/ThemeRegistry.h"

#include <benchmark/benchmark.h>

namespace
{

/// BM_ThemeRegistry_GetTheme: lookup a theme by ID
void BM_ThemeRegistry_GetTheme(benchmark::State& state)
{
    markamp::core::ThemeRegistry registry;
    auto init_result = registry.initialize();
    benchmark::DoNotOptimize(init_result);

    for (auto _ : state)
    {
        auto theme = registry.get_theme("midnight-neon");
        benchmark::DoNotOptimize(theme);
    }
}
BENCHMARK(BM_ThemeRegistry_GetTheme);

/// BM_ThemeRegistry_HasTheme: check if a theme exists
void BM_ThemeRegistry_HasTheme(benchmark::State& state)
{
    markamp::core::ThemeRegistry registry;
    auto init_result = registry.initialize();
    benchmark::DoNotOptimize(init_result);

    for (auto _ : state)
    {
        auto exists = registry.has_theme("midnight-neon");
        benchmark::DoNotOptimize(exists);
    }
}
BENCHMARK(BM_ThemeRegistry_HasTheme);

/// BM_ThemeRegistry_ListThemes: enumerate all available themes
void BM_ThemeRegistry_ListThemes(benchmark::State& state)
{
    markamp::core::ThemeRegistry registry;
    auto init_result = registry.initialize();
    benchmark::DoNotOptimize(init_result);

    for (auto _ : state)
    {
        auto themes = registry.list_themes();
        benchmark::DoNotOptimize(themes);
    }
}
BENCHMARK(BM_ThemeRegistry_ListThemes);

/// BM_ThemeRegistry_ThemeCount: O(1) count query
void BM_ThemeRegistry_ThemeCount(benchmark::State& state)
{
    markamp::core::ThemeRegistry registry;
    auto init_result = registry.initialize();
    benchmark::DoNotOptimize(init_result);

    for (auto _ : state)
    {
        auto count = registry.theme_count();
        benchmark::DoNotOptimize(count);
    }
}
BENCHMARK(BM_ThemeRegistry_ThemeCount);

} // namespace
