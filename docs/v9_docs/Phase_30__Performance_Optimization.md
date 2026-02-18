# Phase 30: Performance Optimization

## Overview
StartupTimer, TracyIntegration, AdaptiveThrottle, ViewportCache, GlyphAdvanceCache, HitTestAccelerator, TileCache, CoalescingTask, Backpressure, SPSCQueue, RingBuffer, and Watchdog exist as performance infrastructure. The application eagerly constructs 21 services in OnInit(). EventBus publish_fast() still holds a mutex. This phase optimizes startup, runtime, and rendering performance.

## Prerequisites
- Phase 01 (EventBus hardening)
- Phase 28 (FX system for rendering pipeline)
- Phase 29 (Security for bounded collections)

## Tasks

### Task 1: Wire Lazy Service Initialization
**Files:** `src/app/MarkAmpApp.cpp`, `src/core/StartupPhase.h`
**Description:** 21 services are eagerly constructed in OnInit(). Wire lazy initialization: only construct services when first accessed. Group into startup phases: essential (EventBus, Config), deferred (Extension services), lazy (AI, Sync).
**Acceptance Criteria:**
- Essential services (5): EventBus, Config, Platform, ThemeEngine, PluginManager
- Deferred services (10): Extension services loaded after first paint
- Lazy services (6): AI, Sync, Export loaded on first use
- StartupTimer measures each phase
- First paint within 200ms
- All services available within 2 seconds

### Task 2: Wire StartupTimer for Phase Measurement
**Files:** `src/core/StartupTimer.cpp`, `src/core/StartupTimer.h`
**Description:** StartupTimer exists. Wire it to measure each startup phase: config load, theme apply, service creation, plugin scan, first paint, workspace load.
**Acceptance Criteria:**
- Phase timing: config (ms), theme (ms), services (ms), plugins (ms), first paint (ms)
- Total startup time logged
- Timing available in output channel: "Performance"
- Regression detection: warn if startup > 3 seconds
- `StartupCompletedEvent` emitted with timing

### Task 3: Wire ViewportCache for Editor Rendering
**Files:** `src/rendering/ViewportCache.h`, `src/ui/EditorPanel.cpp`
**Description:** ViewportCache exists. Wire it to cache rendered lines in the editor viewport, avoiding re-render on scroll when content hasn't changed.
**Acceptance Criteria:**
- Cache rendered lines in visible viewport
- Scroll within cached range: no re-render
- Content change: invalidate affected lines only
- Cache size: viewport + 2x buffer above/below
- Cache hit rate > 90% during normal editing
- Memory: < 10MB for cached viewport

### Task 4: Wire GlyphAdvanceCache for Text Measurement
**Files:** `src/rendering/GlyphAdvanceCache.h`, `src/ui/EditorPanel.cpp`
**Description:** GlyphAdvanceCache exists. Wire it to cache character width measurements, avoiding repeated font metric queries during text layout.
**Acceptance Criteria:**
- Cache glyph widths per font/size combination
- Cache invalidated on font/size change
- Hit rate: > 99% during normal typing
- Memory: < 1MB for all cached measurements
- Measurably faster text layout in profiling

### Task 5: Wire TileCache for Canvas Rendering
**Files:** `src/canvas/TileCache.cpp`, `src/canvas/TileCache.h`, `src/canvas/CanvasRenderer.cpp`
**Description:** TileCache exists. Wire it to cache rendered canvas tiles, avoiding re-render of static regions when panning.
**Acceptance Criteria:**
- Canvas divided into tiles (256x256px)
- Static tiles cached as bitmaps
- Pan: compose from cached tiles
- Dirty tiles invalidated on object change
- Memory budget: 100MB for tile cache
- Cache eviction: LRU for off-screen tiles

### Task 6: Wire HitTestAccelerator for Canvas Input
**Files:** `src/rendering/HitTestAccelerator.h`, `src/canvas/CanvasInputManager.cpp`
**Description:** HitTestAccelerator exists (likely wrapping the quadtree). Wire it for O(log n) hit testing during mouse events on canvas.
**Acceptance Criteria:**
- Hit test uses spatial index (quadtree)
- Mouse move: < 0.1ms per hit test
- 1000 objects: no perceptible delay
- Index updated on object move/resize/create/delete
- Viewport culling: only test visible objects

