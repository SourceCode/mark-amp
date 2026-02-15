# Phase 32: Health Monitoring Panel

## Metadata

| Field | Value |
|---|---|
| Phase ID | 32 |
| Prerequisites | Phase 09 |
| Estimated Complexity | Medium |
| Estimated File Count | 3 created, 4 modified |
| PRD Sections | 4.3 Health Monitoring Panel, 10 Observability Layer |

---

## Objective

Create an internal diagnostics panel (hidden by default) showing real-time performance metrics including frame latency, memory usage, active plugins, event queue depth, and UI stall count.

---

## Background

The PRD requires: "Hidden performance panel, Frame time histogram, Memory usage graph." And section 4.3: "Internal diagnostics panel showing: Frame latency histogram, Memory usage, Active plugins, Event queue depth, Extension CPU usage, Snapshot generation age." This panel provides developers and advanced users with real-time insight into application health.

---

## Scope

### Tasks

1. **Create `src/ui/HealthPanel.h` / `HealthPanel.cpp`**:
   - wxPanel-derived class, themed via ThemeEngine
   - Sections:
     - **Frame Latency Histogram**: bar chart of last 100 frame times (uses existing FrameHistogram data)
     - **Memory Usage**: current RSS, peak RSS, budget limit, percentage bar
     - **Active Plugins**: list of active plugins with activation time
     - **Event Queue Depth**: current pending event count in EventBus
     - **Extension CPU Usage**: per-extension timing (if Phase 33 available, placeholder otherwise)
     - **Snapshot Generation Age**: time since last document snapshot
     - **UI Stall Count**: cumulative stalls detected by watchdog (if Phase 31 available)
   - Auto-refresh at 1Hz (timer-based, not every frame — avoids impacting frame budget)
   - Sized to fit in a side panel or floating window

2. **Register in Command Palette**:
   - Command: "Developer: Open Health Panel"
   - Toggle visibility on repeat invocation
   - Add to View menu under "Developer Tools" submenu

3. **Wire data sources**:
   - FrameHistogram: read from existing profiler/frame timing infrastructure
   - Memory: query allocator stats (mimalloc from Phase 10, or system RSS query)
   - Active plugins: query PluginManager
   - Event queue depth: query EventBus pending count
   - UI stall count: query Watchdog (Phase 31) if available

4. **Style with ThemeEngine**:
   - Use themed colors for histogram bars
   - Green/yellow/red for budget compliance
   - Monospace font for numeric values

5. **Create `tests/unit/test_health_panel.cpp`**:
   - Test data binding: set known values, verify display values match
   - Test refresh timer fires at expected interval
   - Test panel creation and destruction (no leaks)
   - Test with missing data sources (graceful null handling)

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/ui/HealthPanel.h` |
| Create | `src/ui/HealthPanel.cpp` |
| Modify | `src/ui/MainFrame.cpp` |
| Modify | `src/app/MarkAmpApp.cpp` |
| Modify | `src/CMakeLists.txt` |
| Create | `tests/unit/test_health_panel.cpp` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- Use `wxTimer` for the 1Hz refresh. The timer callback queries all data sources and updates UI elements.
- The histogram can be rendered using `wxDC::DrawRectangle()` calls with color-coded bars (green <8ms, yellow 8-16ms, red >16ms).
- Memory query on macOS: `task_info()` with `TASK_BASIC_INFO` gives resident size. On Linux: read `/proc/self/status` for VmRSS. On Windows: `GetProcessMemoryInfo()`.
- If mimalloc is available (Phase 10), use `mi_heap_get_used()` for more accurate allocation tracking.
- The panel should handle missing data sources gracefully — if Watchdog isn't initialized yet, show "N/A" for stall count.
- Consider using `wxCollapsiblePane` for each section to allow user customization.

---

## Acceptance Criteria

- [ ] Panel opens via Command Palette: "Developer: Open Health Panel"
- [ ] Shows live frame time histogram with color coding
- [ ] Shows current memory usage with percentage of budget
- [ ] Shows active plugin count and list
- [ ] Shows event queue depth
- [ ] Refresh rate is 1Hz (does not impact frame budget)
- [ ] `test_health_panel` validates data binding
- [ ] Panel is themed correctly (dark/light mode)
- [ ] Panel handles missing data sources without crashing

---

## Testing Strategy

- Run test_health_panel for data binding validation
- Manual test: open Health Panel via Command Palette, verify data updates
- Verify refresh rate is 1Hz (not faster, which would waste CPU)
- Verify panel does not impact typing latency when open
- Test with various themes for visual correctness
