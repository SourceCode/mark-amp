# Phase 49 -- Performance Dashboard and Monitoring

## Objective

Build an integrated performance dashboard and monitoring system that provides real-time visibility into application performance: frame timing, render call counts, memory usage, EventBus throughput, file cache statistics, startup timing, extension load times, and performance regression detection. Extends the existing EditorPerformanceBudget, FrameScheduler, and AdaptiveThrottle infrastructure into a comprehensive developer-facing performance observability surface.

## Prerequisites

- Phase 48 complete (Welcome and Onboarding Experience)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h` -- budget thresholds, violation detection
- `/Users/ryanrentfro/code/markamp/src/core/FrameScheduler.h` -- frame-budget-aware task scheduling
- `/Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h` -- typing/idle mode switching
- `/Users/ryanrentfro/code/markamp/src/core/Logger.h` -- existing logging infrastructure
- Phase 41 components (ThemedProgressBar, Badge, SegmentedControl)

## VSCode / Chrome DevTools Reference Behavior

- Chrome DevTools Performance tab: frame time graph, flamechart, call tree
- VSCode Developer Tools: extension host profiling, startup timeline
- Frame budget: 16ms target line with green (under) and red (over) coloring
- Memory usage: heap graph with GC markers
- Live updating metrics with 1-second refresh rate

## Target Files

| File | Action |
|------|--------|
| `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/FrameTimeGraph.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/FrameTimeGraph.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/MetricsCollector.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/MetricsCollector.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/StartupTimeline.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/StartupTimeline.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/PaintRegionOverlay.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/PaintRegionOverlay.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/tests/unit/test_performance_dashboard.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/CMakeLists.txt` | Modify |

## Tasks

### Task 01 -- Create PerformanceCounters Core Service

**Description:** Create a centralized service for collecting and aggregating performance metrics from all parts of the application.

**Implementation Details:** PerformanceCounters is a thread-safe service using atomic counters and lock-free ring buffers. Metrics: `frame_time_us` (per-frame), `render_call_count` (per-frame), `paint_region_count` (per-frame), `event_bus_publish_count` (per-second), `event_bus_latency_us` (per-event), `memory_usage_bytes` (sampled per-second), `file_cache_hits` (cumulative), `file_cache_misses` (cumulative), `extension_load_time_ms` (per-extension), `startup_phase_ms` (per-phase). Ring buffer stores last 300 samples (5 minutes at 1 sample/second for aggregate metrics, 300 frames for per-frame metrics). Thread safety via `std::atomic` for counters and `core::SPSCQueue` for frame timing.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.cpp` (create)

**Acceptance Criteria:**
- All metric types are collected and stored
- Ring buffer holds last 300 samples per metric
- Thread-safe reads and writes (no data races)
- Lock-free design for hot paths (frame timing)
- Metrics are queryable by type and time range

**Dependencies:** None

---

### Task 02 -- Instrument Frame Timing

**Description:** Add instrumentation to the main frame loop to record frame time (the time from the start of a frame to the end of the paint cycle).

**Implementation Details:** In the MainFrame's idle/paint handler (or FrameScheduler.run_frame()), record the wall-clock time from frame start to frame end. Push each frame time to PerformanceCounters::record_frame_time(). The measurement uses `std::chrono::high_resolution_clock` for microsecond precision. Record both total frame time and component breakdown: input processing time, layout time, paint time, background task time. These breakdowns enable identifying which phase of the frame is taking the most time.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/FrameScheduler.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` (modify)

**Acceptance Criteria:**
- Frame time recorded for every frame with microsecond precision
- Component breakdown: input, layout, paint, background
- Measurement overhead is under 1us per frame
- Frame times are pushed to PerformanceCounters
- Recording can be enabled/disabled at runtime

**Dependencies:** Task 01

---

### Task 03 -- Instrument EventBus Throughput

**Description:** Add instrumentation to EventBus to count publishes per second and measure average event dispatch latency.

