/// bench_constexpr_lookup.cpp — Phase 26: Constexpr map vs runtime map benchmarks

#include "core/ConstexprMap.h"

#include <benchmark/benchmark.h>
#include <string>
#include <string_view>
#include <unordered_map>

namespace
{

/// Baseline: unordered_map MIME lookup
void BM_UnorderedMap_MimeLookup(benchmark::State& state)
{
    std::unordered_map<std::string, std::string> mime_map;
    mime_map["html"] = "text/html";
    mime_map["css"] = "text/css";
    mime_map["js"] = "application/javascript";
    mime_map["json"] = "application/json";
    mime_map["md"] = "text/markdown";
    mime_map["png"] = "image/png";
    mime_map["svg"] = "image/svg+xml";

    for (auto _ : state)
    {
        auto iter = mime_map.find("json");
        benchmark::DoNotOptimize(iter);
    }
}
BENCHMARK(BM_UnorderedMap_MimeLookup);

/// ConstexprMap MIME lookup (binary search, no heap)
void BM_ConstexprMap_MimeLookup(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto result = markamp::core::kMimeTypes.find("json");
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_ConstexprMap_MimeLookup);

/// ConstexprMap keyword lookup
void BM_ConstexprMap_KeywordLookup(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto result = markamp::core::kCppKeywords.find("constexpr");
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_ConstexprMap_KeywordLookup);

} // namespace
