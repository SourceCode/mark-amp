# Phase 42: Freehand Drawing Engine Quality

## Overview
Improve freehand drawing to feel natural, low-latency, and visually clean with robust stroke editing fundamentals.

## Prerequisites
- Phase 41
- `src/canvas/DrawTool.cpp`
- `src/canvas/FreehandPath.cpp`

## Tasks

### Task 1: Improve stroke sampling and smoothing
**Files:** `src/canvas/DrawTool.cpp`, `src/canvas/FreehandPath.cpp`
**Description:** Use adaptive point sampling and smoothing based on speed.
**Acceptance Criteria:**
- Slow strokes preserve precision
- Fast strokes remain smooth without jagged corners

### Task 2: Add pressure-aware architecture (future-proof)
**Files:** `src/canvas/DrawTool.h`, `src/canvas/CanvasTypes.h`
**Description:** Add optional pressure channel support even if devices do not provide it yet.
**Acceptance Criteria:**
- Stroke data model supports variable width input

### Task 3: Add eraser and partial-stroke erase
**Files:** `src/canvas/DrawTool.cpp`, `src/canvas/SelectionManager.cpp`
**Description:** Support object erase and segment erase modes.
**Acceptance Criteria:**
- Erase operations are undoable and non-destructive to unrelated objects

### Task 4: Add stroke style presets
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/canvas/FreehandPath.h`
**Description:** Provide quick presets for pen, marker, highlighter.
**Acceptance Criteria:**
- Presets persist and are quickly switchable via toolbar/tool rail

### Task 5: Add freehand regression tests
**Files:** `tests/unit/test_canvas_freehand.cpp`
**Description:** Validate path integrity, smoothing bounds, and serialization.
**Acceptance Criteria:**
- Freehand rendering and undo/redo pass deterministic tests

## Testing Requirements
- Latency profiling while drawing continuously
- Zoom-level rendering quality checks

## Phase Completion Criteria
- Freehand drawing is stable, expressive, and performant
