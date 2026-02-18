# Phase 49: Snapping Grid and Guide Precision

## Overview
Refine grid and snapping behavior for precision placement without friction.

## Prerequisites
- Phase 48
- `src/canvas/SnapEngine.*`
- `src/canvas/CanvasRenderer.*`

## Tasks

### Task 1: Expand snap target model
**Files:** `src/canvas/SnapEngine.cpp`, `src/canvas/SnapEngine.h`
**Description:** Add snap-to-grid, snap-to-object edges, centers, and key points.
**Acceptance Criteria:**
- Snap priority rules are deterministic and configurable

### Task 2: Improve guide rendering
**Files:** `src/canvas/AlignmentGuides.cpp`, `src/canvas/CanvasRenderer.cpp`
**Description:** Render transient guides with clear visual hierarchy.
**Acceptance Criteria:**
- Guides are visible but non-intrusive

### Task 3: Add smart spacing hints
**Files:** `src/canvas/AlignmentGuides.cpp`
**Description:** Show equal-spacing hints during move/resize.
**Acceptance Criteria:**
- Spacing suggestions appear only when relevant

### Task 4: Add grid customization controls
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/canvas/CanvasRenderer.h`
**Description:** Expose spacing, style, major line interval, visibility toggles.
**Acceptance Criteria:**
- Grid settings persist per board

### Task 5: Add snap precision tests
**Files:** `tests/unit/test_canvas_snapping.cpp`
**Description:** Validate snap outcomes and tolerance bounds.
**Acceptance Criteria:**
- Snap behavior remains deterministic under transforms

## Testing Requirements
- Snap at extreme zoom in/out
- Dense object layout edge cases

## Phase Completion Criteria
- Placement precision matches advanced whiteboard expectations
