# Phase 45: Text Editing and Typography on Canvas

## Overview
Make Canvas text editing robust with better typography controls, inline editing, and text-object ergonomics.

## Prerequisites
- `src/canvas/TextBox.*`
- `src/canvas/TextBoxRenderer.*`

## Tasks

### Task 1: Improve inline text editing flow
**Files:** `src/ui/CanvasPanel.cpp`, `src/canvas/TextBox.cpp`
**Description:** Add predictable enter/edit/commit/cancel behavior.
**Acceptance Criteria:**
- Double-click and keyboard edit entry paths behave consistently

### Task 2: Add typography controls
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/canvas/TextBox.h`
**Description:** Support font family, size, weight, color, align, line spacing.
**Acceptance Criteria:**
- Inspector exposes full text styling set

### Task 3: Add auto-size and fixed-box modes
**Files:** `src/canvas/TextBox.cpp`, `src/canvas/TextBoxRenderer.cpp`
**Description:** Toggle between content-fit and fixed-wrap boxes.
**Acceptance Criteria:**
- Wrap and resize behavior is deterministic

### Task 4: Add text style presets
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/core/Config.cpp`
**Description:** Save/load heading/body/callout presets.
**Acceptance Criteria:**
- Presets can be applied to selection and new text objects

### Task 5: Add text object tests
**Files:** `tests/unit/test_canvas_text.cpp`
**Description:** Validate layout, wrapping, and serialization.
**Acceptance Criteria:**
- Rendering and edit edge cases covered

## Testing Requirements
- Long text and mixed-style scenarios
- Unicode and RTL baseline compatibility checks

## Phase Completion Criteria
- Canvas text editing is production-grade and predictable
