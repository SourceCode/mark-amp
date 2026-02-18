# Phase 73: Canvas Accessibility and Assistive Navigation

## Overview
Improve Canvas accessibility fundamentals for keyboard, focus visibility, and assistive semantics.

## Prerequisites
- Phase 72
- `src/ui/CanvasPanel.cpp`

## Tasks

### Task 1: Add accessible object navigation model
**Files:** `src/ui/CanvasPanel.cpp`, `src/canvas/SelectionManager.cpp`
**Description:** Enable logical traversal of objects by keyboard.
**Acceptance Criteria:**
- Users can traverse and select objects without pointer input

### Task 2: Add focus ring and selection contrast improvements
**Files:** `src/canvas/SelectionRenderer.cpp`, `src/canvas/CanvasRenderer.cpp`
**Description:** Ensure focus and selection are visually distinct.
**Acceptance Criteria:**
- Focus remains visible across themes and zoom levels

### Task 3: Add assistive labels for object metadata
**Files:** `src/canvas/CanvasObject.h`, `src/canvas/MetadataPanel.cpp`
**Description:** Expose readable names/types for selected objects.
**Acceptance Criteria:**
- Object names and types are available to assistive systems

### Task 4: Add reduced-motion handling for canvas interactions
**Files:** `src/ui/CanvasPanel.cpp`, `src/core/SettingsCatalog.cpp`
**Description:** Remove non-essential animation in reduced motion mode.
**Acceptance Criteria:**
- Reduced motion mode affects camera/selection animations

### Task 5: Add accessibility tests
**Files:** `tests/unit/test_canvas_accessibility.cpp`
**Description:** Validate focus traversal and semantic announcements.
**Acceptance Criteria:**
- Accessibility regressions are caught in CI

## Testing Requirements
- Keyboard-only usability pass
- Contrast and reduced-motion checks

## Phase Completion Criteria
- Canvas fundamentals meet accessibility baseline expectations