**Implementation Details:** In EventBus::publish() and EventBus::publish_fast(), increment an atomic counter for publish count. Record the time from publish start to the completion of all subscriber callbacks (dispatch latency). Push latency samples to PerformanceCounters. Add a "slow event" threshold (configurable, default 5ms): events taking longer than the threshold are flagged and their type names are logged. A per-second aggregation timer summarizes: total publishes, average latency, max latency, slow event count. This should not impact the EventBus hot path significantly (atomic increment is <1ns).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/EventBus.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/EventBus.cpp` (modify)

**Acceptance Criteria:**
- Publish count tracked per second
- Dispatch latency measured per event
- Slow events flagged when exceeding threshold
- Measurement overhead under 100ns per publish
- Per-second aggregation available for dashboard

**Dependencies:** Task 01

---

### Task 04 -- Instrument Memory Usage

**Description:** Sample the application's memory usage (resident set size) periodically and track allocation trends.

**Implementation Details:** Every 1 second, query the OS for the process's memory usage: `mach_task_basic_info` on macOS, `GetProcessMemoryInfo` on Windows, `/proc/self/status` on Linux. Push to PerformanceCounters as `memory_usage_bytes`. Track: RSS (resident set size), virtual memory, heap size (if available). Calculate delta: `memory_delta_bytes = current - previous` for detecting leaks. If memory delta exceeds 10MB in 60 seconds, flag as potential leak. Platform-specific code goes in the platform abstraction layer.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/platform/MacPlatform.mm` (modify)

**Acceptance Criteria:**
- Memory usage sampled every second
- RSS, virtual, and heap tracked
- Memory delta detects growth trends
- 10MB/60s growth flags potential leak
- Platform-specific queries work on macOS

**Dependencies:** Task 01

---

### Task 05 -- Instrument File Cache Statistics

**Description:** Track file cache hit/miss ratios and cache size to understand file I/O performance.

**Implementation Details:** Instrument the AsyncFileLoader and any file caching layer to record: cache hits (file loaded from cache), cache misses (file loaded from disk), cache evictions, cache size (number of entries, total bytes). Push to PerformanceCounters. Calculate hit rate: `hits / (hits + misses) * 100`. A low hit rate (<80%) suggests the cache is too small. Track the 10 most frequently accessed files for debugging.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/AsyncFileLoader.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.h` (modify)

**Acceptance Criteria:**
- Hit and miss counts are accurate
- Hit rate percentage calculated correctly
- Cache size (entries and bytes) tracked
- Top 10 most accessed files tracked
- Low hit rate generates warning

**Dependencies:** Task 01

---

### Task 06 -- Instrument Startup Timing

**Description:** Record the duration of each startup phase: initialization, config loading, theme loading, extension loading, window creation, first render.

**Implementation Details:** Add `StartupTimeline` to PerformanceCounters. Each phase is recorded with `begin_phase(phase_name)` and `end_phase(phase_name)`. Phases: "Init" (MarkAmpApp::OnInit start to EventBus creation), "Config" (Config loading), "Theme" (ThemeEngine + built-in themes), "Extensions" (extension scanning + loading), "Window" (MainFrame creation), "FirstRender" (first paint event). Total startup time is the sum of all phases. The timeline is recorded once per launch and persisted to a log file for trend analysis. Display the startup breakdown in the Performance Dashboard.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/StartupTimeline.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/StartupTimeline.cpp` (create)
- `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp` (modify)

**Acceptance Criteria:**
- All startup phases are timed
- Total startup time is calculated
- Timeline is recorded once per launch
- Phase durations are available for dashboard
- Startup log persists for trend analysis

**Dependencies:** Task 01

---

### Task 07 -- Instrument Extension Load Times

**Description:** Track how long each extension takes to load and activate, identifying slow extensions.

**Implementation Details:** In the extension loading pipeline (ExtensionScanner, PluginManager), wrap each extension's load and activate calls with timing. Record: `extension_name`, `load_time_ms`, `activate_time_ms`, `total_time_ms`. Push to PerformanceCounters. Extensions taking >500ms to load are flagged as "slow". The dashboard shows a sorted list of extensions by load time. A "Disable Slow Extensions" action is available for extensions exceeding the threshold.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/ExtensionScanner.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.h` (modify)

**Acceptance Criteria:**
- Each extension's load and activate time is recorded
- Slow extensions (>500ms) are flagged
- Dashboard shows sorted extension list
- "Disable Slow Extensions" action is available
- Timing does not affect extension loading

**Dependencies:** Task 01

---

