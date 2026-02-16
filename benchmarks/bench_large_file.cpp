/// bench_large_file.cpp — Phase 35: Large file mode benchmarks

#include "core/LargeFileMode.h"

#include <benchmark/benchmark.h>

namespace
{

void BM_LargeFileMode_Evaluate(benchmark::State& state)
{
    markamp::core::LargeFileMode lfm;
    auto file_size = static_cast<std::size_t>(state.range(0));

    for (auto _ : state)
    {
        lfm.evaluate(file_size, file_size / 80); // ~80 chars per line
        benchmark::DoNotOptimize(lfm.is_active());
    }
}
BENCHMARK(BM_LargeFileMode_Evaluate)->Range(1024, 16 * 1024 * 1024);

void BM_LargeFileMode_FeatureCheck(benchmark::State& state)
{
    markamp::core::LargeFileMode lfm;
    lfm.evaluate(10 * 1024 * 1024, 100000);

    for (auto _ : state)
    {
        auto features = lfm.allowed_features();
        benchmark::DoNotOptimize(features.syntax_highlighting);
        benchmark::DoNotOptimize(features.minimap);
    }
}
BENCHMARK(BM_LargeFileMode_FeatureCheck);

} // namespace
