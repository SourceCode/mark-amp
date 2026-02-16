# benchmarks/CMakeLists.txt -- Google Benchmark integration
#
# Builds the markamp_bench executable containing all micro-benchmarks.
# Gated behind MARKAMP_BUILD_BENCHMARKS (OFF by default).

find_package(benchmark CONFIG REQUIRED)

# Create the unified benchmark executable
add_executable(markamp_bench
    # Phase 06: Core data structure benchmarks
    bench_eventbus.cpp
    bench_profiler.cpp
    bench_frame_arena.cpp
    bench_spsc_queue.cpp
    bench_piece_table.cpp
    # Phase 08: Startup profiling
    bench_startup.cpp
    # Phase 09: Runtime profiling
    bench_markdown_parser.cpp
    bench_syntax_highlighter.cpp
    bench_config.cpp
    bench_theme_registry.cpp
    bench_html_renderer.cpp
    # Phase 10: Allocator benchmarks
    bench_allocator.cpp
    # Phase 12: Lazy service benchmarks
    bench_lazy_startup.cpp
    # Phase 14: Arena hot-path benchmarks
    bench_arena_hotpaths.cpp
    # Phase 16: Profiler v2 benchmarks
    bench_profiler_v2.cpp
    # Phase 17: Branch elimination benchmarks
    bench_branch_elimination.cpp
    # Phase 18: Rendering pipeline benchmarks
    bench_rendering_pipeline.cpp
)

target_include_directories(markamp_bench PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(markamp_bench PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
    markamp_core
)

# IDE source grouping
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Benchmarks" FILES
    bench_eventbus.cpp
    bench_profiler.cpp
    bench_frame_arena.cpp
    bench_spsc_queue.cpp
    bench_piece_table.cpp
    bench_startup.cpp
    bench_markdown_parser.cpp
    bench_syntax_highlighter.cpp
    bench_config.cpp
    bench_theme_registry.cpp
    bench_html_renderer.cpp
    bench_allocator.cpp
    bench_lazy_startup.cpp
    bench_arena_hotpaths.cpp
    bench_profiler_v2.cpp
    bench_branch_elimination.cpp
    bench_rendering_pipeline.cpp
    bench_eventbus_v2.cpp
    bench_snapshot_store.cpp
    bench_data_oriented.cpp
    bench_constexpr_lookup.cpp
    bench_incremental_tokenization.cpp
    bench_large_file.cpp
)
