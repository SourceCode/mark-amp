# Phase 26: Performance Optimization

## Objective

Optimize MarkAmp for production performance: sub-500ms cold start, smooth 60fps scrolling with 10,000+ line documents, non-blocking Mermaid rendering, efficient memory usage, and virtualized rendering where applicable. Profile, benchmark, and eliminate bottlenecks.

## Prerequisites

- Phase 19 (Split View Mode Implementation)
- Phase 20 (Editor Enhancements)
- Phase 18 (Preview Component)

## Deliverables

1. Performance profiling report with identified bottlenecks
2. Optimized cold start time (target: under 500ms)
3. Smooth scrolling for large documents
4. Async/non-blocking Mermaid rendering
5. Memory usage optimization
6. Rendering pipeline optimization
7. Performance regression test suite

## Tasks

### Task 26.1: Create performance profiling infrastructure

Create `/Users/ryanrentfro/code/markamp/src/core/Profiler.h` and `Profiler.cpp`:

```cpp
namespace markamp::core {

class ScopedTimer
{
public:
    explicit ScopedTimer(std::string_view name);
    ~ScopedTimer();
    // Non-copyable, non-movable
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_;
};

class Profiler
{
public:
    static auto instance() -> Profiler&;

    // Manual timing
    void begin(std::string_view name);
    void end(std::string_view name);

    // Scoped timing
    [[nodiscard]] auto scope(std::string_view name) -> ScopedTimer;

    // Results
    struct TimingResult {
        std::string name;
        double avg_ms;
        double min_ms;
        double max_ms;
        size_t call_count;
    };
    [[nodiscard]] auto results() const -> std::vector<TimingResult>;
    void reset();
    void dump_to_log();

    // Memory tracking
    [[nodiscard]] auto memory_usage_mb() const -> double;

private:
    struct TimingData {
        std::vector<double> durations_ms;
    };
    std::unordered_map<std::string, TimingData> timings_;
    std::mutex mutex_;
};

// Convenience macro
#define MARKAMP_PROFILE_SCOPE(name) \
    auto _profiler_##__LINE__ = markamp::core::Profiler::instance().scope(name)

} // namespace markamp::core
```

**Instrumentation points:**
- Application startup (total and per-phase)
- Theme switching
- File loading
- Markdown parsing
- HTML rendering
- Mermaid rendering
- Preview update
- Editor keystroke handling
- Sidebar rendering
- Window resize

**Acceptance criteria:**
- Profiler captures timing data for all instrumented operations
- Results can be dumped to log
- Overhead of profiling is negligible (under 1% impact)

### Task 26.2: Optimize cold start time

Target: Application window visible in under 500ms from launch.

**Profiling the startup:**
1. `main()` entry
2. wxWidgets initialization
3. Config loading
4. Theme system initialization
5. Font loading
6. Window creation
7. Layout setup
8. Initial file loading
9. Initial markdown parse and render
10. First paint

**Optimization strategies:**
1. **Lazy initialization:** Defer non-critical subsystems
   - Mermaid renderer: initialize on first use, not at startup
   - Syntax highlighter: initialize language definitions on demand
   - Theme gallery: create on first open, not at startup
2. **Parallel initialization:**
   - Load config and fonts simultaneously
   - Parse and render initial content in parallel with window setup
3. **Reduce font loading time:**
   - Embed fonts as binary data (compile-time), not load from disk
   - Or cache font loading results
4. **Precomputed resources:**
   - Pre-render theme gallery card previews on first gallery open
   - Cache markdown parse results for sample files
5. **Minimize allocations:**
   - Use memory pools for frequently allocated objects
   - Pre-allocate string buffers

**Acceptance criteria:**
- Cold start measured from `main()` to first visible paint under 500ms
- Measurable on macOS, Windows, Linux
- No visible loading delay (window appears quickly with content)

### Task 26.3: Optimize markdown rendering pipeline

Profile and optimize the markdown -> HTML -> display pipeline:

