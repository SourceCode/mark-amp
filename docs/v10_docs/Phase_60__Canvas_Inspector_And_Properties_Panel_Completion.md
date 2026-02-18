# Phase 60: Canvas Inspector and Properties Panel Completion

## Overview
Build a complete, context-sensitive inspector so object properties are quickly editable and clearly grouped.

## Prerequisites
- `src/ui/CanvasWorkspacePanel.cpp`
- `src/canvas/MetadataPanel.*`

## Tasks

### Task 1: Add object-type aware inspector sections
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/canvas/MetadataPanel.cpp`
**Description:** Render relevant controls per selected object type.
**Acceptance Criteria:**
- Irrelevant controls are hidden; relevant controls are grouped logically

### Task 2: Add multi-select property editing
**Files:** `src/canvas/MetadataPanel.cpp`, `src/canvas/SelectionManager.cpp`
**Description:** Apply shared property changes across selected objects.
**Acceptance Criteria:**
- Conflicting values show mixed-state indicators

### Task 3: Add reset and style-copy controls
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/canvas/CanvasCommands.cpp`
**Description:** Provide quick reset-to-default and copy/apply style actions.
**Acceptance Criteria:**
- Style copy/apply workflow is one-click from inspector

### Task 4: Add property validation and constraints
**Files:** `src/canvas/MetadataPanel.cpp`, `src/core/ValidationUtils.*`
**Description:** Enforce numeric and logical limits for properties.
**Acceptance Criteria:**
- Invalid values are blocked with clear inline guidance

### Task 5: Add inspector tests
**Files:** `tests/unit/test_canvas_inspector.cpp`
**Description:** Validate rendering and value application logic.
**Acceptance Criteria:**
- Inspector behavior for single/multi-select is regression-tested

## Testing Requirements
- Rapid selection changes while inspector is open
- Mixed object-type multi-select edit checks

## Phase Completion Criteria
- Canvas inspector is complete, fast, and trustworthy
