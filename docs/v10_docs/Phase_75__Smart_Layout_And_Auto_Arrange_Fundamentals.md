# Phase 75: Smart Layout and Auto Arrange Fundamentals

## Overview
Add practical auto-layout helpers for common board cleanup and structure tasks.

## Prerequisites
- `src/canvas/LayoutEngine.*`
- Phase 48

## Tasks

### Task 1: Implement auto-align and tidy actions
**Files:** `src/canvas/LayoutEngine.cpp`, `src/canvas/CanvasCommands.cpp`
**Description:** Add tidy-up actions for selected objects.
**Acceptance Criteria:**
- Tidy operations preserve object order and are undoable

### Task 2: Add spacing normalization command
**Files:** `src/canvas/LayoutEngine.cpp`
**Description:** Normalize horizontal/vertical spacing among selection.
**Acceptance Criteria:**
- Spacing output is deterministic and previewable

### Task 3: Add flow layout presets
**Files:** `src/canvas/LayoutEngine.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Support row, column, and grid arrangement presets.
**Acceptance Criteria:**
- Preset layout operations complete quickly on large selections

### Task 4: Add container-aware layout behavior
**Files:** `src/canvas/LayoutEngine.cpp`, `src/canvas/FrameObject.cpp`
**Description:** Respect frame boundaries during auto-layout.
**Acceptance Criteria:**
- Layout actions avoid unintended spillover across containers

### Task 5: Add auto-layout tests
**Files:** `tests/unit/test_canvas_auto_layout.cpp`
**Description:** Validate arrangement algorithms and invariants.
**Acceptance Criteria:**
- Layout operations produce stable, expected results

## Testing Requirements
- Large selection auto-arrange benchmarks
- Edge cases with mixed object sizes

## Phase Completion Criteria
- Auto-arrange controls speed up board cleanup significantly
