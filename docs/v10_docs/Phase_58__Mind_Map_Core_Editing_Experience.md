# Phase 58: Mind Map Core Editing Experience

## Overview
Polish mind map fundamentals: node creation, keyboard-driven expansion, and layout stability.

## Prerequisites
- `src/canvas/MindMapController.*`
- `src/canvas/MindMapNode.*`

## Tasks

### Task 1: Improve node creation shortcuts
**Files:** `src/canvas/MindMapController.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Add enter/tab shortcuts for sibling/child node creation.
**Acceptance Criteria:**
- Node authoring is efficient without toolbar dependency

### Task 2: Add branch layout controls
**Files:** `src/canvas/MindMapController.cpp`, `src/canvas/LayoutEngine.cpp`
**Description:** Support radial and directional layout options.
**Acceptance Criteria:**
- Layout updates maintain readable spacing

### Task 3: Add collapse/expand branch behavior
**Files:** `src/canvas/MindMapNode.cpp`, `src/canvas/MindMapController.cpp`
**Description:** Improve branch toggles for dense maps.
**Acceptance Criteria:**
- Collapse state persists and is reflected visually

### Task 4: Add branch style presets
**Files:** `src/canvas/MindMapNode.h`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Apply color and connector style by branch depth.
**Acceptance Criteria:**
- Style presets are optional and quickly switchable

### Task 5: Add mind map tests
**Files:** `tests/unit/test_canvas_mindmap.cpp`
**Description:** Validate editing operations and layout constraints.
**Acceptance Criteria:**
- Mind map interactions remain stable under rapid edits

## Testing Requirements
- Very large map expansion/collapse tests
- Keyboard-only map editing workflows

## Phase Completion Criteria
- Mind map core editing reaches fast, reliable baseline
