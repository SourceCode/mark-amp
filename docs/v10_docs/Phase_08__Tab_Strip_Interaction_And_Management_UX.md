# Phase 08: Tab Strip Interaction and Management UX

## Overview
Make tab management more legible and less error-prone through better controls for overflow, pinning, grouping, and drag behavior.

## Prerequisites
- Phase 02
- Phase 05

## Tasks

### Task 1: Improve Overflow Controls
**Files:** `src/ui/TabBar.cpp`, `src/ui/TabBar.h`
**Description:** Add explicit overflow dropdown with searchable tab list.
**Acceptance Criteria:**
- Users can quickly jump to hidden tabs from overflow control

### Task 2: Add Tab Group Operations
**Files:** `src/ui/TabBar.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Support group actions (close group, pin group, color group).
**Acceptance Criteria:**
- Group actions available in context menu and command palette

### Task 3: Refine Drag and Drop Feedback
**Files:** `src/ui/TabBar.cpp`
**Description:** Improve drag ghost, insertion marker, and cancel behavior.
**Acceptance Criteria:**
- Drag reorder feels stable and avoids accidental closes

### Task 4: Improve Modified and Conflict Indicators
**Files:** `src/ui/TabBar.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Distinguish dirty, externally changed, and conflict states.
**Acceptance Criteria:**
- Status indicators are visually distinct and tooltip-labeled

### Task 5: Add Tab UX Regression Tests
**Files:** `tests/unit/test_tab_bar_interactions.cpp`
**Description:** Add tests for reorder, close policies, overflow, and keyboard traversal.
**Acceptance Criteria:**
- Regressions in tab behavior are caught automatically

## Testing Requirements
- Mouse and keyboard tab workflows
- Overflow behavior at small window widths

## Phase Completion Criteria
- Tab strip supports heavy multi-file workflows cleanly
