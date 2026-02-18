# Phase 41: Canvas Input Model and Tool State Machine

## Overview
Establish a deterministic input pipeline for Canvas tools so pointer, keyboard, and gesture behavior is consistent and predictable. This phase explicitly excludes collaboration and sharing.

## Prerequisites
- `src/canvas/CanvasInputManager.cpp`
- `src/canvas/CanvasTool.h`

## Tasks

### Task 1: Define canonical tool state machine
**Files:** `src/canvas/CanvasTool.h`, `src/canvas/CanvasInputManager.cpp`
**Description:** Standardize idle, hover, pressed, drag, commit, cancel states for all tools.
**Acceptance Criteria:**
- Every tool follows the same lifecycle callbacks
- Cancel path always restores pre-action state

### Task 2: Normalize pointer event routing
**Files:** `src/ui/CanvasPanel.cpp`, `src/canvas/CanvasInputManager.cpp`
**Description:** Unify hit target, capture, and propagation order.
**Acceptance Criteria:**
- No duplicate dispatch for single pointer event
- Pointer capture is released safely on cancel/escape

### Task 3: Add modifier key contract
**Files:** `src/canvas/CanvasInputManager.cpp`
**Description:** Define Shift/Ctrl/Cmd/Alt behavior for constrain, duplicate, additive select.
**Acceptance Criteria:**
- Modifier semantics are documented and consistent across tools

### Task 4: Add tool change safety policy
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/canvas/CanvasInputManager.cpp`
**Description:** Prevent state corruption when switching tools mid-gesture.
**Acceptance Criteria:**
- Tool switch finalizes or cancels active gesture deterministically

### Task 5: Add input contract tests
**Files:** `tests/unit/test_canvas_input_state_machine.cpp`
**Description:** Validate transition rules and cancel behavior.
**Acceptance Criteria:**
- Invalid transitions fail tests

## Testing Requirements
- Mouse, trackpad, and keyboard modifier smoke tests
- Tool switch during active drag tests

## Phase Completion Criteria
- Canvas input behavior is deterministic and tool-agnostic
