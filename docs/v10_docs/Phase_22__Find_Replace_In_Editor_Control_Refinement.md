# Phase 22: Find Replace in Editor Control Refinement

## Overview
Improve the in-editor find/replace controls for speed, clarity, and fewer accidental operations.

## Prerequisites
- Phase 21
- Existing editor find actions in `src/ui/EditorPanel.cpp`

## Tasks

### Task 1: Redesign Inline Find Widget
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/EditorPanel.h`
**Description:** Improve layout, compact controls, and state indicators.
**Acceptance Criteria:**
- Widget clearly shows match count and current index

### Task 2: Add Replace Safety Controls
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Distinguish replace one, replace all, and selection-only replace.
**Acceptance Criteria:**
- Replace-all requires explicit context and undo confidence

### Task 3: Add Search Scope Shortcuts
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/MainFrame.cpp`
**Description:** Add quick toggles for selection/document/all open files.
**Acceptance Criteria:**
- Scope toggles are keyboard accessible and visible

### Task 4: Improve Match Highlighting and Navigation
**Files:** `src/ui/EditorPanel.cpp`, `src/rendering/SelectionPainter.h`
**Description:** Clarify current vs non-current match styling.
**Acceptance Criteria:**
- Current match is always visually distinct

### Task 5: Add Find/Replace Tests
**Files:** `tests/unit/test_editor_find_replace_controls.cpp`
**Description:** Cover widget interactions, scopes, and replace actions.
**Acceptance Criteria:**
- Find/replace regressions are caught in CI

## Testing Requirements
- Multi-cursor and selection mode interactions
- Large document find performance check

## Phase Completion Criteria
- Find/replace control UX supports fast, safe editing
