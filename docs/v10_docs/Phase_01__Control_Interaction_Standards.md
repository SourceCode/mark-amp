# Phase 01: Control Interaction Standards

## Overview
Define a single interaction standard for all controls so clicks, hovers, focus, disabled states, and press behavior are consistent across the application.

## Prerequisites
- Existing v9 control surfaces in `src/ui`

## Tasks

### Task 1: Create Control Behavior Spec
**Files:** `docs/v10_docs/ux_control_spec.md`
**Description:** Define canonical behavior for button, toggle, menu item, tab, list row, tree row, and split handle.
**Acceptance Criteria:**
- Every control type has states: default, hover, pressed, focused, disabled, selected
- Keyboard and mouse behavior defined for each control type

### Task 2: Create Reusable State Helpers
**Files:** `src/ui/ControlState.h`, `src/ui/ControlState.cpp`
**Description:** Add shared helpers for hit-testing and state transitions to reduce duplicated control logic.
**Acceptance Criteria:**
- ActivityBar, TabBar, Toolbar, and StatusBarPanel can consume shared state helpers
- No direct duplicated hover/press transition logic in at least 3 target controls

### Task 3: Normalize Cursor Policy
**Files:** `src/ui/ThemeAwareWindow.h`, `src/ui/ThemeAwareWindow.cpp`, `src/ui/*.cpp`
**Description:** Standardize when hand, arrow, I-beam, and resize cursors appear.
**Acceptance Criteria:**
- Interactive controls consistently use hand cursor
- Resize surfaces consistently use resize cursors

### Task 4: Add Interaction Audit Tooling
**Files:** `tests/unit/test_control_interaction_contracts.cpp`
**Description:** Add tests for basic state transitions and focus behavior for shared controls.
**Acceptance Criteria:**
- Contract tests verify hover, pressed, and focus transitions
- Tests fail when controls skip required states

### Task 5: Publish Adoption Checklist
**Files:** `docs/v10_docs/control_adoption_checklist.md`
**Description:** Create a checklist used by each following phase.
**Acceptance Criteria:**
- Checklist includes behavior, visual, keyboard, and accessibility checks

## Testing Requirements
- Unit tests for shared state helpers
- Manual verification on macOS, Windows, Linux

## Phase Completion Criteria
- Canonical interaction standard exists and is implemented for at least 4 major controls