### Task 7: Wire AdaptiveThrottle for Event Processing
**Files:** `src/core/AdaptiveThrottle.h`, `src/core/EventBus.cpp`
**Description:** AdaptiveThrottle exists. Wire it to throttle high-frequency events (mouse move, scroll, resize) to prevent event flooding.
**Acceptance Criteria:**
- Mouse move events throttled to 60Hz
- Scroll events throttled to 120Hz
- Resize events throttled to 30Hz
- Throttle adapts: lower rate when processing is slow
- Final event always delivered (trailing edge)
- Throttle bypass for critical events

### Task 8: Wire CoalescingTask for Debounced Operations
**Files:** `src/core/CoalescingTask.h`
**Description:** CoalescingTask exists. Wire it for debounced operations: auto-save, search-as-you-type, syntax highlighting, config write.
**Acceptance Criteria:**
- Auto-save: 2 second debounce, coalesces rapid saves
- Search: 250ms debounce, cancels previous search
- Highlighting: 100ms debounce after edit
- Config write: 1 second debounce
- All coalescing tasks cancelable
- Task execution on main thread

### Task 9: Wire Backpressure for Async Pipelines
**Files:** `src/core/Backpressure.h`, `src/core/AsyncFileLoader.cpp`
**Description:** Backpressure exists. Wire it to prevent overwhelming consumers: file loading, search indexing, sync uploading.
**Acceptance Criteria:**
- File loading: max 5 concurrent loads
- Search indexing: max 1 concurrent index operation
- Sync upload: max 4 concurrent uploads
- Queue depth limit: 100 pending operations
- Backpressure: slow down producer when queue full
- Metrics: queue depth, processing rate, wait time

### Task 10: Wire SPSCQueue for Lock-Free Event Delivery
**Files:** `src/core/SPSCQueue.h`, `src/core/EventBus.cpp`
**Description:** SPSCQueue (single-producer single-consumer) exists. Wire it to replace mutex-protected queued_events_ in EventBus for lock-free cross-thread event delivery.
**Acceptance Criteria:**
- EventBus fast path uses SPSCQueue
- No mutex on publish_fast() path
- Main thread drains queue on idle
- Queue size: 4096 entries (power of 2)
- Overflow: fallback to mutex-protected queue
- Benchmark: 10x throughput improvement

### Task 11: Wire Watchdog for Hang Detection
**Files:** `src/core/Watchdog.h`, `src/app/MarkAmpApp.cpp`
**Description:** Watchdog exists. Wire it to detect main thread hangs: if main thread doesn't heartbeat within timeout, log warning and optionally break to debugger.
**Acceptance Criteria:**
- Watchdog runs on separate thread
- Main thread heartbeats every 100ms (via idle handler)
- Warning: if no heartbeat for 2 seconds
- Critical: if no heartbeat for 5 seconds (dump stack)
- Watchdog disabled in debug builds
- Hang report includes stack trace if available

### Task 12: Wire Memory Budget Enforcement
**Files:** `src/core/AllocatorConfig.h`, `src/core/SafeAllocation.h`
**Description:** Enforce memory budgets across subsystems: editor (50MB), canvas (200MB), search index (100MB), extensions (50MB per).
**Acceptance Criteria:**
- Per-subsystem memory tracking
- Budget exceeded: warning notification
- Hard limit: graceful degradation (evict caches)
- Memory usage visible in debug overlay
- Total memory target: < 500MB for typical workspace
- Memory statistics in "Performance" output channel

### Task 13: Wire Rendering Pipeline Optimization
**Files:** `src/rendering/RenderPipeline.h`, `src/rendering/DirtyRegion.h`
**Description:** RenderPipeline and DirtyRegion exist. Wire dirty region tracking: only re-render changed regions, not full viewport.
**Acceptance Criteria:**
- Dirty region tracking per panel
- Only dirty regions re-rendered
- Cursor blink: tiny dirty region (not full line)
- Typing: dirty region is affected line(s) only
- Scrolling: compose from cached + render new strip
- Frame budget: < 8ms for render of dirty regions