### Task 08 -- Create PerformanceDashboard Panel

**Description:** Create the main Performance Dashboard panel that aggregates all metrics into a developer-facing monitoring surface, accessible via Command Palette.

**Implementation Details:** PerformanceDashboard extends ThemeAwareWindow and registers as a tool window panel (via ToolWindowHost) with title "Performance". Layout: (1) top row: key metrics cards (frame time avg, memory usage, EventBus events/sec, cache hit rate), (2) middle: FrameTimeGraph (Task 09), (3) bottom: tabbed sections (Startup Timeline, Extension Load Times, Event Details, Memory Details). Refreshes every 1 second via wxTimer. Accessible via Command Palette: "Developer: Open Performance Dashboard". Only available in debug builds (behind `MARKAMP_DEBUG` flag) or when `developer.performanceDashboard` setting is enabled.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.cpp` (create)

**Acceptance Criteria:**
- Dashboard renders all metric categories
- Key metrics cards show current values
- Tabbed sections show detailed breakdowns
- Refreshes every second
- Accessible via Command Palette

**Dependencies:** Tasks 01-07

---

### Task 09 -- Create FrameTimeGraph

**Description:** Create a real-time graph that displays frame time history with a 16ms target line, colored green (under budget) and red (over budget).

**Implementation Details:** FrameTimeGraph extends ThemeAwareWindow. Width: fills parent. Height: 120px. X-axis: last 300 frames (scrolling). Y-axis: 0ms to 33ms (2x budget). Each frame renders as a vertical bar: green if under 16ms, yellow if 16-24ms, red if over 24ms. A horizontal line at 16ms (the 60fps target) renders in accent color. Average frame time and 95th percentile values show in the top-right corner. The graph renders using wxGraphicsContext for smooth anti-aliased lines. Data comes from PerformanceCounters frame time ring buffer.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FrameTimeGraph.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/FrameTimeGraph.cpp` (create)

**Acceptance Criteria:**
- Graph shows last 300 frames as vertical bars
- 16ms target line renders horizontally
- Bars colored green/yellow/red based on frame time
- Average and P95 values shown
- Graph scrolls as new frames arrive

**Dependencies:** Task 02

---

### Task 10 -- Implement Render Call Counter

**Description:** Count the number of wxDC draw calls per frame and display in the dashboard.

**Implementation Details:** Create a scoped counter that increments on each wxDC method call during a frame. This is implemented as a `RenderCallTracker` RAII object created at frame start: counts calls to `DrawRectangle`, `DrawText`, `DrawBitmap`, `DrawLine`, etc. by wrapping or instrumenting the wxDC usage. A simpler alternative: count `OnPaint` invocations across all panels per frame (track how many panels repainted). Push the count to PerformanceCounters. Display in the dashboard as "Render Calls: N/frame".

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` (modify)

**Acceptance Criteria:**
- Render call count tracked per frame
- Count includes all panel paints
- Dashboard displays current count
- High counts (>50/frame) generate warning
- Counter has minimal overhead

**Dependencies:** Task 01

---

### Task 11 -- Create PaintRegionOverlay (Debug Mode)

**Description:** Create a debug overlay that visualizes which regions of the screen were repainted in each frame, helping identify excessive repainting.

**Implementation Details:** PaintRegionOverlay is a transparent wxPanel covering the MainFrame. When enabled, it draws colored rectangles over each repainted region: green for normal repaints, red for full-window repaints, yellow for partial repaints. Rectangles fade out over 500ms. This helps developers identify controls that repaint too frequently. Toggled via Command Palette: "Developer: Toggle Paint Regions". Each painted region also shows its wxWindow class name in small text. The overlay itself does not trigger repaints of underlying content (uses `wxCLIP_CHILDREN`).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PaintRegionOverlay.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/PaintRegionOverlay.cpp` (create)

**Acceptance Criteria:**
- Colored rectangles show repainted regions
- Rectangles fade out over 500ms
- Full-window repaints highlighted in red
- Widget class names shown per region
- Toggle via Command Palette

**Dependencies:** None

---

### Task 12 -- Implement GPU Acceleration Status Display

**Description:** Detect and display whether GPU acceleration is active for rendering.

