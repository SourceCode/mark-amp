# Phase 62: Canvas Context Menus and Quick Actions

## Overview
Improve right-click and quick actions for object, multi-select, and empty-space workflows.

## Prerequisites
- Phase 61
- `src/ui/CanvasPanel.cpp`

## Tasks

### Task 1: Define context action taxonomy
**Files:** `src/ui/CanvasPanel.cpp`, `src/canvas/CanvasCommands.cpp`
**Description:** Standardize action order and grouping by selection state.
**Acceptance Criteria:**
- Menus are predictable across object types

### Task 2: Add object-type specific quick actions
**Files:** `src/ui/CanvasPanel.cpp`
**Description:** Surface relevant actions (crop image, edit text, reroute connector, etc.).
**Acceptance Criteria:**
- Type-specific actions appear only when valid

### Task 3: Add multi-select bulk actions
**Files:** `src/ui/CanvasPanel.cpp`, `src/canvas/GroupingService.cpp`
**Description:** Support align, distribute, group, lock, and style apply actions.
**Acceptance Criteria:**
- Bulk actions are fully undoable

### Task 4: Add empty-space create shortcuts
**Files:** `src/ui/CanvasPanel.cpp`, `src/canvas/CanvasCommands.cpp`
**Description:** Expose quick create actions in empty area menu.
**Acceptance Criteria:**
- Empty-space menu improves first-action discoverability

### Task 5: Add context action tests
**Files:** `tests/unit/test_canvas_context_actions.cpp`
**Description:** Verify action availability and dispatch logic.
**Acceptance Criteria:**
- Context action regressions caught in CI

## Testing Requirements
- Keyboard context menu key behavior
- Selection-state transition checks

## Phase Completion Criteria
- Canvas context actions are coherent and high-signal
