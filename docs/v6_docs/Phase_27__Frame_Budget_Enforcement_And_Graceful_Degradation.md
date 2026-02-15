# Phase 27: Frame Budget Enforcement & Graceful Degradation

## Metadata

| Field | Value |
|---|---|
| Phase ID | 27 |
| Prerequisites | Phase 24 |
| Estimated Complexity | High |
| Estimated File Count | 1 created, 5 modified |
| PRD Sections | 7.1 Frame Budget |

---

## Objective

Wire the existing FrameBudgetToken and DegradationLevel infrastructure into the actual UI rendering loop. Subsystems bail early when the frame budget is exhausted, degrading gracefully instead of dropping frames.

---

## Background

The PRD specifies: "UI target: <16ms per frame, soft limit: 8ms for editor operations." The codebase already has `FrameBudgetToken`, `DegradationLevel`, `AdaptiveThrottle`, and `FrameScheduler` infrastructure, but they may not be fully wired into the rendering pipeline. This phase connects these components to enforce the frame budget.

---

## Scope

### Tasks

1. **Integrate FrameBudgetToken into paint cycle**:
   - Create `FrameBudgetToken` at the start of each frame (idle handler entry)
   - Pass token to all rendering subsystems
   - Each subsystem checks `token.is_exhausted()` before expensive work:
     - Syntax highlighting: stop highlighting beyond visible range
     - Minimap: skip minimap update
     - Preview panel: defer preview regeneration
     - Diagnostics overlay: skip if budget exhausted

2. **Implement degradation levels**:
   - `DegradationLevel::Full`: all rendering features active
   - `DegradationLevel::Reduced`: skip minimap, defer off-screen syntax highlighting, reduce preview quality
   - `DegradationLevel::Minimal`: plain text rendering only, no overlays, no preview update
   - Level determined by frame budget utilization over last 10 frames
   - Auto-upgrade when frame budget consistently under-utilized
   - Auto-downgrade when frame budget consistently exceeded

3. **Wire AdaptiveThrottle into idle handler**:
   - Typing mode (keyboard activity within last 100ms): 4ms budget for rendering
   - Idle mode (no keyboard activity): 16ms budget for rendering
   - Transition smoothly between modes

4. **Integrate FrameScheduler priority queue**:
   - Input processing: highest priority (always runs)
   - Paint operations: second priority
   - Background work (highlighting, preview): lowest priority
   - Background work only executes when budget remains after paint
   - Tasks that miss their deadline are carried to next frame

5. **Create `tests/unit/test_frame_budget_enforcement.cpp`**:
   - Test FrameBudgetToken exhaustion detection
   - Test degradation level transitions
   - Test AdaptiveThrottle mode switching
   - Test FrameScheduler priority ordering
   - Test that input events are never dropped

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/ui/EditorPanel.cpp` |
| Modify | `src/ui/PreviewPanel.cpp` |
| Modify | `src/app/MarkAmpApp.cpp` |
| Modify | `src/core/FrameBudgetToken.h` |
| Create | `tests/unit/test_frame_budget_enforcement.cpp` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- `FrameBudgetToken` should use `std::chrono::steady_clock::now()` for timing. The `is_exhausted()` check compares current time against the deadline.
- Degradation level decisions should be based on a rolling window (e.g., last 10 frames) to avoid oscillation. Use hysteresis: downgrade after 3 consecutive over-budget frames, upgrade after 10 consecutive under-budget frames.
- AdaptiveThrottle: detect keyboard activity via event timestamp. If the last keyboard event was within 100ms, assume typing mode. This is a heuristic — may need tuning.
- FrameScheduler: use a simple priority queue. Each task has a priority (0=input, 1=paint, 2=background) and a time estimate. Tasks are dequeued and executed until budget exhaustion.
- Input processing must NEVER be deferred, even under extreme load. It goes directly to the event handler, bypassing the scheduler.

---

## Acceptance Criteria

- [ ] Frame time never exceeds 20ms (16ms + 4ms grace) during sustained typing
- [ ] Degradation levels are observable under artificial load (inject slow rendering)
- [ ] Input events are never dropped due to rendering overrun
- [ ] `test_frame_budget_enforcement` validates budget checking logic
- [ ] FrameScheduler processes tasks in priority order
- [ ] AdaptiveThrottle correctly switches between typing and idle modes
- [ ] Degradation auto-recovers when load decreases

---

## Testing Strategy

- Run test_frame_budget_enforcement for unit validation
- Run bench_rendering_pipeline (Phase 18) to verify frame times
- Manual test: type rapidly and verify responsiveness
- Inject artificial rendering delay and verify degradation kicks in
