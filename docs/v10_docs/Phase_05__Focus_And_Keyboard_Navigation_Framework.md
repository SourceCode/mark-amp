# Phase 05: Focus and Keyboard Navigation Framework

## Overview
Establish a reliable keyboard-first navigation model for all primary control surfaces.

## Prerequisites
- Phase 01
- Phase 04

## Tasks

### Task 1: Define Global Focus Order
**Files:** `src/ui/MainFrame.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Implement predictable Tab/Shift+Tab traversal between chrome, activity bar, sidebar, editor, panels, and status bar.
**Acceptance Criteria:**
- Focus traversal is deterministic and cyclical

### Task 2: Add Per-Surface Arrow Key Contracts
**Files:** `src/ui/ActivityBar.cpp`, `src/ui/FileTreeCtrl.cpp`, `src/ui/TabBar.cpp`, `src/ui/SettingsPanel.cpp`
**Description:** Ensure arrow navigation semantics are consistent for list/tree/strip controls.
**Acceptance Criteria:**
- Arrow behavior matches control type conventions

### Task 3: Add Focus Restoration Rules
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/MainFrame.cpp`
**Description:** Return focus to prior logical control after dialogs/panels close.
**Acceptance Criteria:**
- Closing overlays restores prior focus target

### Task 4: Render Strong Focus Indicators
**Files:** `src/ui/*.cpp`
**Description:** Draw token-based focus rings for all keyboard focusable custom controls.
**Acceptance Criteria:**
- Focus always visible and distinct from hover

### Task 5: Add Keyboard Navigation Tests
**Files:** `tests/unit/test_keyboard_navigation_controls.cpp`
**Description:** Add regression tests for focus and activation behavior.
**Acceptance Criteria:**
- Coverage includes activity bar, tab bar, tree, settings, status items

## Testing Requirements
- Keyboard-only workflow run-through
- Focus ring visual audit across themes

## Phase Completion Criteria
- Users can operate primary controls without a mouse
