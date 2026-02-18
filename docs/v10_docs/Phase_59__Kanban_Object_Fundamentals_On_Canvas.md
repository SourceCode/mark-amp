# Phase 59: Kanban Object Fundamentals on Canvas

## Overview
Complete kanban-on-canvas basics for lanes, cards, movement, and visual clarity. Collaboration features remain out of scope.

## Prerequisites
- `src/canvas/KanbanController.*`
- `src/canvas/KanbanObjects.*`

## Tasks

### Task 1: Improve lane and card creation controls
**Files:** `src/canvas/KanbanController.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Add quick create actions and inline card editing.
**Acceptance Criteria:**
- Lane/card creation is low-friction and keyboard-friendly

### Task 2: Add drag reorder behavior
**Files:** `src/canvas/KanbanController.cpp`, `src/canvas/SelectionManager.cpp`
**Description:** Support card reorder within and across lanes.
**Acceptance Criteria:**
- Drag preview and drop targets are clear and stable

### Task 3: Add card style and priority controls
**Files:** `src/canvas/KanbanObjects.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Support labels, color coding, and due date display fields.
**Acceptance Criteria:**
- Card metadata presentation remains readable at common zooms

### Task 4: Add lane layout constraints
**Files:** `src/canvas/LayoutEngine.cpp`, `src/canvas/KanbanController.cpp`
**Description:** Keep lane geometry consistent during board resize.
**Acceptance Criteria:**
- Layout avoids overlap and clipping during edits

### Task 5: Add kanban tests
**Files:** `tests/unit/test_canvas_kanban.cpp`
**Description:** Validate creation, reorder, and serialization.
**Acceptance Criteria:**
- Kanban object model remains robust under frequent changes

## Testing Requirements
- High card-count lane stress tests
- Keyboard drag alternative actions

## Phase Completion Criteria
- Kanban fundamentals support practical planning workflows
