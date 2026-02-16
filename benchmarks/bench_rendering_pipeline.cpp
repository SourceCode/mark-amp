/// bench_rendering_pipeline.cpp — Phase 18: Rendering pipeline benchmarks
///
/// Measures RenderPipeline event processing and frame lifecycle overhead.

#include "rendering/RenderPipeline.h"

#include <benchmark/benchmark.h>

namespace
{

/// BM_RenderPipeline_Scroll: single scroll event processing
void BM_RenderPipeline_Scroll(benchmark::State& state)
{
    markamp::rendering::RenderPipeline pipeline;
    pipeline.initialize(1920, 1080);

    for (auto _ : state)
    {
        pipeline.on_scroll(0, 20);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_RenderPipeline_Scroll);

/// BM_RenderPipeline_EditInvalidation: edit + dirty region computation
void BM_RenderPipeline_EditInvalidation(benchmark::State& state)
{
    markamp::rendering::RenderPipeline pipeline;
    pipeline.initialize(1920, 1080);

    // Pre-populate some wrap data
    for (std::size_t idx = 0; idx < 100; ++idx)
    {
        pipeline.line_wrap().set(idx, markamp::rendering::WrapResult{{10, 20}, 80});
    }

    for (auto _ : state)
    {
        pipeline.on_edit(50, 55);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_RenderPipeline_EditInvalidation);

/// BM_RenderPipeline_FullFrame: begin_frame + end_frame cycle
void BM_RenderPipeline_FullFrame(benchmark::State& state)
{
    markamp::rendering::RenderPipeline pipeline;
    pipeline.initialize(1920, 1080);
    pipeline.set_viewport(100, 50);

    for (auto _ : state)
    {
        pipeline.on_scroll(0, 15);
        auto work = pipeline.begin_frame(10000);
        benchmark::DoNotOptimize(work.dirty_rects);
        pipeline.end_frame();
    }
}
BENCHMARK(BM_RenderPipeline_FullFrame);

/// BM_RenderPipeline_Resize: resize event processing
void BM_RenderPipeline_Resize(benchmark::State& state)
{
    markamp::rendering::RenderPipeline pipeline;
    pipeline.initialize(1920, 1080);

    for (auto _ : state)
    {
        pipeline.on_resize(1920, 1080);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_RenderPipeline_Resize);

} // namespace
