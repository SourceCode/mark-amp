# Phase 50: Layering Z-Order and Locking Fundamentals

## Overview
Complete foundational layer controls including ordering, visibility, and locking for safer editing.

## Prerequisites
- Phase 48
- `src/canvas/LayeringService.*`
- `src/canvas/LockingService.*`

## Tasks

### Task 1: Add explicit z-order actions
**Files:** `src/canvas/LayeringService.cpp`, `src/canvas/CanvasCommands.cpp`
**Description:** Support bring forward/backward and send to front/back.
**Acceptance Criteria:**
- Z-order actions are undoable and command-palette addressable

### Task 2: Add lock/hidden object states
**Files:** `src/canvas/LockingService.cpp`, `src/canvas/SelectionManager.cpp`
**Description:** Prevent accidental edits to protected objects.
**Acceptance Criteria:**
- Locked/hidden objects communicate state clearly

### Task 3: Add layer mini-panel
**Files:** `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Provide a compact layer stack for selected region/board.
**Acceptance Criteria:**
- Users can reorder and lock/unlock from panel controls

### Task 4: Add selection filtering by lock/visibility
**Files:** `src/canvas/SelectionManager.cpp`
**Description:** Ensure selection ignores hidden and optionally locked objects.
**Acceptance Criteria:**
- Selection behavior respects layer state policy

### Task 5: Add layering tests
**Files:** `tests/unit/test_canvas_layering.cpp`
**Description:** Verify ordering and lock semantics.
**Acceptance Criteria:**
- Layer state survives save/load and undo/redo

## Testing Requirements
- Complex board with many overlapping objects
- Hit-test correctness with hidden/locked objects

## Phase Completion Criteria
- Layer controls are robust and prevent accidental edits