**Bottleneck analysis:**
1. Markdown parsing (md4c): likely fast, profile to confirm
2. AST traversal and HTML generation: optimize string concatenation
3. CSS generation: cache between theme changes
4. HTML display (wxHtmlWindow/wxWebView): profile rendering time
5. Mermaid rendering: async, already addressed

**Optimization strategies:**
1. **Incremental parsing (if feasible):**
   - Only re-parse changed sections of the document
   - md4c does not natively support incremental parsing, so this requires a change-detection layer:
     - Compare old and new content
     - If change is within a single block, re-parse only that block
     - Merge result into existing AST
2. **HTML string building:**
   - Use `std::string::reserve()` with estimated size
   - Or use a `std::stringstream` with adequate buffer
   - Avoid repeated small allocations
3. **CSS caching:**
   - Generate CSS once per theme change, not per render
   - Store the CSS string and reuse it
4. **Debounced rendering:**
   - Already implemented in Phase 14 (300ms debounce)
   - Tune the debounce interval based on profiling
5. **wxHtmlWindow optimization:**
   - Use `Freeze()` / `Thaw()` around content updates
   - Disable scroll events during update

**Performance targets:**
- Parse 10,000-line markdown: under 50ms
- Render HTML for 10,000-line document: under 100ms
- Total pipeline (parse + render + display): under 200ms

**Acceptance criteria:**
- Rendering pipeline meets performance targets
- Large documents render without blocking the UI
- Typing in a large document does not cause visible lag in the preview

### Task 26.4: Optimize editor performance for large files

Ensure the editor handles large files smoothly:

**Scintilla optimizations:**
```cpp
// Already applied in Phase 20, verify and tune:
editor_->SetLayoutCache(wxSTC_CACHE_DOCUMENT);
editor_->SetBufferedDraw(true);
editor_->SetIdleStyling(wxSTC_IDLESTYLING_ALL); // Style while idle
```

**Additional optimizations:**
1. **Disable unnecessary features for large files:**
   - Syntax highlighting: off for files > 50,000 lines
   - Bracket matching: off for files > 100,000 lines
   - Auto-indent: always on (lightweight)
2. **Optimize content change notifications:**
   - Increase debounce time for large files (500ms instead of 300ms)
   - Skip preview updates during rapid scrolling
3. **Memory-mapped file loading (for very large files):**
   - For files > 10MB: use memory mapping instead of loading entire file into memory
   - wxStyledTextCtrl loads into its own buffer; pass content efficiently

**Performance targets:**
- Open 10,000-line file: under 500ms
- Typing latency in 10,000-line file: under 16ms (60fps)
- Scrolling in 100,000-line file: smooth at 60fps

**Acceptance criteria:**
- Performance targets met
- No visible lag when typing in large files
- Scrolling is smooth for all tested file sizes

### Task 26.5: Optimize Mermaid rendering

Ensure Mermaid diagrams render without blocking the UI:

**Optimization strategies:**
1. **Background thread rendering:**
   - Render Mermaid in a dedicated thread
   - Show placeholder while rendering
   - Update preview when result is ready
2. **Caching:**
   - Cache rendered SVG keyed by (mermaid_source_hash, theme_id)
   - Reuse cached SVG when content hasn't changed
   - Invalidate cache only on content change or theme change
3. **Render throttling:**
   - Do not re-render Mermaid on every keystroke
   - Only re-render when the mermaid block content changes
   - Use a separate longer debounce (500ms) for Mermaid re-renders
4. **Timeout protection:**
   - Cancel rendering after 10 seconds
   - Show error: "Diagram too complex to render"

**Acceptance criteria:**
- Mermaid rendering does not block the UI thread
- Cached diagrams display instantly
- Complex diagrams show placeholder while rendering
- Timeout protection prevents infinite loops

### Task 26.6: Optimize memory usage

Profile and reduce memory consumption:

**Memory profiling:**
- Use ASAN leak detection
- Track peak memory usage during various operations
- Identify memory-heavy data structures