**Implementation Details:** Query the wxGraphicsRenderer to determine if hardware acceleration is in use. On macOS: check if Metal or CoreGraphics is the active renderer. On Windows: check if Direct2D is active. Display status in the dashboard: "GPU Acceleration: Active (Metal)" or "GPU Acceleration: Inactive (Software)". If acceleration is inactive, suggest enabling it in settings. Also display the graphics backend version and renderer name.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.cpp` (modify)

**Acceptance Criteria:**
- GPU acceleration status detected correctly
- Backend name and version displayed
- Inactive acceleration shows suggestion
- Works on macOS, Windows, and Linux
- Status updates if renderer changes

**Dependencies:** Task 08

---

### Task 13 -- Implement Lazy Panel Creation Status

**Description:** Display which panels have been lazily created vs. eagerly created, showing the impact of lazy loading on startup time.

**Implementation Details:** Track panel creation timing in ToolWindowHost and PaneManager. For each panel: record whether it was created at startup (eager) or on first use (lazy), and the creation time in milliseconds. Display in the dashboard as a table: Panel Name, Creation Mode, Creation Time (ms), Status (Created/Pending). Highlight panels that took >100ms to create. Calculate total startup time saved by lazy loading: sum of all "Pending" panel creation times.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/ToolWindowHost.cpp` (modify)

**Acceptance Criteria:**
- Each panel's creation mode is tracked
- Creation times recorded for all panels
- Lazy vs eager creation clearly shown
- Total startup time savings calculated
- Slow panels (>100ms) highlighted

**Dependencies:** Task 08

---

### Task 14 -- Implement Slow Event Detection and Logging

**Description:** Detect EventBus events that take longer than the slow event threshold and log them with details for debugging.

**Implementation Details:** When an event dispatch exceeds the slow threshold (default 5ms, configurable), log: event type name, subscriber count, total dispatch time, individual subscriber times (to identify the slow subscriber). The dashboard shows a "Slow Events" tab with a scrollable list of recent slow events. Each entry: timestamp, event type, dispatch time, slowest subscriber. A "Clear" button resets the list. The threshold is configurable via `developer.slowEventThreshold` setting. Severe slow events (>50ms) trigger a notification via NotificationService.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/EventBus.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.cpp` (modify)

**Acceptance Criteria:**
- Slow events detected when exceeding threshold
- Event type, subscriber count, and timing logged
- Individual subscriber times available
- Dashboard shows slow event list
- Severe slow events (>50ms) notify the user

**Dependencies:** Task 03

---

### Task 15 -- Implement Layout Reflow Counter

**Description:** Count and display the number of layout reflows (sizer recalculations) per frame, helping identify excessive layout thrashing.

**Implementation Details:** Instrument wxSizer::Layout() calls (or wxWindow::Layout()) to count reflows per frame. A reflow is expensive because it measures and positions all child controls. More than 2 reflows per frame suggests layout thrashing. Push count to PerformanceCounters. Display in the dashboard. When reflow count exceeds 5 per frame, log the call stack to help identify the trigger. Track which panel initiated each reflow.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` (modify)

**Acceptance Criteria:**
- Reflow count tracked per frame
- Dashboard displays current count
- >5 reflows/frame triggers warning log
- Initiating panel identified per reflow
- Counter has minimal overhead

**Dependencies:** Task 01

---

### Task 16 -- Implement Performance Profiling Toggle

**Description:** Add a toggle to enable detailed performance profiling that captures more granular data at the cost of some overhead.

**Implementation Details:** Normal mode: only lightweight metrics (atomic counters, ring buffers). Profiling mode: adds call timing for every EventBus subscriber, detailed wxDC call tracking, per-control paint timing, and memory allocation tracking. Toggled via Command Palette: "Developer: Toggle Performance Profiling". When enabled, a red "Recording" indicator appears in the status bar. Profiling data is stored in a temporary buffer (max 60 seconds). When profiling stops, the data can be exported.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (modify)

**Acceptance Criteria:**
- Toggle enables/disables detailed profiling
- Red "Recording" indicator in status bar during profiling
- Profiling data buffered for 60 seconds max
- Normal mode overhead under 1% CPU
- Profiling mode overhead under 10% CPU

**Dependencies:** Task 01

---

### Task 17 -- Implement Performance Report Export