### Task 14: Wire Incremental Markdown Parsing
**Files:** `src/core/MarkdownParser.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Full document re-parse on every edit is expensive for large files. Wire incremental parsing: only re-parse changed lines, update AST incrementally.
**Acceptance Criteria:**
- Single line edit: re-parse that line + context (< 5 lines)
- Multi-line edit: re-parse affected range
- Block-level invalidation (code blocks, lists)
- Full re-parse only on file open
- Parse time for single line edit: < 5ms
- 10,000 line document editable at 60fps

### Task 15: Wire Search Index Performance
**Files:** `src/core/ContentIndexer.cpp`, `src/core/SearchEngine.cpp`
**Description:** Optimize search index: incremental index updates (not full rebuild), memory-mapped index, concurrent query support.
**Acceptance Criteria:**
- Single file update: < 10ms
- Full workspace index (1000 files): < 5 seconds
- Query time: < 50ms for simple queries
- Memory-mapped index for fast startup
- Concurrent queries supported (read-lock)
- Index size: < 10% of workspace size

### Task 16: Wire Canvas Performance for Large Boards
**Files:** `src/canvas/CanvasRenderer.cpp`, `src/canvas/Board.cpp`
**Description:** Optimize canvas for large boards: viewport culling (only render visible objects), level-of-detail (distant objects simplified), batched rendering.
**Acceptance Criteria:**
- 1000 objects: 60fps rendering
- Viewport culling: only visible objects rendered
- Level-of-detail: distant objects as colored rectangles
- Batched rendering: same-type objects rendered together
- Zoom out: progressive simplification
- Memory: < 50MB for 1000-object board

### Task 17: Wire Startup Profiling
**Files:** `src/core/TracyIntegration.h`, `src/app/MarkAmpApp.cpp`
**Description:** TracyIntegration exists. Wire Tracy profiler markers at all startup phases for detailed performance analysis.
**Acceptance Criteria:**
- Tracy zones for each startup phase
- Tracy zones for service construction
- Tracy zones for plugin loading
- Tracy zones for theme application
- Frame markers for first paint timing
- Tracy connection: localhost:8086

### Task 18: Wire Performance Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register performance commands: "Performance: Show Metrics", "Performance: Toggle Debug Overlay", "Performance: Report Startup Time", "Performance: Profile Next 60 Frames".
**Acceptance Criteria:**
- All commands registered in command palette
- "Show Metrics" opens performance output channel
- "Debug Overlay" shows FPS, memory, frame time
- "Startup Time" reports phase timings
- "Profile" captures and reports frame timing

### Task 19: Wire Performance Settings
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.h`
**Description:** Performance settings: hardware acceleration toggle, render quality, memory budget, auto-save interval, search index update frequency.
**Acceptance Criteria:**
- Hardware acceleration: on/off
- Render quality: low/medium/high
- Memory budget: conservative (300MB), moderate (500MB), generous (1GB)
- Auto-save interval: 1/2/5/10 seconds
- Search index update: real-time/on-save/manual

### Task 20: Add Performance Tests
**Files:** `tests/unit/test_performance_infra.cpp`, `tests/unit/test_phase20_perf.cpp`
**Description:** Test performance infrastructure: benchmarks for critical paths, regression detection, memory budget enforcement.
**Acceptance Criteria:**
- Startup time benchmark: < 3 seconds target
- Editor typing latency: < 16ms (60fps)
- Search query: < 100ms for 1000-file workspace
- Canvas render: < 16ms for 200 objects
- Memory: within budget for each subsystem
- Benchmarks automated in CI

## Testing Requirements
- Startup time measurement and regression detection
- Editor typing latency benchmark
- Canvas rendering benchmark
- Search performance benchmark
- Memory budget compliance

## Phase Completion Criteria
- Lazy service initialization reduces startup time
- ViewportCache and TileCache reduce re-rendering
- SPSCQueue removes mutex from fast path
- Dirty region rendering for editor
- Canvas viewport culling for large boards
- Memory budgets enforced
- All tests pass
