# Phase 13: Status Bar as Actionable Control Rail

## Overview
Transform the status bar into a reliable quick-action control rail without adding clutter.

## Prerequisites
- Phase 02
- Phase 05

## Tasks

### Task 1: Define Status Item Priority and Grouping
**Files:** `src/ui/StatusBarPanel.cpp`, `src/ui/StatusBarPanel.h`
**Description:** Separate passive info from actionable controls with stable ordering.
**Acceptance Criteria:**
- Item order remains predictable during state changes

### Task 2: Add Consistent Click Targets
**Files:** `src/ui/StatusBarPanel.cpp`
**Description:** Ensure clickable items have clear hover/focus/pressed behavior.
**Acceptance Criteria:**
- Clickable status items are visually distinct and keyboard reachable

### Task 3: Add Quick Menus for EOL, Encoding, Indent, Zoom
**Files:** `src/ui/StatusBarPanel.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Standardize popup menus and command routing.
**Acceptance Criteria:**
- Menus support mouse and keyboard invocation

### Task 4: Improve Progress and Background Job Feedback
**Files:** `src/ui/StatusBarPanel.cpp`, `src/core/Events.h`
**Description:** Clarify active job status, cancellation affordance, and completion transitions.
**Acceptance Criteria:**
- Background tasks can expose progress and optional cancel action

### Task 5: Add Status Bar Tests
**Files:** `tests/unit/test_status_bar_actions.cpp`
**Description:** Cover item layout, hit-testing, and action dispatch.
**Acceptance Criteria:**
- Item interaction contract tested and stable

## Testing Requirements
- Tooltips and actions for all clickable status items
- Small-window truncation behavior checks

## Phase Completion Criteria
- Status bar acts as a compact, dependable action surface
