# Phase 52: Sticky Notes and Quick Idea Capture UX

## Overview
Polish sticky note workflows for rapid capture, grouping, and cleanup.

## Prerequisites
- `src/canvas/StickyNote.*`
- `src/canvas/StickyAggregator.*`

## Tasks

### Task 1: Improve sticky creation flow
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/canvas/CanvasCommands.cpp`
**Description:** Add one-click and keyboard quick-create with immediate edit focus.
**Acceptance Criteria:**
- New sticky creation requires minimal interactions

### Task 2: Add color and size presets
**Files:** `src/canvas/StickyNote.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Provide default styles for brainstorming patterns.
**Acceptance Criteria:**
- Presets persist and can be applied to multi-selection

### Task 3: Improve text overflow and resize behavior
**Files:** `src/canvas/StickyNoteRenderer.cpp`
**Description:** Ensure note content remains readable and clipped correctly.
**Acceptance Criteria:**
- Sticky content handles long text gracefully

### Task 4: Add smart clustering helpers
**Files:** `src/canvas/StickyAggregator.cpp`
**Description:** Offer optional grouping suggestions based on proximity/tag.
**Acceptance Criteria:**
- Suggestions are non-destructive and undoable

### Task 5: Add sticky note tests
**Files:** `tests/unit/test_canvas_sticky_notes.cpp`
**Description:** Validate creation, styling, and aggregation helpers.
**Acceptance Criteria:**
- Sticky workflows pass deterministic tests

## Testing Requirements
- High-count sticky board performance checks
- Keyboard-only capture and edit flow tests

## Phase Completion Criteria
- Sticky note fundamentals are fast and production-ready
