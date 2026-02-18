# Phase 65: Pan Zoom and Camera Ergonomics

## Overview
Refine camera controls for natural movement, precision zooming, and reduced motion fatigue.

## Prerequisites
- `src/canvas/ViewportTransform.*`
- `src/canvas/PanTool.*`

## Tasks

### Task 1: Normalize zoom behavior around cursor
**Files:** `src/canvas/ViewportTransform.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Keep cursor-focused zoom anchor consistent.
**Acceptance Criteria:**
- Zoom-in/out keeps target area stable on screen

### Task 2: Add smooth pan velocity controls
**Files:** `src/canvas/PanTool.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Tune pan drag and inertial behavior for trackpad/mouse.
**Acceptance Criteria:**
- Pan movement is responsive without overshoot

### Task 3: Add zoom presets and fit modes
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/canvas/BoardNavigator.cpp`
**Description:** Provide 50/100/200%, fit board, fit selection actions.
**Acceptance Criteria:**
- Presets are accessible via toolbar and shortcuts

### Task 4: Add camera boundaries and recovery
**Files:** `src/canvas/ViewportTransform.cpp`
**Description:** Prevent lost-camera states and add quick recenter.
**Acceptance Criteria:**
- Users can always recover to a known visible region

### Task 5: Add camera tests
**Files:** `tests/unit/test_canvas_camera.cpp`
**Description:** Validate transform math and zoom anchor correctness.
**Acceptance Criteria:**
- Camera calculations are deterministic and stable

## Testing Requirements
- Continuous zoom/pan stress sequences
- Different input device behavior checks

## Phase Completion Criteria
- Camera controls feel natural and precise
