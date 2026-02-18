# Phase 23: Editor Inline Control Overlays

## Overview
Refine inline editor controls (format bar, link/image/table popovers) so they are contextual, stable, and keyboard-operable.

## Prerequisites
- Phase 05
- Existing overlay components in `src/ui`

## Tasks

### Task 1: Standardize Overlay Positioning Rules
**Files:** `src/ui/FloatingFormatBar.cpp`, `src/ui/LinkPreviewPopover.cpp`, `src/ui/ImagePreviewPopover.cpp`
**Description:** Prevent clipping, overlap, and off-screen placement.
**Acceptance Criteria:**
- Overlays reposition safely in all viewport edges

### Task 2: Add Overlay Focus Trap and Escape Semantics
**Files:** `src/ui/FloatingFormatBar.cpp`, `src/ui/TableEditorOverlay.cpp`
**Description:** Ensure predictable keyboard control entry and exit.
**Acceptance Criteria:**
- Escape closes overlay and restores prior focus

### Task 3: Improve Action Density and Grouping
**Files:** `src/ui/FloatingFormatBar.cpp`, `src/ui/TableEditorOverlay.cpp`
**Description:** Reorganize controls by action group and frequency.
**Acceptance Criteria:**
- Most-used actions reachable in one interaction

### Task 4: Add Inline State Feedback
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/FloatingFormatBar.cpp`
**Description:** Show selected format states and unavailable actions.
**Acceptance Criteria:**
- Control states mirror editor selection context

### Task 5: Add Overlay Tests
**Files:** `tests/unit/test_editor_overlays.cpp`
**Description:** Test positioning, focus behavior, and action dispatch.
**Acceptance Criteria:**
- Overlay behavior is stable under resize/scroll

## Testing Requirements
- Selection changes while overlay open
- Multi-monitor window boundary checks

## Phase Completion Criteria
- Inline overlays are reliable, contextual, and usable
