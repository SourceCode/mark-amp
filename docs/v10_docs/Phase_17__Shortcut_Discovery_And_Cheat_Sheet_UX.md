# Phase 17: Shortcut Discovery and Cheat Sheet UX

## Overview
Improve shortcut discovery through stronger overlay UX and in-context hints.

## Prerequisites
- Phase 10
- Existing `src/ui/ShortcutOverlay.cpp`

## Tasks

### Task 1: Rework Overlay Layout and Search
**Files:** `src/ui/ShortcutOverlay.cpp`
**Description:** Improve category organization, filtering relevance, and scanability.
**Acceptance Criteria:**
- Search produces ranked, highlighted results

### Task 2: Add Context-Aware Shortcut Subsets
**Files:** `src/ui/ShortcutOverlay.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Show shortcuts relevant to current workbench mode first.
**Acceptance Criteria:**
- Overlay defaults to mode-relevant shortcuts

### Task 3: Add "Where Used" Command Links
**Files:** `src/ui/ShortcutOverlay.cpp`, `src/ui/CommandPalette.cpp`
**Description:** Allow jumping from shortcut entry to command palette item.
**Acceptance Criteria:**
- Shortcut entries can invoke command details/actions

### Task 4: Add Tooltip Shortcut Standardization
**Files:** `src/ui/*.cpp`
**Description:** Ensure key controls display shortcut hints consistently.
**Acceptance Criteria:**
- Tooltip format is consistent across controls

### Task 5: Add Shortcut Overlay Tests
**Files:** `tests/unit/test_shortcut_overlay.cpp`
**Description:** Verify filter logic, category rendering, and activation behavior.
**Acceptance Criteria:**
- Overlay behavior is regression-protected

## Testing Requirements
- Keyboard-only overlay usage
- Platform shortcut rendering validation

## Phase Completion Criteria
- Shortcut discovery is fast and context-relevant