**Description:** Export collected performance data as a JSON report for analysis, sharing, and CI comparison.

**Implementation Details:** Add "Export Report" button in the PerformanceDashboard. The report includes: timestamp, app version, OS info, hardware info, startup timeline, frame time statistics (avg, P50, P95, P99), memory usage trend, EventBus statistics, file cache stats, extension load times, slow events list. Export as JSON to a user-selected location. The report format is versioned (schema version field) for forward compatibility. Also support automated export via command: `developer.exportPerformanceReport` which saves to a default location.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.cpp` (modify)

**Acceptance Criteria:**
- Report contains all metric categories
- JSON format with schema version
- Automated export works via command
- Report includes hardware and OS info
- Report is human-readable and machine-parseable

**Dependencies:** Task 08

---

### Task 18 -- Implement Performance Regression Alerts

**Description:** Detect performance regressions by comparing current metrics against historical baselines and alerting the developer.

**Implementation Details:** On startup, load the previous performance report (if it exists). Compare key metrics: startup time, average frame time, P95 frame time, memory usage at idle. If any metric regresses by more than 20%, generate a warning notification: "Performance regression detected: frame time increased by 35% (12ms -> 16.2ms)". Baselines update automatically after each session (rolling average of last 5 sessions). The comparison only runs in debug builds or when explicitly enabled. A "Performance Health" indicator in the status bar shows green (healthy), yellow (warning), red (regression).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (modify)

**Acceptance Criteria:**
- Regression detected when metric degrades >20%
- Warning notification includes specific metric and delta
- Baselines use rolling average of 5 sessions
- Status bar shows health indicator
- Only active in debug/developer mode

**Dependencies:** Tasks 01, 06, Phase 43 (Notification System)

---

### Task 19 -- Implement Startup Time Breakdown Visualization

**Description:** Create a visual breakdown of startup time as a horizontal bar chart in the dashboard.

**Implementation Details:** In the "Startup" tab of the dashboard, render a horizontal stacked bar chart showing each startup phase as a colored segment. Phases: Init (gray), Config (blue), Theme (purple), Extensions (orange), Window (green), FirstRender (teal). Each segment is labeled with phase name and duration. The total bar width represents 100% of startup time. Hovering over a segment shows detailed timing in a tooltip. Below the chart, show individual extension load times as a sorted list.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.cpp` (modify)

**Acceptance Criteria:**
- Horizontal bar chart renders all phases
- Segments are colored and labeled
- Hover tooltip shows detailed timing
- Extension load times listed below
- Total startup time shown

**Dependencies:** Task 06

---

### Task 20 -- Implement Memory Usage Graph

**Description:** Create a line graph showing memory usage over time (last 5 minutes) with trend line.

**Implementation Details:** In the "Memory" tab of the dashboard, render a line graph. X-axis: time (last 5 minutes, 1-second intervals). Y-axis: memory in MB (auto-scale). Line: current RSS. Fill below line with semi-transparent color. Trend line (dashed): linear regression of the last 5 minutes. If trend line shows consistent growth (>0.5 MB/min), display warning: "Memory appears to be growing". Below the graph, show: current RSS, peak RSS, trend (growing/stable/shrinking), virtual memory size.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.cpp` (modify)

**Acceptance Criteria:**
- Line graph shows last 5 minutes of memory usage
- Auto-scaling Y-axis adapts to usage range
- Trend line shows growth direction
- Warning displayed for sustained growth
- Current, peak, and trend values shown

**Dependencies:** Task 04

---

### Task 21 -- Implement EventBus Throughput Graph

**Description:** Create a graph showing EventBus events per second over time.

**Implementation Details:** In the "Events" tab of the dashboard, render a bar graph showing events/second for the last 60 seconds. Each bar represents one second. Bar color: green for <100 events/s, yellow for 100-500, red for >500. Below the graph, show: current events/s, average events/s, peak events/s, slow event count. A "Top Events" table shows the most frequently published event types and their average subscriber count. This helps identify chatty subsystems.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.cpp` (modify)

**Acceptance Criteria:**
- Bar graph shows last 60 seconds of throughput
- Color coding based on event rate
- Top events table shows frequent types
- Average and peak values displayed
- Slow event count shown prominently

**Dependencies:** Task 03

