# Phase 09: Toolbar Action Surface Redesign

## Overview
Redesign toolbar controls around high-value actions, mode context, and clear affordances.

## Prerequisites
- Phase 03
- Phase 06

## Tasks

### Task 1: Define Contextual Action Slots
**Files:** `src/ui/Toolbar.h`, `src/ui/Toolbar.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Split toolbar into global actions and mode-specific action slots.
**Acceptance Criteria:**
- Toolbar updates actions when workbench mode changes

### Task 2: Improve Button Semantics and States
**Files:** `src/ui/Toolbar.cpp`
**Description:** Normalize toggle vs momentary button behavior and visuals.
**Acceptance Criteria:**
- Toggle actions show explicit on/off state

### Task 3: Add Overflow and Customization
**Files:** `src/ui/Toolbar.cpp`, `src/core/Config.cpp`
**Description:** Add overflow menu and user-configurable visible actions.
**Acceptance Criteria:**
- Hidden actions remain discoverable via overflow
- Toolbar layout persists across sessions

### Task 4: Add Shortcut and Secondary-Action Hints
**Files:** `src/ui/Toolbar.cpp`, `src/ui/MainFrame.cpp`
**Description:** Tooltips include shortcuts and alternate-click behavior where applicable.
**Acceptance Criteria:**
- Every toolbar action has a descriptive tooltip

### Task 5: Add Toolbar Tests
**Files:** `tests/unit/test_toolbar_behavior.cpp`
**Description:** Test context switching, toggle semantics, and overflow rendering logic.
**Acceptance Criteria:**
- Contextual action sets validate against active mode

## Testing Requirements
- Mode-switch action surface tests
- Toolbar compact-mode visual checks

## Phase Completion Criteria
- Toolbar is contextual, customizable, and easier to learn
