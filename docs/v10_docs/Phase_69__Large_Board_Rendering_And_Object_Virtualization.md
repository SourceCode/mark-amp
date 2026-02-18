# Phase 69: Large Board Rendering and Object Virtualization

## Overview
Improve rendering fundamentals to keep interaction smooth on very large boards.

## Prerequisites
- `src/canvas/CanvasRenderer.*`
- `src/canvas/Quadtree.*`
- `src/canvas/TileCache.*`

## Tasks

### Task 1: Optimize spatial culling
**Files:** `src/canvas/Quadtree.cpp`, `src/canvas/CanvasRenderer.cpp`
**Description:** Reduce draw and hit-test work to visible regions.
**Acceptance Criteria:**
- Offscreen objects do not contribute to frame cost

### Task 2: Add LOD rendering policy
**Files:** `src/canvas/CanvasRenderer.cpp`
**Description:** Simplify object detail at distant zoom levels.
**Acceptance Criteria:**
- LOD transitions are visually stable without popping artifacts

### Task 3: Improve tile cache strategy
**Files:** `src/canvas/TileCache.cpp`
**Description:** Tune cache eviction and reuse for pan/zoom workflows.
**Acceptance Criteria:**
- Cache thrash reduced during rapid navigation

### Task 4: Add render telemetry counters
**Files:** `src/canvas/CanvasRenderer.cpp`, `src/core/Profiler.*`
**Description:** Track frame time, culled object count, and draw calls.
**Acceptance Criteria:**
- Metrics exposed for performance tuning

### Task 5: Add large-board performance tests
**Files:** `tests/unit/test_canvas_large_board_performance.cpp`
**Description:** Benchmark representative heavy boards.
**Acceptance Criteria:**
- Performance budgets are defined and validated

## Testing Requirements
- 10k+ object synthetic board tests
- Continuous pan/zoom interaction profiling

## Phase Completion Criteria
- Large board interaction remains smooth and responsive
