# Phase 07: Explorer and File Tree Control Refinement

## Overview
Improve file tree controls for clarity, speed, and low-friction operations at large workspace scale.

## Prerequisites
- Phase 03
- Phase 05

## Tasks

### Task 1: Improve Tree Row Affordances
**Files:** `src/ui/FileTreeCtrl.cpp`
**Description:** Clarify hover/select/focus/active visuals and twisty hit zones.
**Acceptance Criteria:**
- Rows clearly communicate state without ambiguity

### Task 2: Upgrade Inline Rename/Create UX
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Add inline editing with validation and conflict messaging.
**Acceptance Criteria:**
- Rename/create flows are inline, validated, and cancellable

### Task 3: Add Bulk Tree Operations
**Files:** `src/ui/FileTreeCtrl.cpp`
**Description:** Support multi-select and bulk actions where safe.
**Acceptance Criteria:**
- Multi-select supports delete/move/reveal/copy path actions

### Task 4: Enhance Type-Ahead and Filter UX
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Improve filter highlighting, match count, and clear states.
**Acceptance Criteria:**
- Filter results and empty states are explicit and keyboard-friendly

### Task 5: Add Explorer Control Tests
**Files:** `tests/unit/test_file_tree_controls.cpp`
**Description:** Add tests for hit-testing, keyboard navigation, and filtering.
**Acceptance Criteria:**
- Critical interactions covered by tests

## Testing Requirements
- Large tree performance checks
- Keyboard + context menu workflow verification

## Phase Completion Criteria
- Explorer controls are fast, predictable, and easy to operate
