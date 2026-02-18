# Phase 48: Selection Transform and Alignment Core

## Overview
Harden selection and transform fundamentals for single and multi-object operations.

## Prerequisites
- Phase 43
- `src/canvas/SelectionManager.*`

## Tasks

### Task 1: Standardize selection semantics
**Files:** `src/canvas/SelectionManager.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Align click, shift-click, marquee, and lasso rules.
**Acceptance Criteria:**
- Selection model is consistent across tools

### Task 2: Improve transform handles
**Files:** `src/canvas/SelectionRenderer.cpp`
**Description:** Add clearer handles for move, scale, rotate.
**Acceptance Criteria:**
- Handle hit targets are reliable at varied zoom levels

### Task 3: Add alignment and distribution commands
**Files:** `src/canvas/AlignmentGuides.cpp`, `src/canvas/CanvasCommands.cpp`
**Description:** Support align edges/centers and distribute spacing.
**Acceptance Criteria:**
- Multi-object alignment results are mathematically stable

### Task 4: Add transform origin and snapping controls
**Files:** `src/canvas/SnapEngine.cpp`, `src/canvas/SelectionManager.cpp`
**Description:** Allow precise transform around configurable pivots.
**Acceptance Criteria:**
- Transform operations can snap to grid/guides/objects

### Task 5: Add selection/transform tests
**Files:** `tests/unit/test_canvas_selection_transform.cpp`
**Description:** Validate invariants for rotate/scale/move sequences.
**Acceptance Criteria:**
- No object corruption after repeated transforms

## Testing Requirements
- Mixed object-type transform tests
- Large multi-select operations

## Phase Completion Criteria
- Selection and transform controls are precise and stable
