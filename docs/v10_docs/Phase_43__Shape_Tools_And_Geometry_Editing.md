# Phase 43: Shape Tools and Geometry Editing

## Overview
Strengthen core shape creation and editing so rectangles, circles, polygons, arrows, and lines behave predictably and edit cleanly.

## Prerequisites
- Phase 41
- `src/canvas/ShapeData.*`
- `src/canvas/ShapeRenderer.*`

## Tasks

### Task 1: Expand shape primitive set
**Files:** `src/canvas/ShapeData.h`, `src/canvas/ShapeData.cpp`
**Description:** Add missing essential primitives and consistent defaults.
**Acceptance Criteria:**
- Core primitive library covers typical whiteboarding workflows

### Task 2: Add geometry handles and anchor editing
**Files:** `src/canvas/SelectionRenderer.cpp`, `src/canvas/ShapeData.cpp`
**Description:** Support resize, corner radius, and vertex edits.
**Acceptance Criteria:**
- Handle behavior is consistent and snap-aware

### Task 3: Add constrained draw modes
**Files:** `src/canvas/DrawTool.cpp`, `src/canvas/CanvasInputManager.cpp`
**Description:** Shift-constrain proportions/angles during draw and resize.
**Acceptance Criteria:**
- Constrain behavior matches user expectations across shapes

### Task 4: Add style inheritance for new shapes
**Files:** `src/canvas/CanvasCommands.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** New objects inherit last-used style unless overridden.
**Acceptance Criteria:**
- Style carryover is predictable and resettable

### Task 5: Add shape edit tests
**Files:** `tests/unit/test_canvas_shapes.cpp`
**Description:** Validate geometry editing invariants.
**Acceptance Criteria:**
- Shape bounds/transform integrity verified by tests

## Testing Requirements
- High zoom and low zoom handle interaction checks
- Constrained drag behavior tests

## Phase Completion Criteria
- Shape tools feel precise and complete for core workflows
