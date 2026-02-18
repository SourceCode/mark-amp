# Phase 25: Canvas Tooling and Inspector Control UX

## Overview
Improve canvas controls so object creation, manipulation, and property editing are faster and clearer.

## Prerequisites
- Existing canvas UI in `src/ui/CanvasPanel.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
- Phase 09

## Tasks

### Task 1: Redesign Canvas Tool Strip
**Files:** `src/ui/CanvasPanel.cpp`, `src/canvas/CanvasInputManager.cpp`
**Description:** Group tools by category with active-state clarity.
**Acceptance Criteria:**
- Tool strip supports keyboard selection and clear active indicator

### Task 2: Add Contextual Inspector Panel
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/canvas/*.cpp`
**Description:** Show object properties with type-specific controls.
**Acceptance Criteria:**
- Inspector updates immediately based on selection type

### Task 3: Add Transform and Alignment Control Cluster
**Files:** `src/ui/CanvasPanel.cpp`, `src/canvas/AlignmentGuides.cpp`
**Description:** Add explicit controls for align, distribute, layer order, and lock.
**Acceptance Criteria:**
- Transform controls reduce reliance on hidden shortcuts

### Task 4: Improve Multi-Select Action UX
**Files:** `src/canvas/SelectionManager.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Show selection count and available group actions.
**Acceptance Criteria:**
- Multi-select actions are discoverable and reversible

### Task 5: Add Canvas Control Tests
**Files:** `tests/unit/test_canvas_control_ux.cpp`
**Description:** Validate tool switching, inspector updates, and transform actions.
**Acceptance Criteria:**
- Canvas control behavior has regression coverage

## Testing Requirements
- Mixed object-type selection scenarios
- Canvas zoom levels and control usability checks

## Phase Completion Criteria
- Canvas controls are easier to discover and operate
