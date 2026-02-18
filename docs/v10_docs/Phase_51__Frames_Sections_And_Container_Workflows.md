# Phase 51: Frames Sections and Container Workflows

## Overview
Improve structural organization controls with better frames, sections, and container behaviors for large boards.

## Prerequisites
- Phase 50
- `src/canvas/FrameObject.*`
- `src/canvas/SectionObject.*`

## Tasks

### Task 1: Strengthen frame semantics
**Files:** `src/canvas/FrameObject.cpp`, `src/canvas/SectionObject.cpp`
**Description:** Define containment rules for move, resize, duplicate, and export.
**Acceptance Criteria:**
- Containment behavior is deterministic and reversible

### Task 2: Add frame title and styling controls
**Files:** `src/canvas/FrameRenderer.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Support frame labels, tint, border presets.
**Acceptance Criteria:**
- Frame styling controls are exposed in inspector

### Task 3: Add frame-based navigation actions
**Files:** `src/canvas/BoardNavigator.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Quick jump to previous/next frame.
**Acceptance Criteria:**
- Frame navigation is keyboard addressable

### Task 4: Add container-aware select/move options
**Files:** `src/canvas/SelectionManager.cpp`
**Description:** Toggle select-children vs container-only behavior.
**Acceptance Criteria:**
- Selection mode is explicit and predictable

### Task 5: Add frame/section tests
**Files:** `tests/unit/test_canvas_frames_sections.cpp`
**Description:** Validate containment, serialization, and navigation.
**Acceptance Criteria:**
- Structure integrity preserved across edits

## Testing Requirements
- Nested container scenarios
- Large board navigation through frames

## Phase Completion Criteria
- Structural organization controls scale to large canvases
