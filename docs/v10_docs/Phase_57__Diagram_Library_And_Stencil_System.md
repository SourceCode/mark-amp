# Phase 57: Diagram Library and Stencil System

## Overview
Expand diagram fundamentals with reusable stencils, categories, and insertion workflows.

## Prerequisites
- `src/canvas/DiagramLibraryPanel.*`
- `src/canvas/DiagramShapeObject.*`

## Tasks

### Task 1: Define stencil taxonomy
**Files:** `src/canvas/IconLibrary.cpp`, `src/canvas/DiagramLibraryPanel.cpp`
**Description:** Organize stencils by flowchart, architecture, UI, and general symbols.
**Acceptance Criteria:**
- Users can browse and search stencils quickly

### Task 2: Add drag-insert and click-insert modes
**Files:** `src/canvas/DiagramLibraryPanel.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Support both direct placement and drag placement.
**Acceptance Criteria:**
- Inserted stencils adopt default style profile consistently

### Task 3: Add favorite/recent stencil controls
**Files:** `src/canvas/DiagramLibraryPanel.cpp`, `src/core/Config.cpp`
**Description:** Provide quick access to frequently used shapes.
**Acceptance Criteria:**
- Favorites and recents persist per user profile

### Task 4: Add style overrides at insert time
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/canvas/DiagramShapeObject.cpp`
**Description:** Allow optional insert-time color/stroke presets.
**Acceptance Criteria:**
- Insert-time style selection does not break default flow

### Task 5: Add stencil tests
**Files:** `tests/unit/test_canvas_stencils.cpp`
**Description:** Validate taxonomy, insertion, and persistence.
**Acceptance Criteria:**
- Stencil insertion is deterministic and undoable

## Testing Requirements
- Large stencil set search and scroll checks
- Insert workflow speed tests

## Phase Completion Criteria
- Diagram library is fast, organized, and practical