**Optimization strategies:**
1. **String optimization:**
   - Use `std::string_view` where ownership is not needed
   - Use small-string optimization (SSO) -- default in most std::string implementations
   - For very large documents: consider rope data structure
2. **AST optimization:**
   - Use a memory pool for MdNode allocation
   - Or use a flat vector with indices instead of pointers
3. **Theme resource management:**
   - Share wxBrush/wxPen/wxFont objects across components
   - ThemeEngine already caches these (verify they are shared, not duplicated)
4. **Image handling:**
   - Load images lazily (only when visible in viewport)
   - Unload images when scrolled out of view
   - Limit total image memory to 100MB

**Memory targets:**
- Idle application: under 50MB
- With a 10,000-line document: under 100MB
- With 10 Mermaid diagrams: under 200MB

**Acceptance criteria:**
- Memory usage meets targets
- No memory leaks (ASAN clean)
- Memory does not grow unbounded with repeated operations

### Task 26.7: Create performance benchmarks

Create `/Users/ryanrentfro/code/markamp/tests/performance/benchmark_suite.cpp`:

**Benchmarks:**
1. Cold start time (time from main() to first paint)
2. Markdown parse time for various document sizes (100, 1K, 10K, 100K lines)
3. HTML render time for various document sizes
4. Theme switch time
5. File load time for various file sizes
6. Mermaid render time for various diagram complexities
7. Editor keystroke latency
8. Editor scroll performance (fps measurement)
9. Preview update latency (content change to visible update)
10. Memory usage at steady state

**Benchmark framework:**
Use Catch2's `BENCHMARK` macro or a standalone benchmark framework:
```cpp
BENCHMARK("parse_10k_lines") {
    auto doc = parser.parse(generate_markdown(10000));
    return doc;
};
```

**Acceptance criteria:**
- All benchmarks produce reproducible measurements
- Results are logged in a machine-readable format
- Regressions can be detected by comparing benchmark results

### Task 26.8: Write performance regression tests

Create `/Users/ryanrentfro/code/markamp/tests/performance/test_performance_regression.cpp`:

Tests with hard thresholds:
1. Cold start: assert under 2000ms (generous for CI, target 500ms locally)
2. Parse 1K lines: assert under 10ms
3. Parse 10K lines: assert under 100ms
4. Render 1K lines: assert under 50ms
5. Theme switch: assert under 100ms
6. Editor keystroke: assert under 16ms

These tests serve as regression guards: if a code change pushes a metric above the threshold, the test fails.

**Acceptance criteria:**
- All regression tests pass
- Thresholds are reasonable and documented
- Tests can run in CI

## Files Created/Modified

| File | Action |
|---|---|
| `src/core/Profiler.h` | Created |
| `src/core/Profiler.cpp` | Created |
| `src/app/MarkAmpApp.cpp` | Modified (startup profiling) |
| `src/core/MarkdownParser.cpp` | Modified (optimization) |
| `src/rendering/HtmlRenderer.cpp` | Modified (optimization) |
| `src/core/MermaidRenderer.cpp` | Modified (caching, async) |
| `src/ui/EditorPanel.cpp` | Modified (large file optimization) |
| `src/ui/PreviewPanel.cpp` | Modified (rendering optimization) |
| `tests/performance/benchmark_suite.cpp` | Created |
| `tests/performance/test_performance_regression.cpp` | Created |
| `tests/CMakeLists.txt` | Modified |
| `src/CMakeLists.txt` | Modified |

## Dependencies

- All rendering sub-systems
- ASAN (memory leak detection)
- Catch2 (benchmarking)
- Platform profiling tools (Instruments on macOS, perf on Linux, VTune on Windows)

## Estimated Complexity

**High** -- Performance optimization requires profiling, identifying bottlenecks, and implementing targeted fixes. Some optimizations (incremental parsing, memory-mapped files) are architecturally significant. Ensuring optimizations do not introduce bugs requires careful testing.
