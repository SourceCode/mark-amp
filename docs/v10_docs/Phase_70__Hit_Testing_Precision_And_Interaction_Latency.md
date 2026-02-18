# Phase 70: Hit Testing Precision and Interaction Latency

## Overview
Increase hit-testing accuracy and reduce interaction latency for small handles and dense scenes.

## Prerequisites
- Phase 69
- `src/canvas/SelectionManager.*`

## Tasks

### Task 1: Improve hit-test priority resolution
**Files:** `src/canvas/SelectionManager.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Define stable precedence for handles, objects, and containers.
**Acceptance Criteria:**
- Intended target wins in dense overlap scenarios

### Task 2: Add pixel tolerance scaling by zoom
**Files:** `src/canvas/SelectionManager.cpp`, `src/canvas/ViewportTransform.cpp`
**Description:** Keep handle usability consistent at all zoom levels.
**Acceptance Criteria:**
- Tiny controls remain selectable without accidental misses

### Task 3: Add hover prefetch for likely targets
**Files:** `src/canvas/SelectionManager.cpp`, `src/canvas/Quadtree.cpp`
**Description:** Precompute nearest interactive candidates.
**Acceptance Criteria:**
- Hover-to-click latency is reduced under heavy object counts

### Task 4: Add latency budget instrumentation
**Files:** `src/ui/CanvasPanel.cpp`, `src/core/Profiler.*`
**Description:** Measure pointer-down to visual-feedback delay.
**Acceptance Criteria:**
- Interaction latency metrics are available for regression tracking

### Task 5: Add hit-test tests
**Files:** `tests/unit/test_canvas_hit_testing.cpp`
**Description:** Validate overlap and tolerance edge cases.
**Acceptance Criteria:**
- Hit-test outcomes are deterministic across runs

## Testing Requirements
- Dense overlapping object scenes
- Small-object precision tests

## Phase Completion Criteria
- Canvas interactions feel accurate and immediate