---

### Task 22 -- Implement File Cache Statistics Display

**Description:** Display file cache statistics in the dashboard: hit rate, cache size, most accessed files, eviction rate.

**Implementation Details:** In the "Cache" tab of the dashboard, show: current hit rate percentage (with color: green >80%, yellow 60-80%, red <60%), total cache entries, cache memory usage, eviction count. Below, a table of the 10 most frequently accessed files with hit count and miss count per file. A "Clear Cache" button allows resetting the cache. Cache efficiency recommendations appear below the statistics: "Cache size may be too small" if hit rate <60%.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.cpp` (modify)

**Acceptance Criteria:**
- Hit rate displayed with color coding
- Cache size and memory usage shown
- Top 10 accessed files listed
- "Clear Cache" button works
- Efficiency recommendations shown for low hit rates

**Dependencies:** Task 05

---

### Task 23 -- Add Dashboard Commands to Command Palette

**Description:** Register all performance dashboard commands.

**Implementation Details:** Register commands: `developer.openPerformanceDashboard`, `developer.togglePaintRegions`, `developer.togglePerformanceProfiling`, `developer.exportPerformanceReport`, `developer.clearPerformanceData`. All categorized under "Developer" in the Command Palette. Add a developer submenu in the Help menu with these commands. Commands are only visible when `developer.showDeveloperTools` is enabled in settings (default false for normal users, true for debug builds).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/Command.cpp` (modify)

**Acceptance Criteria:**
- All 5 commands appear in Command Palette (when developer mode enabled)
- Commands categorized under "Developer"
- Help menu includes Developer submenu
- Commands hidden for non-developer users
- Debug builds show commands by default

**Dependencies:** Tasks 08, 11, 16, 17

---

### Task 24 -- Implement Performance Dashboard Accessibility

**Description:** Ensure the Performance Dashboard is accessible: all metrics are readable by screen readers, graphs have text descriptions.

**Implementation Details:** Each metric card has an accessible name: "Frame Time: 12.5 milliseconds average". Graphs have accessible descriptions: "Frame time graph showing last 300 frames, average 12.5ms, 95th percentile 16.2ms". Tab panels have correct roles. The Paint Region overlay announces "Paint regions overlay enabled/disabled". All buttons and toggles in the dashboard have accessible labels. Data tables use correct table roles and column headers.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.cpp` (modify)

**Acceptance Criteria:**
- Screen reader reads all metric values
- Graph descriptions provide summary statistics
- Tab panels have correct roles
- All interactive elements have labels
- Paint region toggle is announced

**Dependencies:** Task 08

---

### Task 25 -- Add CMake Integration and Unit Tests

**Description:** Add all new performance monitoring files to CMakeLists.txt and create unit tests.

**Implementation Details:** Add all new .h/.cpp files to CMakeLists.txt. Create `test_performance_dashboard.cpp` with sections: (1) PerformanceCounters ring buffer wraps correctly at 300 samples, (2) Frame time averages calculated correctly, (3) EventBus throughput counter increments atomically, (4) Memory delta detection flags >10MB growth, (5) File cache hit rate calculation, (6) StartupTimeline records phase durations, (7) Slow event detection at 5ms threshold, (8) Performance report JSON export includes all fields, (9) Regression detection flags >20% degradation, (10) Layout reflow counter tracks per-frame counts.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (modify)
- `/Users/ryanrentfro/code/markamp/tests/unit/test_performance_dashboard.cpp` (create)

**Acceptance Criteria:**
- `cmake --build build/debug` compiles without errors
- All 10 test sections pass
- Counter and buffer tests are deterministic
- No undefined symbol errors
- source_group entries match add_executable

**Dependencies:** Tasks 01-24

## Completion Gates

- All 25 tasks executed or explicitly deferred with rationale
- Frame time graph renders in real time at 60fps
- Memory usage graph shows 5-minute history
- EventBus throughput and slow events tracked
- Startup timeline shows phase breakdown
- Extension load times tracked and displayed
- Paint region overlay visualizes repaints
- Performance report exports as JSON
- Regression alerts detect performance degradation
- `cmake --build build/debug -j$(sysctl -n hw.ncpu)` succeeds
- `cd build/debug && ctest --output-on-failure` passes
