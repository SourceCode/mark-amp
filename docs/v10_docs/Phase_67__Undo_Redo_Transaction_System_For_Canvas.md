# Phase 67: Undo Redo Transaction System for Canvas

## Overview
Strengthen undo/redo fundamentals with coherent transaction boundaries and high reliability.

## Prerequisites
- `src/canvas/UndoRedoStack.*`
- `src/canvas/CanvasCommands.*`

## Tasks

### Task 1: Define transaction boundary policy
**Files:** `src/canvas/UndoRedoStack.cpp`, `src/canvas/CanvasCommands.cpp`
**Description:** Group micro-edits into meaningful undo steps.
**Acceptance Criteria:**
- Undo steps map to user intent, not raw event count

### Task 2: Add compound action support
**Files:** `src/canvas/UndoRedoStack.h`, `src/canvas/CanvasCommands.cpp`
**Description:** Support begin/end transaction for multi-object operations.
**Acceptance Criteria:**
- Compound operations undo/redo atomically

### Task 3: Add history metadata and previews
**Files:** `src/canvas/UndoRedoStack.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Show readable history labels for recent actions.
**Acceptance Criteria:**
- Users can see what next undo/redo will do

### Task 4: Add memory budget controls
**Files:** `src/canvas/UndoRedoStack.cpp`, `src/core/Config.cpp`
**Description:** Prevent unbounded history memory growth.
**Acceptance Criteria:**
- History respects configured memory/step limits

### Task 5: Add undo/redo tests
**Files:** `tests/unit/test_canvas_undo_redo.cpp`
**Description:** Validate transaction atomicity and stack integrity.
**Acceptance Criteria:**
- No stack corruption under rapid action sequences

## Testing Requirements
- Long edit session stress tests
- Undo after complex grouped operations

## Phase Completion Criteria
- Undo/redo behavior is trustworthy at scale
